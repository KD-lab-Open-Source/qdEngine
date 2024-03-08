#pragma	once

#include <vector>

typedef std::vector<CString> CStringVect_t;
class CurDirSafer;

/**
	����� ������� ��� ������ � ������
 */
namespace disk_op
{
/// ����������� ������������ �� ����� ������
/**
	��������� FileOpen/Save ������. ��� ��� ���� ��������/������� ������������ � ptrFiles
	���� � ������ ������������ ������������ �������� �������. �� �������� ������� 
	�� ������ ������� � �������� ������� ����� ������
	\param hWnd - ������������ ����
	\param bNewFile		- ��������� �� ������� ��� ��������������� �����
	\param ptrFiles		- ������ ��������� ������������� ������
	\param strExt		- ���������� �� ��������� �� CFileDialog
	\param strFilter	- ������ �� CFileDialog 
	\param strTitle		- ��������� ������� �� CFileDialog
	\param strNewFileName	- ��� ������ ����� �� CFileDialog
	\param cur_dir_safer	- ������� ������� ��� �������
	\param dwFlags			- ��������� ����� �� CFileDialog
	\return true			- ���� ������ OK
 */
bool AskUser4FileNames(HWND hWnd, bool bNewFile, CStringVect_t* ptrFiles,
		const CString& strExt, const CString& strFilter,
		const CString& strTitle, const CString& strNewFileName,
		CurDirSafer& cur_dir_safer, DWORD dwFlags);

/// ���������� ��� ������! �����
/**
	���������� ������������� ���� � ���������� �����. ���� ������ ������, ���� ������
	�� �������
	\param get_dir_safer	- ���������� CurDirSafer, �������� ������� ���������
 */
const CString Get1DiskFileName(CurDirSafer& (*get_dir_safer)(), 
							   LPCTSTR strFilter,
							   LPCTSTR strExt, LPCTSTR strTitle, BOOL bOpen = TRUE);
/// ���������� ��� ������! �����
/**
	���������� ������������� ���� � ���������� �����. ���� ������ ������, ���� ������
	�� �������.
 */
const CString Get1DiskFileName(CurDirSafer& safer, 
							   LPCTSTR strFilter,
							   LPCTSTR strExt, LPCTSTR strTitle, BOOL bOpen = TRUE);
/// �� ���� ���������� ��� �����
CString GetFileName(const CString &strPath);
/// ���������� ���� � ����������
CString GetModulePath();
/// �� ������� ���� �������� ��� �����
CString GetPathFromFullFileName(CString str);

/// ��������� ������ ������ �������� �� �����
CString GetDiskFolder(HWND hWnd, LPCTSTR title, bool bAllowCreate, LPCTSTR initDir);

/// �� ����������� ���������� ��� ������������ �����
enum eFileType {FT_UNDEFINED, FT_QDA, FT_TGA};
eFileType GetImageFileType(LPCTSTR lpszFileName);


}