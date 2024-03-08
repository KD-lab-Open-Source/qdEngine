#pragma once

#include "mwdatabase.h"

/**
	Данные для визарда добавляеющего бекграунд интерфейсному экрану
 */
class AddBackData : public MWDataBase
{
	/// Имя
	CString name_;
	/// Имя файла анимации
	CString anim_file_;
	/// Экран, в котроый объект будет добавлен
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