#include "stdafx.h"
#include ".\soundplayer.h"

#include "qd_sound.h"

SoundPlayer::SoundPlayer(void)
{
}

SoundPlayer::~SoundPlayer(void)
{
}

/**
	Если ресурсы звука еще не загружены - загружаются. Предыдущая громкость звука 
	восстанавливается
 */
void SoundPlayer::play(qdSound* sound, int vol)
{
	if (!sound)
		return;

	if (!sound->is_resource_loaded()) {
		CWaitCursor __wait__;
		sound->load_resource();
	}
	int volbak = sound->volume();
	sound->set_volume(vol);
	sound->play();
	sound->set_volume(volbak);
}
