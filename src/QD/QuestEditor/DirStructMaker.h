#pragma once

#include "IShowProgress.h"

/**
	Создает структуру каталогов
 */
class DirStructMaker
{
public:
	DirStructMaker(IShowProgress* psp);
	~DirStructMaker(void);

	/**
		Входная функция.
		@param lpszDir - папка, в которой буду созданы каталоги
		@param scens_count - количество сценных папок
	 */
	bool make(LPCTSTR lpszDir, int scenes_count);
private:
	/// Создает папку
	/**
		@return true - если папка создана или уже существавала
	 */
	bool make_dir(LPCTSTR lpszDirName);
	/// Создает подкаталого lpszSubDir в lpszDirBase
	bool make_sub_dir(LPCTSTR lpszDirBase, LPCTSTR lpszSubDir);

	/// Создает структру каталогов для каталога сцены, содержащего исходные матерьялы
	/**
		@param strSceneName - каталог сцены
		@param lpszMovObjDir - имя каталога персонажей
		@param lpszDynObjDir - имя каталога анимированных объектов
		@param lpszStaticObjDir - имя каталога статических объектов
		@param lpszWavsDir	- имя каталога со звуками
		@param lpszDlgsDir	- имя каталога с диалогами
	 */
	bool make_scene_dir_struct(const CString& strSceneName, 
								LPCTSTR lpszMovObjDir,
								LPCTSTR lpszDynObjDir,
								LPCTSTR lpszStaticObjDir,
								LPCTSTR lpszWavsDir,
								LPCTSTR lpszDlgsDir
								);
	/// @brief Создает структру каталогов для каталога сцены, 
	/// содержащего собранную анимацию
	/**
		@param strSceneName - каталог сцены
		@param lpszAnimationDir - имя каталога с анимацией
		@param lpszStaticObjDir - имя каталога статических объектов
		@param lpszDlgsDir	- имя каталога с диалогами
		@param lpszWavsDir	- имя каталога со звуками
	 */
	bool make_scene_dir_struct2(const CString& strSceneName, 
								LPCTSTR lpszAnimationDir,
								LPCTSTR lpszStaticObjDir,
								LPCTSTR lpszDlgsDir,
								LPCTSTR lpszWavsDir
								);

	/// Создает набор каталогов для исходных матерьялов
	bool make_materials_dir_struct(LPCTSTR lpszDir, int nScenesCount);
	/// Создает набор каталогов для ресурсов проекта
	bool make_resource_dir_struct(LPCTSTR lpszDir, int nScenesCount);

private:
	/// Интерфейс класса, отображающего прогресс
	IShowProgress *m_ptrShowProgress;
};
