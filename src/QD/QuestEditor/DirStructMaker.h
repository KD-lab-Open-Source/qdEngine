#pragma once

#include "IShowProgress.h"

/**
	������� ��������� ���������
 */
class DirStructMaker
{
public:
	DirStructMaker(IShowProgress* psp);
	~DirStructMaker(void);

	/**
		������� �������.
		@param lpszDir - �����, � ������� ���� ������� ��������
		@param scens_count - ���������� ������� �����
	 */
	bool make(LPCTSTR lpszDir, int scenes_count);
private:
	/// ������� �����
	/**
		@return true - ���� ����� ������� ��� ��� ������������
	 */
	bool make_dir(LPCTSTR lpszDirName);
	/// ������� ����������� lpszSubDir � lpszDirBase
	bool make_sub_dir(LPCTSTR lpszDirBase, LPCTSTR lpszSubDir);

	/// ������� �������� ��������� ��� �������� �����, ����������� �������� ���������
	/**
		@param strSceneName - ������� �����
		@param lpszMovObjDir - ��� �������� ����������
		@param lpszDynObjDir - ��� �������� ������������� ��������
		@param lpszStaticObjDir - ��� �������� ����������� ��������
		@param lpszWavsDir	- ��� �������� �� �������
		@param lpszDlgsDir	- ��� �������� � ���������
	 */
	bool make_scene_dir_struct(const CString& strSceneName, 
								LPCTSTR lpszMovObjDir,
								LPCTSTR lpszDynObjDir,
								LPCTSTR lpszStaticObjDir,
								LPCTSTR lpszWavsDir,
								LPCTSTR lpszDlgsDir
								);
	/// @brief ������� �������� ��������� ��� �������� �����, 
	/// ����������� ��������� ��������
	/**
		@param strSceneName - ������� �����
		@param lpszAnimationDir - ��� �������� � ���������
		@param lpszStaticObjDir - ��� �������� ����������� ��������
		@param lpszDlgsDir	- ��� �������� � ���������
		@param lpszWavsDir	- ��� �������� �� �������
	 */
	bool make_scene_dir_struct2(const CString& strSceneName, 
								LPCTSTR lpszAnimationDir,
								LPCTSTR lpszStaticObjDir,
								LPCTSTR lpszDlgsDir,
								LPCTSTR lpszWavsDir
								);

	/// ������� ����� ��������� ��� �������� ����������
	bool make_materials_dir_struct(LPCTSTR lpszDir, int nScenesCount);
	/// ������� ����� ��������� ��� �������� �������
	bool make_resource_dir_struct(LPCTSTR lpszDir, int nScenesCount);

private:
	/// ��������� ������, ������������� ��������
	IShowProgress *m_ptrShowProgress;
};
