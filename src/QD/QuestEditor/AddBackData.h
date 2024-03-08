#pragma once

#include "mwdatabase.h"

/**
	������ ��� ������� ������������� ��������� ������������� ������
 */
class AddBackData : public MWDataBase
{
	/// ���
	CString name_;
	/// ��� ����� ��������
	CString anim_file_;
	/// �����, � ������� ������ ����� ��������
	class qdInterfaceScreen* pscreen_;
public:
	explicit AddBackData(qdInterfaceScreen* pscreen){
		pscreen_ = pscreen;
	}
	qdInterfaceScreen* screen() const{
		return pscreen_;
	}
	void set_name(const CString& name){
		name_ = name;
		change();
	}
	const CString& name() const {
		return name_;
	}

	void set_anim_file(const CString& file_name){
		anim_file_ = file_name;
		change();
	}
	const CString& anim_file() const{
		return anim_file_;
	}
};