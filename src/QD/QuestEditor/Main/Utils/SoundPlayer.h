#pragma once

class qdSound;

///ʊ����-�������, ������������� ����
class SoundPlayer
{
	SoundPlayer(void);
	~SoundPlayer(void);
public:
	/// ����������� ���� � ��������� ����������
	static void play(qdSound* sound, int vol);
};
