#ifndef __QD_MINIGAME_H__
#define __QD_MINIGAME_H__

#include "qd_named_object.h"
#include "qd_minigame_config.h"

class qdMiniGameInterface;

//! ����-����.
class qdMiniGame : public qdNamedObject
{
public:
	qdMiniGame();
	qdMiniGame(const qdMiniGame& mg);
	~qdMiniGame();

	qdMiniGame& operator = (const qdMiniGame& mg);

	int named_object_type() const { return QD_NAMED_OBJECT_MINIGAME; }

	//! ������������� ������, ���������� ��� ������ � ����������� �������� ����.
	bool init();

	const char* config_file_name() const { return config_file_name_.c_str(); }
	void set_config_file_name(const char* file_name){ config_file_name_ = file_name; }
	bool has_config_file() const { return !config_file_name_.empty(); }

	//! ����� ����, ���������� ��� ������ �� �����, ������� ��������� ����.
	bool start();
	bool is_started() const;
	//! ���������� ����� ����, �������� - �����, ������� ������ ������ � ���� (� ��������).
	bool quant(float dt);
	//! ��������� ����, ���������� ��� ����� �� �����, ������� ����������� �����.
	bool end();

	/// ����������, ���������� ��� ���������� ����� \a scene
	int save_game(char* buffer, int buffer_size, qdGameScene* scene);
	/// ��������, ���������� ��� �������� ����� \a scene
	int load_game(const char* buffer, int buffer_size, qdGameScene* scene);

	//! ���������� ��� ������������ ��� ���� dll.
	const char* dll_name() const { return dll_name_.c_str(); }
	//! ������������� ��� ������������ ��� ���� dll.
	void set_dll_name(const char* p){ dll_name_ = p; }
	bool has_dll() const { return !dll_name_.empty(); }

	const char* game_name() const { return game_name_.c_str(); }
	void set_game_name(const char* p){ game_name_ = p; }

	//! �������� ������ �� �������.
	bool load_script(const xml::tag* p);
	//! ������ ������ � ������.
	bool save_script(XStream& fh,int indent = 0) const;

	bool adjust_files_paths(const char* copy_dir, const char* pack_dir, bool can_overwrite = false);
	//! ��������� � ������������ ������ ����� ������, ����������� ��� ��������.
	bool get_files_list(qdFileNameList& files_to_copy,qdFileNameList& files_to_pack) const;

#ifdef _QUEST_EDITOR
	typedef std::list<qdMinigameConfigParameter> config_container_t;
#else
	typedef std::vector<qdMinigameConfigParameter> config_container_t;
#endif

	const config_container_t& config() const { return config_; }
	void set_config(const config_container_t& cfg){ config_ = cfg; }
	bool load_config();

	/// ���������� �������� ��������� � ������ cfg_param_name.
	/**
	���� �������� � ����� ������ �� ������, �� ���������� 0.
	*/
	const char* config_parameter_value(const char* cfg_param_name) const;

private:

	//! ��� ������������ ��� ���� dll.
	std::string dll_name_;
	//! .ini ���� � ����������� ����.
	std::string config_file_name_;
	//! ��� ����, �� �������� ��� ������ � dll.
	std::string game_name_;
	//! ����� ������������ dll.
	void* dll_handle_;
	//! ��������� � ���� �� dll.
	qdMiniGameInterface* interface_;

	//! ��������� ����.
	config_container_t config_;

	bool load_interface();
	bool release_interface();
};

#endif /* __QD_MINIGAME_H__ */
