#pragma once

class qdSound;

///КЉласс-утилита, проигрывающий звук
class SoundPlayer
{
	SoundPlayer(void);
	~SoundPlayer(void);
public:
	/// Проигрывает звук с указанной громкостью
	static void play(qdSound* sound, int vol);
};
