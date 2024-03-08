#pragma	once

#include <vector>

typedef std::vector<CString> CStringVect_t;
class CurDirSafer;

/**
	Набор функций для работы с диском
 */
namespace disk_op
{
/// Запрашивает пользователя об имени файлов
/**
	Открывает FileOpen/Save диалог. Все что было выбранно/введено возвращается в ptrFiles
	Пути к файлам возвращаются относительно каталога проекта. Он является текущим 
	до вызова функции и остается текущим после вызова
	\param hWnd - родительское окно
	\param bNewFile		- позволять ли вводить имя несуществующего файла
	\param ptrFiles		- список выбранных пользователем файлов
	\param strExt		- расширение по умолчанию см CFileDialog
	\param strFilter	- фильтр см CFileDialog 
	\param strTitle		- заголовок диалога см CFileDialog
	\param strNewFileName	- имя нового файла см CFileDialog
	\param cur_dir_safer	- текущий каталог для запроса
	\param dwFlags			- различные флаги см CFileDialog
	\return true			- если нажали OK
 */
bool AskUser4FileNames(HWND hWnd, bool bNewFile, CStringVect_t* ptrFiles,
		const CString& strExt, const CString& strFilter,
		const CString& strTitle, const CString& strNewFileName,
		CurDirSafer& cur_dir_safer, DWORD dwFlags);

/// Возвращает имя одного! файла
/**
	Возвращает относительный путь к выбранному файлу. Либо пустую строку, если ничего
	не выбрали
	\param get_dir_safer	- возвращает CurDirSafer, хранящий текущую директоию
 */
const CString Get1DiskFileName(CurDirSafer& (*get_dir_safer)(), 
							   LPCTSTR strFilter,
							   LPCTSTR strExt, LPCTSTR strTitle, BOOL bOpen = TRUE);
/// Возвращает имя одного! файла
/**
	Возвращает относительный путь к выбранному файлу. Либо пустую строку, если ничего
	не выбрали.
 */
const CString Get1DiskFileName(CurDirSafer& safer, 
							   LPCTSTR strFilter,
							   LPCTSTR strExt, LPCTSTR strTitle, BOOL bOpen = TRUE);
/// По пути возвращает имя файла
CString GetFileName(const CString &strPath);
/// Возвращает путь к приложению
CString GetModulePath();
/// Из полного пути вырезает имя файла
CString GetPathFromFullFileName(CString str);

/// Открывает диалог выбора каталога на диске
CString GetDiskFolder(HWND hWnd, LPCTSTR title, bool bAllowCreate, LPCTSTR initDir);

/// По рассширению определяет тип графического файла
enum eFileType {FT_UNDEFINED, FT_QDA, FT_TGA};
eFileType GetImageFileType(LPCTSTR lpszFileName);


}