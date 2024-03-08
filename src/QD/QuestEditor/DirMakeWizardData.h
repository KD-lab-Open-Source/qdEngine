
#pragma once

/// Данные для визарда. 
/**
	проект этот визард не меняет. 
	Поэтому нет необходимости во флаге иземенния
 */
class DirMakeWizardData{
public:
	/// Папка, в котрой будет прописана структура папок
	CString m_strDir;
	/// Количетво сцен,  папки для которых будут созданы
	int scenes_count;
	DirMakeWizardData():scenes_count(0){}
};