#include <my_stl.h>
#include <windows.h>
#include <stdio.h>

#include <hash_map>
#include <list>

#include "XZip.h"
#include "FileUtils.h"

#include "zlib\contrib\minizip\unzip.h"
#include "zlib\contrib\minizip\zip.h"

using namespace file_utils;

class ZLibArchive : public XZipArchive
{
public:
	ZLibArchive(bool handle_errors = false);
	~ZLibArchive();

	bool open(const char* name, XZipOpenMode mode = XZS_IN);
	bool isOpen() const { if(isOutput()) return zipHandle_ != 0; else return !index_.empty(); }
	bool close();

	bool fileOpen(XZipStream& fh, const char* file_name, unsigned int flags = XZS_IN, int compression_level = 0);
	bool fileClose(XZipStream& fh);

	unsigned long fileRead(void* file_handle, void* buf, unsigned long len);
	unsigned long fileWrite(void* file_handle, const void* buf, unsigned long len);
	long fileSize(void* file_handle) const;
	long fileTell(void* file_handle) const; 
	bool fileEOF(void* file_handle) const;
	void fileTime(void* file_handle, unsigned& fdate, unsigned& ftime) const;
	long fileSeek(void* file_handle, long offset, int dir);

	virtual const char* convertFileName(const char* file_name) const;

private:

	unzFile zipHandle_;

	class ZLibFileInfo
	{
	public:
		ZLibFileInfo(const unz_file_info& info, const unz_file_pos& pos, const unz_file_seek_info& seek_info) : 
		  filePos_(pos), seekInfo_(seek_info) {
			  compressionMethod_ = info.compression_method;
			  dosDate_ = info.dosDate;
		  }

		unsigned int compressionMethod_;
		unsigned int dosDate_;

		unz_file_pos filePos_;
		unz_file_seek_info seekInfo_;
	};

	typedef std::hash_map<std::string, ZLibFileInfo, std::hash<std::string> > IndexMap;
	IndexMap index_;

	/// имя открытого в данный момент файла
	std::string fileName_;
	/// размер открытого в данный момент файла
	unsigned long fileSize_;
	/// дата открытого в данный момент файла
	unsigned long fileDate_;
};

static char* openMSG	 = "OPEN FAILURE (file don't exist or not valid)";
static char* openInMSG	 = "OPEN FAILURE FILE IN ARCHIVE";
static char* closeInMSG  = "CLOSE FAILURE FILE IN ARCHIVE";
static char* closeMSG	 = "CLOSE FAILURE";
static char* appendMSG	 = "APPENDING FAILURE";
static char* sizeMSG	 = "FILE SIZE CALCULATION ERROR";
static char* surfMSG	 = "SURFING IN ARCHIVE ERROR";
static char* locateMSG   = "LOCATE FILE ERROR";
static char* timeMSG	 = "FILE GET TIME ERROR";
static char* readMSG	 = "WRONG READING";
static char *seekMSG	 = "BAD SEEK";

static void zipError(const char* message, const char* subject = 0)
{
	std::string str = message;
	if(subject){
		str += "\n";
		str += subject;
	}

	MessageBox(0, message, "ZIP Error", MB_ICONERROR | MB_OK);
	exit(1);
}

// ----------------------------

XZipFileOpenMode XZipStream::openMode_ = XZIP_FILE_PRIORITY;

XZipStream::XZipStream(bool handle_errors) : archive_(0)
{
	handleErrors_ = handle_errors;

	directReadMode_ = false;
	directReadPosition_ = 0;
	directReadEOF_ = false;
	directReadFromArchive_ = false;
	directReadStartPosition_ = 0;
	directReadSize_ = 0;
	dosDate_ = 0;

	compressionLevel_ = 0;
}

XZipStream::~XZipStream()
{
	close();
}

XZipStream::XZipStream(const char* name, int flags, bool handle_errors, int compression_level) : archive_(0)
{
	handleErrors_ = handle_errors;

	directReadMode_ = false;
	directReadPosition_ = 0;
	directReadEOF_ = false;
	directReadFromArchive_ = false;
	directReadStartPosition_ = 0;
	directReadSize_ = 0;
	dosDate_ = 0;

	compressionLevel_ = compression_level;

	open(name, flags);
}

void XZipStream::gettime(unsigned& fdate,unsigned& ftime) const
{
	fdate = ftime = 0;

	if(directReadMode_){
		if(directReadFromArchive_){
			fdate = dosDate_ >> 16;
			ftime = dosDate_ & 0xFFFF;
		}
		else {
			unsigned short dt,tm;
			FILETIME ft;
			if(!GetFileTime(fileHandle_,0,0,&ft)){
				if(handleErrors_)
					zipError(timeMSG, GetFileName());
				else
					return;
			}

			if(!FileTimeToDosDateTime(&ft,&dt,&tm)){
				if(handleErrors_)
					zipError(timeMSG, GetFileName());
				else
					return;
			}
			fdate = dt;
			ftime = tm;
		}
	}
	else {
		if(archive_)
			archive_->fileTime(fileHandle_, fdate, ftime);
	}
}

bool XZipStream::open(const char* name, int flags)
{
	if(isOpen())
		close();

	if(flags & XZS_IN){
		switch(openMode_){
		case XZIP_ARCHIVE_ONLY:
			return openFromArchive(name, flags);
		case XZIP_ARCHIVE_PRIORITY:
			if(!openFromArchive(name, flags))
				return openDirect(name, flags);
			return true;
		case XZIP_FILE_PRIORITY:
			if(!openDirect(name, flags))
				return openFromArchive(name, flags);
			return true;
		case XZIP_FILE_ONLY:
			return openDirect(name, flags);
		}
	}
	else if(flags & XZS_OUT)
		return openFromArchive(name, flags);

	return false;
}

bool XZipStream::isOpen() const
{
	return directReadMode_ || (archive_ && fileHandle_ != INVALID_HANDLE_VALUE);
}

void XZipStream::close()
{
	if(directReadMode_){
		directReadPosition_ = 0;
		directReadEOF_ = false;
		directReadFromArchive_ = false;
		directReadStartPosition_ = 0;
		directReadSize_ = 0;
		dosDate_ = 0;

		directReadMode_ = false;

		if(fileHandle_ == INVALID_HANDLE_VALUE) return;

		if(!CloseHandle(fileHandle_) && handleErrors_)
			zipError(closeMSG);

		fileHandle_ = INVALID_HANDLE_VALUE;
	}
	else {
		if(archive_)
			archive_->fileClose(*this);

		archive_ = 0;
		fileHandle_ = INVALID_HANDLE_VALUE;
	}
}

long XZipStream::size() const
{
	if(directReadMode_){
		if(!directReadFromArchive_){
			long sz = GetFileSize(fileHandle_, 0);
			if(sz == -1L){
				if(handleErrors_) 
					zipError(sizeMSG);
				else 
					return -1;
			}
			return sz;
		}
		else
			return directReadSize_;
	}
	else {
		if(archive_)
			return archive_->fileSize(fileHandle_);
	}

	return 0;
}

long XZipStream::tell() const
{ 
	if(directReadMode_)
		return directReadPosition_;
	else {
		if(archive_)
			return archive_->fileTell(fileHandle_);
	}

	return 0;
}

long XZipStream::seek(long offset, int dir)
{
	if(directReadMode_){
		if(directReadFromArchive_){
			switch(dir){
			case XZS_BEG:
				offset += directReadStartPosition_;
				break;
			case XZS_END:
				offset = directReadStartPosition_ + directReadSize_ - offset;
				dir = XZS_BEG;
				break;
			}
		}

		long ret = SetFilePointer(fileHandle_, offset, 0, dir);
		if(ret == -1){
			if(handleErrors_)
				zipError(seekMSG, GetFileName());
			else
				return -1;
		}

		if(directReadFromArchive_)
			ret -= directReadStartPosition_;

		if(ret >= size() - 1)
			directReadEOF_ = true;
		else 
			directReadEOF_ = false;

		directReadPosition_ = ret;
		return ret;
	}
	else {
		if(archive_)
			return archive_->fileSeek(fileHandle_, offset, dir);
	}

	return -1;
}

bool XZipStream::eof() const
{
	if(directReadMode_)
		return directReadEOF_;
	else {
		if(archive_)
			return archive_->fileEOF(fileHandle_);
	}

	return false;
}

unsigned long XZipStream::read(void* buf, unsigned long len)
{
	if(directReadMode_){
		unsigned long ret;

		if(!ReadFile(fileHandle_, buf, len, &ret, 0)){
			if(handleErrors_)
				zipError(readMSG);
			else
				return 0;
		}

		directReadPosition_ += ret;

		if(ret < len)
			directReadEOF_ = true;

		return ret;
	}
	else {
		if(archive_)
			return archive_->fileRead(fileHandle_, buf, len);
	}

	return 0;
}

unsigned long XZipStream::write(const void* buf, unsigned long len)
{
	if(archive_)
		return archive_->fileWrite(fileHandle_, buf, len);

	return 0;
}

void XZipStream::setCompressionLevel(int level)
{
	compressionLevel_ = level;
	if(compressionLevel_ < 0)
		compressionLevel_ = 0;
	if(compressionLevel_ > 9)
		compressionLevel_ = 9;
}

XZipStream& XZipStream::operator> (char* v)
{
	read(v,(unsigned)strlen(v));
	return *this;
}

XZipStream& XZipStream::operator> (char& v)
{
	read(&v,(unsigned)sizeof(char));
	return *this;
}

XZipStream& XZipStream::operator> (unsigned char& v)
{
	read(&v,(unsigned)sizeof(char));
	return *this;
}

XZipStream& XZipStream::operator> (short& v)
{
	read(&v,(unsigned)sizeof(short));
	return *this;
}

XZipStream& XZipStream::operator> (unsigned short& v)
{
	read(&v,(unsigned)sizeof(unsigned short));
	return *this;
}

XZipStream& XZipStream::operator> (int& v)
{
	read(&v,(unsigned)sizeof(int));
	return *this;
}

XZipStream& XZipStream::operator> (unsigned& v)
{
	read(&v,(unsigned)sizeof(unsigned));
	return *this;
}

XZipStream& XZipStream::operator> (long& v)
{
	read(&v,(unsigned)sizeof(long));
	return *this;
}

XZipStream& XZipStream::operator> (unsigned long& v)
{
	read(&v,(unsigned)sizeof(unsigned long));
	return *this;
}

XZipStream& XZipStream::operator> (double& v)
{
	read(&v,(unsigned)sizeof(double));
	return *this;
}

XZipStream& XZipStream::operator> (float& v)
{
	read(&v,(unsigned)sizeof(float));
	return *this;
}

XZipStream& XZipStream::operator> (long double& v)
{
	read(&v,(unsigned)sizeof(long double));
	return *this;
}

bool XZipStream::openFromArchive(const char* file_name, int flags)
{
	if(!XZipArchiveManager::instance().fileOpen(*this, file_name, flags, compressionLevel_)){
		if(handleErrors_)
			zipError(openMSG, file_name);
		else
			return false;
	}
	
	return true;
}

bool XZipStream::openDirect(const char* file_name, int flags)
{
	fileHandle_ = CreateFile(file_name, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, 0);

	if(fileHandle_ == INVALID_HANDLE_VALUE){
		if(handleErrors_)
			zipError(openMSG, file_name);
		else
			return false;
	}

	SetFilePointer(fileHandle_, 0L, 0, FILE_CURRENT);
	directReadPosition_ = 0;
	directReadEOF_ = false;

	directReadMode_ = true;

	return true;
}

// -----------------------------------

XZipArchive::XZipArchive(bool handle_errors)
{
	handleErrors_ = handle_errors;
	mode_ = XZS_IN;
	compressionLevel_ = 0;
}

void XZipArchive::setCompressionLevel(int level)
{
	compressionLevel_ = level;
	if(compressionLevel_ < 0)
		compressionLevel_ = 0;
	if(compressionLevel_ > 9)
		compressionLevel_ = 9;
}

// -----------------------------------

ZLibArchive::ZLibArchive(bool handle_errors) : XZipArchive(handle_errors),
	zipHandle_(0)
{
	fileSize_ = 0;
	fileDate_ = 0;
}

ZLibArchive::~ZLibArchive()
{
	close();
}

bool ZLibArchive::open(const char* name, XZipOpenMode open_mode)
{
	close();

	mode_ = open_mode;

	if(mode_ == XZS_IN){
		zipHandle_ = unzOpen(name);
		if(!zipHandle_){
			if(handleErrors()) 
				zipError(openMSG, name);
			else
				return false;
		}

		int err = unzGoToFirstFile(zipHandle_);
		unz_file_pos pos;
		unz_file_info file_info;
		unz_file_seek_info seek_info;

		while(err == UNZ_OK){
			char fname[MAX_PATH];
			err = unzGetCurrentFileInfo(zipHandle_, &file_info, fname, MAX_PATH, 0, 0, 0, 0);
			if(err == UNZ_OK){
				err = unzGetFileSeekPos(zipHandle_, &pos, &seek_info);
				if(err == UNZ_OK){
					const char* file_name = convertFileName(fname);
					index_.insert(IndexMap::value_type(file_name, ZLibFileInfo(file_info, pos, seek_info)));
				}

				err = unzGoToNextFile(zipHandle_);
			}
		}
		unzClose(zipHandle_);
		zipHandle_ = 0;
	}
	else {
		if(mode_ == XZS_APPEND){
			zipHandle_ = zipOpen(name, APPEND_STATUS_ADDINZIP);
			if(!zipHandle_){
				zipHandle_ = zipOpen(name, APPEND_STATUS_CREATE);
				mode_ = XZS_OUT;
			}
		}
		else if(mode_ == XZS_OUT){
			zipHandle_ = zipOpen(name, APPEND_STATUS_CREATE);
		}

		if(!zipHandle_){
			if(handleErrors()) 
				zipError(openMSG, name);
			else
				return false;
		}
	}

	zipName_ = name;

	return true;
}

bool ZLibArchive::close()
{
	if(zipHandle_){
		if(isInput()){
			int err_code = unzClose(zipHandle_);
			if(err_code != UNZ_OK){
				if(handleErrors())
					zipError(closeMSG, zipName_.c_str());
				else
					return false;
			}
		}
		else {
			int err_code = zipClose(zipHandle_, 0);
			if(err_code){
				if(handleErrors())
					zipError(closeMSG, zipName_.c_str());
				else
					return false;
			}
		}
	}

	index_.clear();

	zipHandle_ = 0;
	zipName_ = "";
	return true;
}

bool ZLibArchive::fileOpen(XZipStream& fh, const char* file_name, unsigned int flags, int compression_level)
{
	if(!isOpen())
		return false;

	if(flags & XZS_IN){
		if(!isInput()){
			if(handleErrors()) 
				zipError(openMSG, file_name);
			else 
				return false;
		}

		const char* fname = convertFileName(file_name);

		IndexMap::iterator it = index_.find(fname);
		if(it != index_.end()){
			if(it->second.compressionMethod_){
				void* handle = unzOpen(zipName_.c_str());
				if(unzGoToFilePos(handle, &it->second.filePos_) != UNZ_OK){
					if(handleErrors())
						zipError(locateMSG, fname);
					else {
						unzClose(handle);
						return false;
					}
				}

				if(unzOpenCurrentFile(handle) != UNZ_OK){
					if(handleErrors()) 
						zipError(openMSG, zipName_.c_str());
					else {
						unzClose(handle);
						return false;
					}
				}

				fh.setZipHandle(handle, this);
				return true;
			}
			else {
				void* handle = CreateFile(zipName_.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, 0);

				if(handle == INVALID_HANDLE_VALUE){
					if(handleErrors())
						zipError(openMSG, zipName_.c_str());
					else
						return false;
				}

				SetFilePointer(handle, it->second.seekInfo_.seek_offset, 0, FILE_CURRENT);

				fh.fileHandle_ = handle;
				fh.directReadPosition_ = 0;
				fh.directReadEOF_ = false;

				fh.directReadStartPosition_ = it->second.seekInfo_.seek_offset;
				fh.directReadSize_ = it->second.seekInfo_.seek_size;

				fh.dosDate_ = it->second.dosDate_;

				fh.directReadMode_ = true;
				fh.directReadFromArchive_ = true;

				return true;
			}
		}
		else
			return false;
	}
	else if(flags & XZS_OUT){
		if(!isOutput()){
			if(handleErrors()) 
				zipError(openMSG, file_name);
			else 
				return false;
		}

		fileName_ = file_name;

		setCompressionLevel(compression_level);

		zip_fileinfo inf;
		memset(&inf, 0, sizeof(zip_fileinfo));

		SYSTEMTIME tm;
		GetLocalTime(&tm);

		inf.tmz_date.tm_sec = tm.wSecond;
		inf.tmz_date.tm_min = tm.wMinute;
		inf.tmz_date.tm_hour = tm.wHour;
		inf.tmz_date.tm_mday = tm.wDay;
		inf.tmz_date.tm_mon = tm.wMonth - 1;
		inf.tmz_date.tm_year = tm.wYear;

		if(!zipOpenNewFileInZip(zipHandle_, file_name, &inf, 0, 0, 0, 0, 0, compressionLevel() ? Z_DEFLATED : 0, compressionLevel())){
			fh.setZipHandle(zipHandle_, this);
			return true;
		}
	}

	return false;
}

bool ZLibArchive::fileClose(XZipStream& fh)
{
	if(isInput()){
		if(unzCloseCurrentFile(fh.fileHandle_) == UNZ_OK){
			unzClose(fh.fileHandle_);
			return true;
		}
	}
	else {
		if(!zipCloseFileInZip(fh.fileHandle_))
			return true;
	}

	if(handleErrors()) 
		zipError(closeInMSG, fileName_.c_str());

	return false;
}

unsigned long ZLibArchive::fileRead(void* file_handle, void* buf, unsigned long len)
{
	int ret = unzReadCurrentFile(file_handle, buf, len);
	if(ret > 0)
		return ret;
	else
		return 0;
}

unsigned long ZLibArchive::fileWrite(void* file_handle, const void* buf, unsigned long len)
{
	int ret = zipWriteInFileInZip(file_handle, buf, len);
	if(ret > 0)
		return ret;
	else
		return 0;
}

long ZLibArchive::fileSize(void* file_handle) const
{
	unz_file_info file_info;
	int err = unzGetCurrentFileInfo(file_handle, &file_info, 0, 0, 0, 0, 0, 0);
	if(err != UNZ_OK){
		if(handleErrors()) 
			zipError(openMSG, zipName_.c_str());
		else 
			return 0;
	}

	return file_info.uncompressed_size;
}

long ZLibArchive::fileTell(void* file_handle) const
{
	return unztell(file_handle);
}

bool ZLibArchive::fileEOF(void* file_handle) const
{
	return unzeof(file_handle) != 0;
}

void ZLibArchive::fileTime(void* file_handle, unsigned& fdate, unsigned& ftime) const
{
	unz_file_info file_info;
	int err = unzGetCurrentFileInfo(file_handle, &file_info, 0, 0, 0, 0, 0, 0);
	if(err != UNZ_OK){
		if(handleErrors()) 
			zipError(openMSG, zipName_.c_str());
		else 
			return;
	}

	uLong ulDosDate = file_info.dosDate;
	fdate = ulDosDate >> 16;
	ftime = ulDosDate & 0xFFFF;
}	

long ZLibArchive::fileSeek(void* file_handle, long offset, int dir)
{
	long pos = fileTell(file_handle);
	long dest_pos = pos;
	switch(dir){
	case XZS_BEG:
		dest_pos = offset;
		break;
	case XZS_CUR:
		dest_pos += offset;
		break;
	case XZS_END:
		dest_pos = fileSize(file_handle) - offset;
		break;
	}

	if(pos > dest_pos){
		if(unzOpenCurrentFile(file_handle) == UNZ_OK)
			pos = 0;
		else
			return -1;
	}

	if(pos < dest_pos){
		const int buf_size = 4096;
		static char buf[buf_size];
		long delta = dest_pos - pos;
		while(pos < dest_pos){
			pos += fileRead(file_handle, buf, (delta > buf_size) ? buf_size : delta);
			delta -= buf_size;
		}
		return pos;
	}
	else
		if(pos == dest_pos)
			return pos;

	return -1;
}

const char* ZLibArchive::convertFileName(const char* file_name) const
{
	static char out_name[MAX_PATH];
	strcpy(out_name, localizePath(file_name));

	char* ptr = out_name;

	int sz = (int)strlen(file_name);
	for(int i = 0; i < sz; i++, ptr++)
		if(*ptr == '\\') *ptr = '/';

	return out_name;
}

// -----------------------------------

XZipArchiveManager::XZipArchiveManager(bool handle_errors)
{
	handleErrors_ = handle_errors;
	archives_.reserve(32);

	outputArchive_ = createArchive();
}

XZipArchiveManager::~XZipArchiveManager()
{
/*	for(Archives::iterator it = archives_.begin(); it != archives_.end(); ++it)
		releaseArchive(*it);

	archives_.clear();

	releaseArchive(outputArchive_);*/
}

bool XZipArchiveManager::openArchive(const char* archive_name, XZipOpenMode mode)
{
	if(mode == XZS_IN){
		XZipArchive* p = createArchive();
		if(p->open(archive_name)){
			archives_.push_back(p);
			return true;
		}

		releaseArchive(p);
	}
	else {
		if(outputArchive_->open(archive_name, mode))
			return true;
	}

	return false;
}

bool XZipArchiveManager::closeArchive(const char* archive_name)
{
	for(Archives::iterator it = archives_.begin(); it != archives_.end(); ++it){
		if(!stricmp((*it)->zipName(), archive_name)){
			(*it)->close();
			return true;
		}
	}

	if(!stricmp(outputArchive_->zipName(), archive_name)){
		outputArchive_->close();
		return true;
	}

	return false;
}

bool XZipArchiveManager::registerArchive(XZipArchive* archive)
{
	Archives::iterator it = std::find(archives_.begin(),archives_.end(), archive);
	if(it != archives_.end())
		return false;

	archives_.push_back(archive);
	return true;
}

bool XZipArchiveManager::unregisterArchive(XZipArchive* archive)
{
	Archives::iterator it = std::find(archives_.begin(),archives_.end(), archive);
	if(it != archives_.end()){
		archives_.erase(it);
		return true;
	}

	return false;
}

bool XZipArchiveManager::fileOpen(XZipStream& fh, const char* file_name, unsigned int flags, int compression_level)
{
	if(flags & XZS_IN){
		for(Archives::iterator it = archives_.begin(); it != archives_.end(); ++it){
			if((*it)->fileOpen(fh, file_name, flags, compression_level))
				return true;
		}
	}
	else {
		if(outputArchive_->fileOpen(fh, file_name, flags, compression_level))
			return true;
	}

	return false;
}

XZipArchive* XZipArchiveManager::createArchive() const
{
	return new ZLibArchive(handleErrors_);
}

void XZipArchiveManager::releaseArchive(XZipArchive* archive) const
{
	delete archive;
}

XZipArchiveManager& XZipArchiveManager::instance()
{
	static XZipArchiveManager mgr;
	return mgr;
}

// -----------------------------------

XZipArchiveMaker::XZipArchiveMaker(const char* zip_name, const char* path, const char* files_mask, const char* exclude_files_mask, int compression, bool purge_files, ProgressCallback callback) : zipName_(zip_name),
	path_(path),
	filesMask_(files_mask),
	excludeFilesMask_(exclude_files_mask),
	compressionLevel_(compression),
	purgeFiles_(purge_files),
	progressCallback_(callback)
{
}

XZipArchiveMaker::~XZipArchiveMaker()
{
}

bool XZipArchiveMaker::buildArchive()
{
	makeFileLists();

	int cur_file = 0;
	int num_files = (int)files_.size();

	const int buf_size = 512 * 1024;
	char buf[buf_size];

	XZipFileOpenMode zip_mode = XZipStream::openMode();
	XZipStream::setOpenMode(XZIP_FILE_ONLY);

	XZipArchiveManager::instance().openArchive(zipName_.c_str(), XZS_OUT);

	for(FileNames::const_iterator it = files_.begin(); it != files_.end(); ++it){
		if(progressCallback_){
			int percents_done = cur_file * 100 / num_files;
			(*progressCallback_)(percents_done,it->c_str());
		}

		XZipStream fin(false);
		if(fin.open(it->c_str(), XZS_IN)){
			XZipStream fout(it->c_str(), XZS_OUT, false, compressionLevel_);

			int sz = fin.size();
			while(sz > 0){
				int delta = sz > buf_size ? buf_size : sz;
				fin.read(buf, delta);
				fout.write(buf, delta);
				sz -= delta;
			}

			fin.close();
			fout.close();

			if(purgeFiles_)
				DeleteFile(it->c_str());
		}

		cur_file++;
	}

//	removeEmptyDirectories(path_.c_str());

	XZipArchiveManager::instance().closeArchive(zipName_.c_str());
	XZipStream::setOpenMode(zip_mode);

	return true;
}

class XZipNameCompareOp
{
public:
	XZipNameCompareOp(const char* fname) : fileName_(fname) { }

	bool operator() (const std::string& str)
	{
		return !stricmp(str.c_str(), fileName_.c_str());
	}

private:
	std::string fileName_;
};

bool XZipArchiveMaker::makeFileLists()
{
	typedef std::list<std::string> FileList;
	FileList temp_list;

	if(!excludeFilesMask_.empty())
		getFileList(path_.c_str(), excludeFilesMask_.c_str(), temp_list);

	for(FileList::const_iterator it = temp_list.begin(); it != temp_list.end(); ++it)
		excludeFiles_.push_back(it->c_str());

	temp_list.clear();
	getFileList(path_.c_str(), filesMask_.c_str(), temp_list);

	files_.reserve(temp_list.size());
	for(FileList::const_iterator it = temp_list.begin(); it != temp_list.end(); ++it){
		FileNames::const_iterator ex = std::find_if(excludeFiles_.begin(), excludeFiles_.end(), XZipNameCompareOp(it->c_str()));
		if(ex == excludeFiles_.end())
			files_.push_back(it->c_str());
	}

	return true;
}
