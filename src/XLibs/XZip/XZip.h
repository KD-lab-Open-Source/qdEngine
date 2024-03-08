#ifndef __XZIP_H__
#define __XZIP_H__

#include <vector>
#include <string>

class XZipArchive;

/// режимы для XZipStream::open()
enum XZipFileOpenMode
{
	XZIP_ARCHIVE_ONLY,		///< читать только из архива
	XZIP_ARCHIVE_PRIORITY,	///< сначала искать файл в архиве, если отсутствует открывать напрямую
	XZIP_FILE_PRIORITY,		///< сначала открывать файл напрямую, если отсутствует искать в архиве
	XZIP_FILE_ONLY			///< читать только из файла
};

/// режимы для XZipStream::seek()
/// должны совпадать с режимами XStream::seek() для совместимости кода
enum XZipSeekMode
{
	XZS_BEG = 0,
	XZS_CUR	= 1,
	XZS_END = 2
};

/// режимы для XZipStream::open()
/// должны совпадать с режимами XStream::open() для совместимости кода
/// одновременное чтение и запись (XZS_IN | XZS_OUT) не поддерживается
enum XZipOpenMode
{
	XZS_IN		= 0x0001,
	XZS_OUT		= 0x0002,
	XZS_APPEND	= 0x0008
};

class XZipStream
{
public:
	XZipStream(bool handle_errors = true);
	XZipStream(const char* name, int flags, bool handle_errors = false, int compression_level = 0);
	~XZipStream();

	bool open(const char* name, int flags = XZS_IN);
	bool isOpen() const;
	void close();

	bool handleErrors() const { return handleErrors_; }
	void setHandleErrors(bool err = true) { handleErrors_ = err; }

	const char* GetFileName() const { return fileName_.c_str(); }
	void gettime(unsigned& fdate,unsigned& ftime) const;

	long size() const;
	long tell() const; 
	long seek(long offset, int dir);
	bool eof() const;

	XZipStream& operator> (char*);
	XZipStream& operator> (char&);
	XZipStream& operator> (unsigned char&);
	XZipStream& operator> (short&);
	XZipStream& operator> (unsigned short&);
	XZipStream& operator> (int&);
	XZipStream& operator> (unsigned int&);
	XZipStream& operator> (long&);
	XZipStream& operator> (unsigned long&);
	XZipStream& operator> (float&);
	XZipStream& operator> (double&);
	XZipStream& operator> (long double&);

	unsigned long read(void* buf, unsigned long len);
	unsigned long write(const void* buf, unsigned long len);

	template<class T> XZipStream& write(const T& v){ write(&v, sizeof(T)); return *this; }
	template<class T> XZipStream& read(T& v){ read(&v, sizeof(T)); return *this; }

	static XZipFileOpenMode openMode(){ return openMode_; }
	static setOpenMode(XZipFileOpenMode mode){ openMode_ = mode; }

	int compressionLevel() const { return compressionLevel_; }
	void setCompressionLevel(int level);

private:

	XZipArchive* archive_;

	bool handleErrors_;
	std::string fileName_;

	/// степень сжатия, 0-9
	int compressionLevel_;

	/// режим чтения напрямую из файла вне архива
	bool directReadMode_;
	unsigned long directReadPosition_;
	bool directReadEOF_;
	bool directReadFromArchive_;
	unsigned long directReadStartPosition_;
	unsigned long directReadSize_;
	unsigned long dosDate_;

	/// handle реального файла или указатель на внутренний zip handle из архива
	void* fileHandle_;

	bool openFromArchive(const char* file_name, int flags = XZS_IN);
	bool openDirect(const char* file_name, int flags = XZS_IN);

	void setZipHandle(void* file_handle, XZipArchive* archive){ fileHandle_ = file_handle; archive_ = archive; }

	static XZipFileOpenMode openMode_;

	friend class XZipArchiveManager;
	friend class ZLibArchive;
};

class XZipArchive
{
public:
	XZipArchive(bool handle_errors = false);
	virtual ~XZipArchive(){ };

	bool handleErrors() const { return handleErrors_; }
	void setHandleErrors(bool err = true) { handleErrors_ = err; }

	bool isInput() const { return mode_ == XZS_IN; }
	bool isOutput() const { return !isInput(); }

	int compressionLevel() const { return compressionLevel_; }
	void setCompressionLevel(int level);

	const char* zipName() const { return zipName_.c_str(); }

	virtual bool open(const char* name, XZipOpenMode mode = XZS_IN) = 0;
	virtual bool isOpen() const = 0;
	virtual bool close() = 0;

	virtual bool fileOpen(XZipStream& fh, const char* file_name, unsigned int flags = XZS_IN, int compression_level = 0) = 0;
	virtual bool fileClose(XZipStream& fh) = 0;

	virtual unsigned long fileRead(void* file_handle, void* buf, unsigned long len) = 0;
	virtual unsigned long fileWrite(void* file_handle, const void* buf, unsigned long len) = 0;
	virtual long fileSize(void* file_handle) const = 0;
	virtual long fileTell(void* file_handle) const = 0; 
	virtual bool fileEOF(void* file_handle) const = 0;
	virtual void fileTime(void* file_handle, unsigned& fdate, unsigned& ftime) const = 0;
	virtual long fileSeek(void* file_handle, long offset, int dir) = 0;

	/// конверсия имени файла, если требуется
	/// обычно для zip'а надо заменять '\' на '/'
	virtual const char* convertFileName(const char* file_name) const = 0;

protected:

	std::string zipName_;
	XZipOpenMode mode_;

private:

	/// степень сжатия, 0-9
	int compressionLevel_;
	bool handleErrors_;
};

class XZipArchiveManager
{
public:
	XZipArchiveManager(bool handle_errors = false);
	~XZipArchiveManager();

	/// открыть архив, в случае успеха он добавляется в список,
	/// можно открывать файлы из него через XZipStream::open()
	bool openArchive(const char* archive_name, XZipOpenMode mode = XZS_IN);
	/// закрыть архив, при этом он остаётся в списке
	bool closeArchive(const char* archive_name);

	/// добавить архив в список
	bool registerArchive(XZipArchive* archive);
	/// удалить архив из списка
	bool unregisterArchive(XZipArchive* archive);

	bool fileOpen(XZipStream& fh, const char* file_name, unsigned int flags = XZS_IN, int compression_level = 0);

	XZipArchive* createArchive() const;
	void releaseArchive(XZipArchive* archive) const;

	static XZipArchiveManager& instance();

private:

	typedef std::vector<XZipArchive*> Archives;
	/// список архивов для чтения
	Archives archives_;

	/// архив для записи, единовременно может быть только один
	XZipArchive* outputArchive_;

	bool handleErrors_;
};


class XZipArchiveMaker
{
public:
	typedef std::vector<std::string> FileNames;
	typedef void (*ProgressCallback)(int percents_done, const char* current_file_name);

	XZipArchiveMaker(const char* zip_name, const char* path, const char* files_mask, const char* exclude_files_mask, int compression = 0, bool purge_files = true, ProgressCallback callback = 0);
	~XZipArchiveMaker();

	bool buildArchive();

	ProgressCallback setProgressCallback(ProgressCallback handler);

private:

	int compressionLevel_;

	bool purgeFiles_;

	std::string zipName_;

	/// относительный путь к файлам
	std::string path_;

	std::string filesMask_;
	std::string excludeFilesMask_;

	/// список имён файлов
	FileNames files_;
	/// список имён файлов, которые не надо класть в архив
	FileNames excludeFiles_;

	ProgressCallback progressCallback_;

	bool makeFileLists();
};

#ifndef _XZIP_NO_AUTOMATIC_LIB

#define _LIB_NAME "XZip"
#include "AutomaticLink.h"

#endif // _XZIP_NO_AUTOMATIC_LIB

#endif // __XZIP_H__
