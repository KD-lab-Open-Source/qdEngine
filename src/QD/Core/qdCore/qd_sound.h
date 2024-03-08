#ifndef __QD_SOUND_H__
#define __QD_SOUND_H__

#include "xml_fwd.h"

#include "qd_named_object.h"
#include "qd_resource.h"
#include "qd_sound_handle.h"

#ifndef __QD_SYSLIB__
#include "wav_sound.h"
#endif

//! ����.
class qdSound : public qdNamedObject, public qdResource
{
public:
	qdSound();
	~qdSound();

	int named_object_type() const { return QD_NAMED_OBJECT_SOUND; }

	//! �������� ������ �� �������.
	void load_script(const xml::tag* p);
	//! ������ ������ � ������.
	bool save_script(XStream& fh,int indent = 0) const;

	bool load_resource();
	bool free_resource();
	//! ������������� ��� �����, � ������� �������� ������ �������.
	void set_resource_file(const char* file_name){ set_file_name(file_name); }
	//! ���������� ��� �����, � ������� �������� ��������.
	const char* resource_file() const { return file_name(); }
#ifdef __QD_DEBUG_ENABLE__
	unsigned resource_data_size() const { return sound_.data_length(); }
#endif

	//! ���������� ��� �����, � ������� �������� ����.
	const char* file_name() const { if(!file_name_.empty()) return file_name_.c_str(); return 0; }
	//! ������������� ��� �����, � ������� �������� ����.
	void set_file_name(const char* fname){ file_name_ = fname; }

	//! ��������� ������������ �����.
	/**
	handle ����� ��� ����������� ���������� ������ � ������������ ��� ���������,
	start_position - ��������� ������� ��� ������� �����, �������� - [0.0, 1.0]
	*/
	bool play(const qdSoundHandle* handle = NULL,bool loop = false,float start_position = 0.0f) const;
	//! ������������� ������������ �����.
	/**
	���� handle ������� - �� ���������� ������ ���������� �������� ���� � �������
	�� sound_, ���� ��������� - ������ ��������������� ����� ������.
	*/
	bool stop(const qdSoundHandle* handle = NULL) const;

	//! ���������� ������� ������� �����, �������� ������������ �������� - [0.0, 1.0].
	/**
	���� ���� �� ��������, ������� ������� ��������� �������. 
	*/
	float position(const qdSoundHandle* handle = NULL) const;

	//! ��������� ������� ����� � frequency_coeff ����� ���.
	bool set_frequency(const qdSoundHandle* handle,float frequency_coeff) const;

	//! ���������� ������������ ����� � ��������.
	float length() const { 
#ifndef __QD_SYSLIB__
		return sound_.length(); 
#endif
	}

	//! ���������� true, ���� ���� �� �������������.
	bool is_stopped(const qdSoundHandle* handle = NULL) const;

	int volume() const { return volume_; }
	void set_volume(int vol){ volume_ = vol; }

private:

	//! ��� �����.
	std::string file_name_;
	//! ��������� �����, [0, 255].
	int volume_;
	//! ������.
#ifndef __QD_SYSLIB__
	wavSound sound_;
#endif
};

typedef std::list<qdSound*> qdSoundList;

#endif /* __QD_SOUND_H__ */

