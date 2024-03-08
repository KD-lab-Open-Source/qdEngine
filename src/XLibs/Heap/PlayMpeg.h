#pragma once


namespace mpp {

bool MpegInitLibrary(void* LPDIRECTSOUND_pDS);

//�������� �� ��������������� DirectSound
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

//�� ������ ������, ���� ��������� ������ ����� 0
//(�� ��������� ������ ��������)
void MpegSetPauseIfNullVolume(bool set=true);

MpegState MpegIsPlay();

bool MpegDebugRealPlay();//������������� �� ������������� ������


void MpegSetVolume(int volume);//0..255
int MpegGetVolume();


typedef const char* (*MpegFuncGetNextFile)();
void MpegSetGetNextFile(MpegFuncGetNextFile p);

//��������� �������, ������ ����������� ����������� ����� ��������� ����������.
//���������� true ���� ������� ��������� � �������.
//� ������� ����� ���� �� ����� 3x ������
bool MpegSteamIsEmpty();//��� ��������� ������

//��������� ��������� c �������� �������� �� volume  �� time ����������
bool MpegStreamVolumeTo(int volume,DWORD time);
bool MpegStreamSetVolume(int volume);
bool MpegStreamOpenToPlay(const char* fname,bool cycled);
bool MpegStreamStop();

//���������� ������ � 1/44100 ����� �������
int MpegGetLen(const char* fname);

}; // namespace mpp
