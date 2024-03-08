
#ifndef __QD_SOUND_INFO_H__
#define __QD_SOUND_INFO_H__

#include "qd_named_object.h"

class qdSound;

//! ���������� � �����.
class qdSoundInfo : public qdNamedObject
{
public:
	qdSoundInfo(){ }
	~qdSoundInfo(){ }

	//! �����.
	enum {
		//! ����������� ���� ��� ������������
		LOOP_SOUND_FLAG = 0x01
	};

	int named_object_type() const { return QD_NAMED_OBJECT_SOUND_INFO; }

	qdSound* sound() const;
};

#endif /* __QD_SOUND_INFO_H__ */
