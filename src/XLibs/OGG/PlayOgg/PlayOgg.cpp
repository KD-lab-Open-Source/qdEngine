#include <windows.h>
#include <dsound.h>
#include <math.h>

#include <xutil.h>

#include "PlayOgg.h"
#include "vorbis/vorbisfile.h"

const int BLK_SIZE = 36 * 32;
const MAX_LENGTH = BLK_SIZE * 8;
const READ_BUFFER_SIZE = 8192;

class OggStreamImpl
{
public:
	OggStreamImpl(const char* file_name = 0, void* data_handle = 0);
	~OggStreamImpl();

	bool open(const char* file_name, void* data_handle = 0);
	bool isOpen() const { return isOpen_; }
	/// возвращает сколько байт было реально прочитано
	int read_frame(char* buffer, int buffer_size);
	void close();

	/// размер распакованного звука в байтах
	int uncompressedSize();

	/// длительность звука в секундах
	float length();
	/// текущая позиция в секундах
	float tell();
	/// количество каналов, моно/стерео - 1 или 2
	int channels() const;
	/// частота дискретизации (количество сэмплов в секунду)
	int samplingRate() const;

private:

    OggVorbis_File vorbisFile_;
	vorbis_info fileInfo_;
	void* fileHandle_;
	int bitstream_;

	bool isOpen_;
	bool needCloseHandle_;
};

class OggPlayerImpl
{
public:
	OggPlayerImpl();
	~OggPlayerImpl();

	bool play(const char* fname, bool cycled = true, void* data_handle = 0);
	void stop();
	void pause();
	void resume();

	const char* fileName();

	OggState state() const;

	void setVolume(int volume);
	int volume() const;
	bool fadeVolume(float time, float new_volume = 0.f);
	void setAutoFade(bool enable, float fade_time){ autoFadeEnabled_ = enable; autoFadeTime_ = fade_time; }
	void setPauseIfZeroVolume(bool set = true){ pauseIfZeroVolume_ = set; }

	bool openFile(const char* fname, void* data_handle)
	{
		if(!fname || !fname[0])
			return false;

		return stream_.open(fname, data_handle);
	}

	void closeFile(){ stream_.close(); }

	// len - величнна буффера buf в short
	bool nextFrame(short*& buf, int& len)
	{
		int ret = stream_.read_frame(buffer_, READ_BUFFER_SIZE);
		len = ret;
		buf = (short*)buffer_;
		return ret > 0;
	}

	float length() { return stream_.length(); }
	float tell(){ return stream_.tell(); }

	void updateVolume();

	static void setCallbacks(OggCallbacks* callbacks);
	static OggCallbacks* callbacks();

	static bool initLibrary(void* dsound);
	static void finitLibrary();

private:

	enum { soundBufferSize_ = 256*1024 };

	char buffer_[READ_BUFFER_SIZE];

	OggStream stream_;

	OggPlayerImpl* prevPlayer_;
	OggPlayerImpl* nextPlayer_;

	WAVEFORMATEX wave_format;
	LPDIRECTSOUNDBUFFER soundBuffer_;
	DWORD soundBufferOffset_;

	int volume_;
	/// текущее значение громкости, может не совпадать с volume_ из-за фэйда
	int currentVolume_;

	bool pauseIfZeroVolume_;

	struct OggFileInfo
	{
		char fileName_[MAX_PATH];
		void* dataHandle_;
		bool needCycle_;

		OggFileInfo(const char* file_name = "", void* data_handle = 0, bool cycle = false){	set(file_name, data_handle, cycle); }
		bool operator == (const OggFileInfo& inf) const { return !stricmp(fileName_, inf.fileName_); }

		void set(const char* file_name, void* data_handle, bool cycle){
			strcpy(fileName_, file_name);
			dataHandle_ = data_handle;
			needCycle_ = cycle;
		}
		void clear(){ set("", 0, false); }

		bool isEmpty() const { return !strlen(fileName_); }
	};

	class EWait
	{
	public:
		EWait()
		{
			if(waitEventHandle_ == INVALID_HANDLE_VALUE) return;
			WaitForSingleObject(waitEventHandle_,INFINITE);
		}
		~EWait()
		{
			if(waitEventHandle_ == INVALID_HANDLE_VALUE)return;
			SetEvent(waitEventHandle_);
		}
	};

	OggFileInfo currentFileInfo_;
	OggFileInfo nextFileInfo_;

	bool clearEndBuffer_;
	OggState state_;

	short lastSignalBuffer_[BLK_SIZE * 2];
	int lastSignalBufferOffset_;

	void hammingCorrect(short* pData, int len);

	bool fadeEnabled_;
	int fadeTimeStart_;
	int fadeTime_;

	bool autoFadeEnabled_;
	float autoFadeTime_;

	float fadeVolume_;

	static OggCallbacks* callbacks_;

	static int hammingWindow_[BLK_SIZE]; // Окно Хэмминга
	enum { h_shift = 14 };

	static LPDIRECTSOUND dSound_;
	static OggPlayerImpl* firstPlayer_;

	static HANDLE waitEventHandle_;
	static HANDLE threadHandle_;
	static int threadStopFlag_;

	bool initSoundBuffer();
	void stopPlayback();
	bool setFade(float time, float new_volume = 0.f);

	void callbackProc();

	//В какое место буфера надо будет писать, 
	//если в него записали offset байт
	void setSoundBufferOffset(DWORD offset);

	void setCurrentVolume(int volume)
	{
		currentVolume_ = volume;
		updateVolume();
	}

	void clearFade();
	void fadeQuant();

	static DWORD WINAPI threadProc(LPVOID lpParameter);
	static void createWindowTable();
};

LPDIRECTSOUND OggPlayerImpl::dSound_ = 0;
OggPlayerImpl* OggPlayerImpl::firstPlayer_ = 0;
int OggPlayerImpl::hammingWindow_[BLK_SIZE];

HANDLE OggPlayerImpl::waitEventHandle_ = INVALID_HANDLE_VALUE;
HANDLE OggPlayerImpl::threadHandle_ = INVALID_HANDLE_VALUE;
int OggPlayerImpl::threadStopFlag_ = 0;

#define DB_MIN		-10000
#define DB_MAX		0
#define DB_SIZE 	10000

static int FromDirectVolume(long vol)
{
	double v=exp((exp(((vol-DB_MIN)/(double)DB_SIZE)*log(10.0f))-1.0)*log(2.0)*8/9.0)-1;
	return (int)(v+0.5);
}

static long ToDirectVolume(int vol)
{

	int v = DB_MIN + (int)(0.5+log10(9.0*log(double(vol + 1))/(log(2.0)*8) + 1.0)*DB_SIZE);
	return v;
}

DWORD WINAPI OggPlayerImpl::threadProc(LPVOID lpParameter)
{
	SetThreadPriority(threadHandle_,THREAD_PRIORITY_TIME_CRITICAL);

	while(!threadStopFlag_){
		{
			EWait w;

			for(OggPlayerImpl* cur = firstPlayer_; cur; cur = cur -> nextPlayer_)
				cur->callbackProc();
		}

		Sleep(10);
	}

	threadStopFlag_=2;
	return 0;
}

bool OggPlayerImpl::initLibrary(void* dsound)
{
	dSound_ = 0;
	createWindowTable();
	threadStopFlag_ = 0;
	dSound_ = (LPDIRECTSOUND)dsound;
	return true;
}

void OggPlayerImpl::finitLibrary()
{
	{
		EWait w;
		for(OggPlayerImpl* cur=firstPlayer_; cur; cur = cur->nextPlayer_){
			cur->stopPlayback();
			if(cur->soundBuffer_) cur->soundBuffer_->Release();
			cur->soundBuffer_ = 0;
		}
	}


	if(threadHandle_ != INVALID_HANDLE_VALUE)
	{
		threadStopFlag_ = 1;
		while(threadStopFlag_ == 1)
			Sleep(10);
	}

	if(waitEventHandle_ != INVALID_HANDLE_VALUE)
		CloseHandle(waitEventHandle_);

	waitEventHandle_ = INVALID_HANDLE_VALUE;
	threadHandle_ = INVALID_HANDLE_VALUE;

	dSound_ = 0;
}

// ------------------------- OggStreamImpl

OggStreamImpl::OggStreamImpl(const char* file_name, void* data_handle) : fileHandle_(0),
	bitstream_(0),
	isOpen_(false),
	needCloseHandle_(false)
{
	if(file_name)
		open(file_name, data_handle);
}

OggStreamImpl::~OggStreamImpl()
{
	close();
}

bool OggStreamImpl::open(const char* file_name, void* data_handle)
{
	if(data_handle){
		needCloseHandle_ = false;
		fileHandle_ = data_handle;

		OggCallbacks* mcb = OggPlayerImpl::callbacks();
		if(!mcb) return false;

		if(!(*mcb->open_func)(fileHandle_, file_name)){
			fileHandle_ = 0;
			return false;
		}

		ov_callbacks cb;
		cb.read_func = mcb->read_func;
		cb.seek_func = mcb->seek_func;
		cb.close_func = mcb->close_func;
		cb.tell_func = mcb->tell_func;

		if(ov_open_callbacks(fileHandle_, &vorbisFile_, 0, 0, cb) < 0){
			fileHandle_ = 0;
			return false;
		}
	}
	else {
		needCloseHandle_ = true;

		fileHandle_ = fopen(file_name,"rb");
		if(!fileHandle_)
			return false;
		if(ov_open((FILE*)fileHandle_, &vorbisFile_, 0, 0) < 0){
			fclose((FILE*)fileHandle_);
			fileHandle_ = 0;
			return false;
		}
	}

	fileInfo_ = *ov_info(&vorbisFile_, -1);
	isOpen_ = true;

	return true;
}

int OggStreamImpl::read_frame(char* buffer, int buffer_size)
{
	return ov_read(&vorbisFile_, buffer, buffer_size, 0, 2, 1, &bitstream_);
}

void OggStreamImpl::close()
{
	if(isOpen_){
		ov_clear(&vorbisFile_);
		if(!needCloseHandle_){
			if(OggCallbacks* mcb = OggPlayerImpl::callbacks())
				(*mcb->close_func)(fileHandle_);
		}
		else
			fclose((FILE*)fileHandle_);

		fileHandle_ = 0;
		isOpen_ = false;
		bitstream_ = 0;
	}
}

int OggStreamImpl::uncompressedSize()
{
	return (int)ov_pcm_total(&vorbisFile_, bitstream_) * 2 * channels();
}

float OggStreamImpl::length()
{
	return (float)ov_time_total(&vorbisFile_, bitstream_);
}

float OggStreamImpl::tell()
{
	return (float)ov_time_tell(&vorbisFile_);
}

int OggStreamImpl::channels() const
{
	return fileInfo_.channels;
}

int OggStreamImpl::samplingRate() const
{
	return fileInfo_.rate;
}

// ------------------------- OggStream

OggStream::OggStream(const char* file_name, void* data_handle)
{
  stream_ = new OggStreamImpl(file_name, data_handle);
}

OggStream::~OggStream()
{
	delete stream_;
}

bool OggStream::open(const char* file_name, void* data_handle)
{
	return stream_->open(file_name, data_handle);
}

bool OggStream::isOpen() const
{
	return stream_->isOpen();
}

int OggStream::read_frame(char* buffer, int size)
{
	return stream_->read_frame(buffer, size);
}

void OggStream::close()
{
	stream_->close();
}

int OggStream::uncompressedSize() const
{
	return stream_->uncompressedSize();
}

float OggStream::length() const
{
	return stream_->length();
}

float OggStream::tell() const
{
	return stream_->tell();
}

int OggStream::channels() const
{
	return stream_->channels();
}

int OggStream::samplingRate() const
{
	return stream_->samplingRate();
}

// ------------------------- OggPlayerImpl

OggCallbacks* OggPlayerImpl::callbacks_ = 0;

OggPlayerImpl::OggPlayerImpl()
{
	EWait w;
	prevPlayer_ = 0;
	nextPlayer_ = firstPlayer_;

	autoFadeEnabled_ = false;
	autoFadeTime_ = 1.f;

	soundBuffer_ = 0;
	soundBufferOffset_ = 0;

	firstPlayer_ = this;
	if(nextPlayer_)
		nextPlayer_->prevPlayer_=this;

	volume_ = 255;
	currentVolume_ = 255;
	pauseIfZeroVolume_ = false;

	clearEndBuffer_ = false;
	state_ = OGG_STOPPED;

	soundBufferOffset_ = 0;

	lastSignalBufferOffset_ = 0;
	fadeEnabled_=false;
	fadeTimeStart_ = fadeTime_ = 0;
	fadeVolume_ = 1.f;

	memset(&wave_format,0,sizeof(wave_format));
}

OggPlayerImpl::~OggPlayerImpl()
{
	EWait w;

	if(prevPlayer_)
		prevPlayer_->nextPlayer_=nextPlayer_;
	else    
		firstPlayer_=nextPlayer_;
	if(nextPlayer_)
		nextPlayer_->prevPlayer_=prevPlayer_;

	if(soundBuffer_){
		soundBuffer_->Release();
		soundBuffer_ = 0;
	}	

	closeFile();
}

void OggPlayerImpl::updateVolume()
{
	if(soundBuffer_){
		long ddvol=ToDirectVolume(currentVolume_);
		HRESULT hr = soundBuffer_->SetVolume(ddvol);

		if(pauseIfZeroVolume_ && state_ == OGG_PLAYING){
			DWORD status;
			if(soundBuffer_->GetStatus(&status) == DS_OK){
				bool b_play = (status & DSBSTATUS_PLAYING) != 0;

				if(!volume_){
					if(b_play){
						soundBuffer_->Stop();
						state_ = OGG_STOPPED;
					}
				}
				else {
					if(!b_play){
						soundBuffer_->Play(0,0,DSBPLAY_LOOPING);
						state_ = OGG_PLAYING;
					}
				}
			}
		}
	}
}

bool OggPlayerImpl::initSoundBuffer()
{
	if(soundBuffer_ && wave_format.nChannels == stream_.channels())
		return true;

	if(!dSound_) 
		return false;
	
	WAVEFORMATEX& wfx = wave_format;

	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = stream_.channels();
	wfx.nSamplesPerSec = stream_.samplingRate();
    wfx.nAvgBytesPerSec = stream_.samplingRate()*2*wfx.nChannels;
    wfx.nBlockAlign = 2*wfx.nChannels; 
    wfx.wBitsPerSample = 16;
    wfx.cbSize = 0;

    DSBUFFERDESC dsbd;
    ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
    dsbd.dwSize = sizeof(DSBUFFERDESC);
    dsbd.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_GETCURRENTPOSITION2;
    dsbd.dwBufferBytes = soundBufferSize_;
    dsbd.lpwfxFormat = &wfx;

	HRESULT hr;
    if(!dSound_ || FAILED(hr = dSound_->CreateSoundBuffer(&dsbd, &soundBuffer_, 0))){
		soundBuffer_ = 0;
        return false;
	}

	updateVolume();

	return true;
}

void OggPlayerImpl::stopPlayback()
{
	state_ = OGG_STOPPED;

	if(!dSound_) return;

	if(soundBuffer_){
		soundBuffer_->Stop();

		BYTE *AudioPtr1,*AudioPtr2;
		DWORD AudioBytes1,AudioBytes2;

		if(FAILED(soundBuffer_->Lock(0,	soundBufferSize_, (LPVOID*)&AudioPtr1, &AudioBytes1,	(LPVOID*)&AudioPtr2, &AudioBytes2, 0))){
//			OutputDebugString("Failed\n");
			return;
		}

		if(AudioBytes1+AudioBytes2 == (DWORD)soundBufferSize_){
			if(AudioPtr1)memset(AudioPtr1, 0, AudioBytes1);
			if(AudioPtr2)memset(AudioPtr2, 0, AudioBytes2);
		}

		soundBuffer_->Unlock((LPVOID)AudioPtr1,AudioBytes1,(LPVOID)AudioPtr2,AudioBytes2);
		
		soundBufferOffset_ = 0;
		soundBuffer_->SetCurrentPosition(0);
	}
}

void OggPlayerImpl::setSoundBufferOffset(DWORD offset)
{
	soundBufferOffset_ = soundBufferOffset_ + offset;
	if(soundBufferOffset_ >= soundBufferSize_)
		soundBufferOffset_ = soundBufferOffset_ % soundBufferSize_; 
}

void OggPlayerImpl::callbackProc()
{
	DWORD dwPlayCursor, dwWriteCursor;
	
	int num_get_bytes = 0;

	if(state_ == OGG_STOPPED && !nextFileInfo_.isEmpty()){ 
		currentFileInfo_ = nextFileInfo_;
		nextFileInfo_.clear();

		bool is_initialize = openFile(currentFileInfo_.fileName_, currentFileInfo_.dataHandle_);

		if(!is_initialize)
			return;

		if(!initSoundBuffer()) return;

		soundBuffer_->Stop();
		soundBuffer_->SetCurrentPosition(0);

		BYTE *AudioPtr1, *AudioPtr2;
		DWORD AudioBytes1, AudioBytes2;

		if(FAILED(soundBuffer_->Lock(0, soundBufferSize_, (LPVOID*)&AudioPtr1, &AudioBytes1, (LPVOID*)&AudioPtr2, &AudioBytes2, 0))){
//			OutputDebugString("Failed\n");
			return;
		}

		if(AudioBytes1+AudioBytes2 == (DWORD)soundBufferSize_){
			if(AudioPtr1)memset(AudioPtr1,0,AudioBytes1);
			if(AudioPtr2)memset(AudioPtr2,0,AudioBytes2);
		}

		soundBuffer_->Unlock((LPVOID)AudioPtr1,AudioBytes1,(LPVOID)AudioPtr2,AudioBytes2);

		soundBufferOffset_=0;
		short* pData;
		int len;
		
		bool getdata = false;
		for(DWORD i=0; i < soundBufferSize_/READ_BUFFER_SIZE + 1; i++){
			if(nextFrame(pData,len)){
				
				BYTE *AudioPtr1,*AudioPtr2;
				DWORD AudioBytes1,AudioBytes2;

				if(FAILED(soundBuffer_->Lock(soundBufferOffset_, len, (LPVOID*)&AudioPtr1, &AudioBytes1, (LPVOID*)&AudioPtr2, &AudioBytes2, 0)))
					return;

				if(AudioBytes1+AudioBytes2==(DWORD)len){
					memcpy(AudioPtr1,pData,AudioBytes1);
					memcpy(AudioPtr2,pData+AudioBytes1,AudioBytes2);
				}


				soundBuffer_->Unlock((LPVOID)AudioPtr1,	AudioBytes1, (LPVOID)AudioPtr2, AudioBytes2);
				// указывает на позицию с которой начинать записывать след. данные (зацикленный буфер)
				setSoundBufferOffset(len);
				num_get_bytes += len;
				getdata = true;
			}
			else {  
				if(!currentFileInfo_.needCycle_ && getdata){
					BYTE *AudioPtr1,*AudioPtr2;
					DWORD AudioBytes1,AudioBytes2;

					if(FAILED(soundBuffer_->Lock(soundBufferOffset_, MAX_LENGTH*wave_format.nChannels,	(LPVOID*)&AudioPtr1, &AudioBytes1, (LPVOID*)&AudioPtr2, &AudioBytes2, 0))){
			//			OutputDebugString("Failed\n");
						return;
					}

					if(AudioBytes1+AudioBytes2 == (DWORD)MAX_LENGTH*wave_format.nChannels){
						if(AudioPtr1)memset(AudioPtr1,0,AudioBytes1);
						if(AudioPtr2)memset(AudioPtr2,0,AudioBytes2);
					}

					soundBuffer_->Unlock((LPVOID)AudioPtr1,AudioBytes1,(LPVOID)AudioPtr2,AudioBytes2);
				}
				break;
			}
		}
		if(getdata){
//			clearFade(); 
			if(soundBuffer_){
				int vol = currentVolume_;
				setCurrentVolume(0);
				soundBuffer_->Stop();
				soundBuffer_->SetCurrentPosition(0);
				if(pauseIfZeroVolume_ && !vol){
					soundBuffer_->Stop();
					state_ = OGG_STOPPED;
				}
				else {
					soundBuffer_->Play(0,0,DSBPLAY_LOOPING);
					if(!FAILED(soundBuffer_->GetCurrentPosition(&dwPlayCursor,&dwWriteCursor)))
						soundBuffer_->SetCurrentPosition(dwPlayCursor);

					setCurrentVolume(vol);
					state_ = OGG_PLAYING;
				}
			}
		}

		if(is_initialize)
			state_ = OGG_PLAYING;

		if(autoFadeEnabled_){
			if(!fadeEnabled_ || fadeVolume_ < 1.f)
				setFade(autoFadeTime_, 1.f);
		}

		return;
	}

	bool loop_flag = true;
	while(loop_flag){
		if(!soundBuffer_)
			return;

		fadeQuant();

		if(FAILED(soundBuffer_->GetCurrentPosition(&dwPlayCursor, &dwWriteCursor)))
			return;

		if(dwPlayCursor<=soundBufferOffset_)
			dwPlayCursor+=soundBufferSize_;

		if(soundBufferOffset_ + MAX_LENGTH * wave_format.nChannels >= dwPlayCursor || num_get_bytes > 2 * MAX_LENGTH * wave_format.nChannels){
//			OutputDebugString(temp_buf);
			return;
		}

		short* pData;
		int len;

		if(nextFrame(pData,len)){
			num_get_bytes += len;

			if(!nextFileInfo_.isEmpty()){
				if(autoFadeEnabled_){
					if(!fadeEnabled_ || fadeVolume_ > 0.01f)
						setFade(autoFadeTime_, 0.f);

					if(!currentVolume_){
						closeFile();
						stopPlayback();
						return;
					}
				}
				else {
					closeFile();
					stopPlayback();
					return;
				}
/*
					currentFileInfo_ = nextFileInfo_;
					nextFileInfo_.clear();

					bool is_initialize = openFile(currentFileInfo_.fileName_, currentFileInfo_.dataHandle_);

					short* pData_;
					int len_;
					if(nextFrame(pData_,len_)){
						lastSignalBufferOffset_ = 0;
						for(int i=0;i<len_;i++)
							lastSignalBuffer_[i] = pData_[i];

						hammingCorrect(pData,len/wave_format.nChannels);
					}

					if(is_initialize)
						state_ = OGG_PLAYING;

					if(autoFadeEnabled_){
						if(!fadeEnabled_ || fadeVolume_ < 1.f)
							setFade(autoFadeTime_, 1.f);
					}
				}
*/
			}

			DWORD cur_write_byte = len;

			BYTE *AudioPtr1,*AudioPtr2;
			DWORD AudioBytes1,AudioBytes2;

			if(FAILED(soundBuffer_->Lock(soundBufferOffset_, cur_write_byte, (LPVOID*)&AudioPtr1, &AudioBytes1, (LPVOID*)&AudioPtr2, &AudioBytes2, 0))){
	//			OutputDebugString("Failed\n");
				return;
			}

			if(AudioBytes1 + AudioBytes2 == cur_write_byte){
				if(AudioPtr1)memcpy(AudioPtr1, (BYTE*)pData, AudioBytes1);
				if(AudioPtr2)memcpy(AudioPtr2, AudioBytes1 + (BYTE*)pData, AudioBytes2);
			}

			soundBuffer_->Unlock((LPVOID)AudioPtr1,AudioBytes1,(LPVOID)AudioPtr2,AudioBytes2);

			setSoundBufferOffset(cur_write_byte);

			clearEndBuffer_=true;
		}
		else {
			loop_flag = false;
			if(!currentFileInfo_.needCycle_ && state_ == OGG_PLAYING && clearEndBuffer_){
				clearEndBuffer_ = false; // only for debug

				BYTE *AudioPtr1,*AudioPtr2;
				DWORD AudioBytes1,AudioBytes2;

				if(FAILED(soundBuffer_->Lock(soundBufferOffset_, MAX_LENGTH*wave_format.nChannels, (LPVOID*)&AudioPtr1, &AudioBytes1, (LPVOID*)&AudioPtr2, &AudioBytes2, 0))){
		//			OutputDebugString("Failed\n");
					return;
				}

				if(AudioBytes1+AudioBytes2 == (DWORD)MAX_LENGTH*wave_format.nChannels){
					if(AudioPtr1)memset(AudioPtr1,0,AudioBytes1);
					if(AudioPtr2)memset(AudioPtr2,0,AudioBytes2);
				}

				soundBuffer_->Unlock((LPVOID)AudioPtr1,AudioBytes1,(LPVOID)AudioPtr2,AudioBytes2);
			}
		}
	}

	closeFile();

	if(currentFileInfo_.needCycle_){
		if(openFile(currentFileInfo_.fileName_, currentFileInfo_.dataHandle_))
			state_ = OGG_PLAYING;
		else
			stopPlayback();
	}
	else {
		if(FAILED(soundBuffer_->GetCurrentPosition(&dwPlayCursor,&dwWriteCursor)))
			return;

		int mi = dwPlayCursor;
		int ma = mi + MAX_LENGTH * wave_format.nChannels;
		int cmp = soundBufferOffset_;
		if(mi > cmp)
			cmp += soundBufferSize_;

		if(mi <= cmp && ma >= cmp)
			stopPlayback();
	}
}

bool OggPlayerImpl::play(const char* fname, bool cycled, void* data_handle)
{
	EWait w;
	nextFileInfo_.set(fname, data_handle, cycled);

	if(nextFileInfo_.isEmpty())
		return false;

	if(nextFileInfo_ == currentFileInfo_ && state_ == OGG_PLAYING){
		nextFileInfo_.clear();
		return false;
	}

	if(threadHandle_ == INVALID_HANDLE_VALUE){
		threadStopFlag_ = 0;
		DWORD ThreadId;
		threadHandle_ = CreateThread(0, 0, threadProc, 0, 0, &ThreadId);
		waitEventHandle_ = CreateEvent(0,FALSE,TRUE,0);
	}

	if(autoFadeEnabled_ && state_ == OGG_PLAYING){
		if(!fadeEnabled_ || fadeVolume_ > 0.01f)
			setFade(autoFadeTime_, 0.f);
	}

	return true;
}

void OggPlayerImpl::stop()
{
	EWait w;
	clearFade();
	stopPlayback();
}

void OggPlayerImpl::setVolume(int volume)
{
	EWait w;

	volume_ = volume;
	if(!fadeEnabled_)
		setCurrentVolume(volume_);
}

int OggPlayerImpl::volume() const
{
	EWait w;
	return volume_; 
}

void OggPlayerImpl::pause()
{
	EWait w;
	if(!soundBuffer_) return;
	soundBuffer_->Stop();
	state_ = OGG_PAUSED;
}

void OggPlayerImpl::resume()
{
	EWait w;
	if(!soundBuffer_)
		return;
	if(state_ == OGG_STOPPED)
		return;

	if(pauseIfZeroVolume_ && !volume_){
		soundBuffer_->Stop();
		state_ = OGG_STOPPED;
	}
	else
		if(SUCCEEDED(soundBuffer_->Play(0,0,DSBPLAY_LOOPING)))
			state_ = OGG_PLAYING;
}

OggState OggPlayerImpl::state() const
{
	EWait w;
	if(!soundBuffer_)return OGG_STOPPED;

	if(state_==OGG_STOPPED && !nextFileInfo_.isEmpty())
		return OGG_PLAYING;
/*
	DWORD status;
	if(FAILED(soundBuffer_->GetStatus(&status)))
		return OGG_STOPPED;
*/
	return state_;//(status&DSBSTATUS_PLAYING)?true:false;
}

void OggPlayerImpl::createWindowTable()
{
	const int mul = 1 << h_shift;
	const double PI = 3.14159265358979323846;
	for(int i = 0; i < BLK_SIZE; i++){
		double t = double(i) / (2 * BLK_SIZE);
		double w = 0.5 + 0.5 * cos(2 * PI * t);;
		hammingWindow_[i] = (int)(w * mul);
	}
}

void OggPlayerImpl::hammingCorrect(short* data, int data_length)
{
	data_length = data_length >> 1;

	int max_offset = lastSignalBufferOffset_ + data_length;
	if(max_offset > BLK_SIZE) max_offset = BLK_SIZE;

	short* out = data;
	short* in = lastSignalBuffer_ + lastSignalBufferOffset_ * 2;

	for(int i = lastSignalBufferOffset_; i < max_offset; i++){
		for(int j = 0; j < 2; j++){
			int o = *out*hammingWindow_[BLK_SIZE-i-1] + *in *hammingWindow_[i];

			o = o >> h_shift;
			if(o > 32767) o = 32767;
			if(o < -32768) o = 32768;

			*out = o;
			in++;
			out++;
		}
	}
	
	lastSignalBufferOffset_ = max_offset;
	if(lastSignalBufferOffset_ >= BLK_SIZE)
		lastSignalBufferOffset_ = 0;
}


bool OggPlayerImpl::fadeVolume(float time, float new_volume)
{
	EWait w;
	return setFade(time, new_volume);
}

bool OggPlayerImpl::setFade(float time, float new_volume)
{
	if(time <= 0.05f){
		clearFade();
		return false;
	}

	fadeEnabled_ = true;
	fadeTimeStart_ = xclock();
	fadeTime_ = round(time * 1000.0f);
	fadeVolume_ = new_volume;

	return true;
}

void OggPlayerImpl::fadeQuant()
{
	if(!fadeEnabled_)
		return;

	int vol = 0;
	float fade_phase = float(xclock() - fadeTimeStart_)/float(fadeTime_);
	if(fade_phase < 0)
		vol = volume_;
	else if(fade_phase < 1){
		vol = round(float(volume_) - float(volume_) * (1.f - fadeVolume_) * fade_phase);
	}
	else {
		vol = round(float(volume_) * fadeVolume_);

		if(vol == volume_)
			fadeEnabled_ = false;
	}

	if(vol != currentVolume_)
		setCurrentVolume(vol);
}

void OggPlayerImpl::clearFade()
{
	fadeEnabled_ = false;
	if(currentVolume_ != volume_)
		setCurrentVolume(volume_);
}

const char* OggPlayerImpl::fileName()
{
	if(currentFileInfo_.isEmpty())
		return 0;

	return currentFileInfo_.fileName_;
}

void OggPlayerImpl::setCallbacks(OggCallbacks* callbacks_ptr)
{
	callbacks_ = callbacks_ptr;
}

OggCallbacks* OggPlayerImpl::callbacks()
{
	return callbacks_;
}

// ------------------------------------ OggPlayer

OggPlayer::OggPlayer()
{
	player_ = new OggPlayerImpl;
}

OggPlayer::~OggPlayer()
{
	delete player_;
}

bool OggPlayer::play(const char* fname, bool cycled, void* data_handle)
{
	return player_->play(fname, cycled, data_handle);
}

void OggPlayer::stop()
{
	player_->stop();
}

void OggPlayer::pause()
{
	player_->pause();
}

void OggPlayer::resume()
{
	player_->resume();
}

const char* OggPlayer::fileName() const
{
	return player_->fileName();
}

void OggPlayer::setPauseIfZeroVolume(bool set)
{
	player_->setPauseIfZeroVolume(set);
}

OggState OggPlayer::state() const
{
	return player_->state();
}

void OggPlayer::setVolume(int volume)
{
	player_->setVolume(volume);
}

int OggPlayer::volume() const
{
	return player_->volume();
}

bool OggPlayer::fadeVolume(float time, float new_volume)
{
	return player_->fadeVolume(time, new_volume);
}

void OggPlayer::setAutoFade(bool enable, float fade_time)
{
	player_->setAutoFade(enable, fade_time);
}

void OggPlayer::setCallbacks(OggCallbacks* callbacks)
{
	OggPlayerImpl::setCallbacks(callbacks);
}

OggCallbacks* OggPlayer::callbacks()
{
	return OggPlayerImpl::callbacks();
}

double OggPlayer::getLength(const char* fname)
{
	OggVorbis_File vf;
	FILE* in=fopen(fname,"rb");
	if(!in)
		return -1;

    if(ov_open(in, &vf, 0, 0) < 0) {
        fclose(in);
        return -1;
    }

	double time = ov_time_total(&vf,-1);
    ov_clear(&vf);
    fclose(in);

	return time;
}

bool OggPlayer::initLibrary(void* dsound)
{
	return OggPlayerImpl::initLibrary(dsound);
}

void OggPlayer::finitLibrary()
{
	OggPlayerImpl::finitLibrary();
}

float OggPlayer::length()
{
	return player_->length();
}

float OggPlayer::tell()
{
	return player_->tell();
}

