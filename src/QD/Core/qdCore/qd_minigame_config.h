#ifndef __QD_MINIGAME_CONFIG_H__
#define __QD_MINIGAME_CONFIG_H__

/// ���������������� ������ ��� ��������.
/**
� .ini �����:
��� ������ - �������� ���������

�����
type = "string" / "file" / "object" - ��� ������ data_type_
count - ���������� ������, data_count_
value - ������ ������, data_string_
comment - �����������, comment_

���� ��� ������ �� ������, �� ��������� ������ "string".

������:

[ParameterExample]
type = "object"
count = 2
value = "Object0"
comment = "������ ���������"
*/
class qdMinigameConfigParameter
{
public:
	qdMinigameConfigParameter();
	~qdMinigameConfigParameter();

	bool operator == (const qdMinigameConfigParameter& prm) const { return (name_ == prm.name_); }
	bool operator == (const char* str) const { return !strcmp(str,name()); }

	//! ��� ������.
	enum data_type_t {
		/// ������ - ������
		PRM_DATA_STRING,
		/// ������ - ��� �����
		PRM_DATA_FILE,
		/// ������ - ��� ������� �� ������� �����
		PRM_DATA_OBJECT
	};

	const char* name() const { return name_.c_str(); }
	void set_name(const char* name){ name_ = name; }

	data_type_t data_type() const { return data_type_; }
	void set_data_type(data_type_t tp){ data_type_ = tp; }

	const char* data_string() const { return data_string_.c_str(); }
	void set_data_string(const char* str){ data_string_ = str; }

	const char* comment() const { return comment_.c_str(); }
	void set_comment(const char* str){ comment_ = str; }

	int data_count() const { return data_count_; }
	void set_data_count(int cnt){ data_count_ = cnt; }

	/// ��������� ���������� ������.
	/**
	���� ������ �� ���� �������, �� ���������� false � �������������
	is_data_valid_ � false.
	*/
	bool validate_data();
	bool is_data_valid() const { return is_data_valid_; }

	//! �������� ������ �� �������.
	bool load_script(const xml::tag* p);
	//! ������ ������ � ������.
	bool save_script(XStream& fh,int indent = 0) const;

	//! �������� ������ �� .ini �����.
	bool load_ini(const char* ini_file,const char* ini_section);

private:

	/// ��� ���������, ������ �� �������� ������������� �� ����.
	std::string name_;

	/// ��� ������.
	data_type_t data_type_;

	/// ���������� ������.
	/**
	������������ ��� �������� ������ (���������, ������� ����� �������� � data_string_)
	� �������� (� ����� �������� ��������������� ���������� ����� ������� �� ����� data_string_,
	� �� ������ ����������� ������������� ���������� �����).

	�� ��������� = 1.
	*/
	int data_count_;

	/// ������ ������.
	/**
	������ ������� �� ���� ������.
	��� �������� ������ - ����� � ��������� ���� ����� ������, ���
	��������� ����� - ������ ������.
	*/
	std::string data_string_;

	/// �����������.
	std::string comment_;

	/// false ���� ������ ������ �� ���� �������.
	bool is_data_valid_;
};

#endif /* __QD_MINIGAME_CONFIG_H__ */

