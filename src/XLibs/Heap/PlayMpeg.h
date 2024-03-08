#pragma once


namespace mpp {

bool MpegInitLibrary(void* LPDIRECTSOUND_pDS);

//Вызывать до деинициализации DirectSound
void MpegDeinitLibrary();

bool MpegOpenToPlay(const char* fname,bool cycled);
void MpegStop();
void MpegPause();
void MpegResume();

enum MpegState
{
	MPEG_STOP=0,
	MPEG_PLAY=1,
	MPEG_PAUSE=2,
};

//Не играть музыку, если громкость музыки равна 0
//(по умолчанию музыка играется)
void MpegSetPauseIfNullVolume(bool set=true);

MpegState MpegIsPlay();

bool MpegDebugRealPlay();//Действительно ли проигрывается музыка


void MpegSetVolume(int volume);//0..255
int MpegGetVolume();


typedef const char* (*MpegFuncGetNextFile)();
void MpegSetGetNextFile(MpegFuncGetNextFile p);

//Потоковые команды, каждая последующая выполняется после окончания предыдущей.
//возвращают true если команда добавлена в очередь.
//в очереди могут быть не более 3x команд
bool MpegSteamIsEmpty();//Нет потоковых команд

//Уменьшить громкость c текущего значения до volume  за time милисекунд
bool MpegStreamVolumeTo(int volume,DWORD time);
bool MpegStreamSetVolume(int volume);
bool MpegStreamOpenToPlay(const char* fname,bool cycled);
bool MpegStreamStop();

//Возвращает длинну в 1/44100 долях секунды
int MpegGetLen(const char* fname);

}; // namespace mpp
