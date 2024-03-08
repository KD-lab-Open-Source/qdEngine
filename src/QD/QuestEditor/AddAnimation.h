#ifndef	 _ADD_ANIMATION_H_

#define  _ADD_ANIMATION_H_
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class qdGameDispatcherBase;
class qdGameObjectAnimated;
class qdGameDispatcher;
class qdAnimation;
class qdGameScene;

#include <map>

interface IShowProgress;

const int AAA_CONV_CATALOG											= 0;
/// Находит сущуствующую анимацию и добавляет её
const int AAA_EXISTING												= 1;
/// Сборка для нескольких сцен
const int AAA_CONV_FOR_SCENES										= 2;
/// Сборка для одной сцены
const int AAA_CONV_FOR_SCENE										= 3;
/// Сборка по строгой структуры каталогов 
const int AAA_CONV_FOR_SCENES_AND_CRT_USE_STRICT_FLDR_STRUCT		= 4;

// Declare a pointer to member function Func2.

/**
	Выполняет работу по сборке анимации
 */
class AddAnimation  
{
	typedef void (AddAnimation::*pmfnAddStep)();

	/// Класс определяет удовлетворяет имя заданному шаблону
	/**
		Шаблон - это несколько букв сначала, а потом цифры.
		Имя файла удовлетворяет шаблону, если совпадает буквенная часть
	 */
	class name_tmpl{
		/// Количество букв 
		int m_iNameLen;
		/// Строчка образец
		CString m_lpszMuster;

	public:
		name_tmpl():m_iNameLen(0){}

		name_tmpl(const name_tmpl&);
		const name_tmpl& operator = (const name_tmpl&);

		/// Возвращает буквенную часть
		const CString name(){
			return m_lpszMuster.Left(m_iNameLen);
		}

		/// Инициализирует образец и определяет количество цифр
		void add_first(LPCTSTR lpszStr);

		/// Проверяет совпадение буквенной части образца с указанной строкой
		bool test_name(LPCTSTR lpszStr) const;

		/// для того, чтобы использовать в стандартных алгоритмах
		bool operator() (LPCTSTR lpszStr) {
			if (m_lpszMuster.IsEmpty()) {
				add_first(lpszStr);
				return true;
			}
			return test_name(lpszStr);
		}
	};

	/// описывает какой объект будем создавать
	enum WHAT_CREATE{WC_NONE, ///< Не будем создавать
		WC_ANIMATED, ///< Будем создавать анимированные объекты
		WC_MOVING///< Будем создавать персонаж
	};
	/// ошибка, которая будет генерироваться, когда нажмут кнопку "Отмена"
	class break_process{
	};

	/// Что будем создавать 
	WHAT_CREATE m_eWhatCreate;
	/// Каталог проекта
	CString m_strProjDir;
	/// Имя каталога сцены, в котором будет сохранена создаваемая анимация
	CString m_strSceneCatalog;
	/// Имя каталога с динамическими объектами
	CString m_strDynObjCatalog;
	/// Имя катлога с персонажами
	CString m_strMovObjCatalog;
	/// Каталог, где расположены ресурсы
	CString m_strResCatalog;
	CString m_strSaveCatalog;///<  = m_strProjDir + m_strResCatalog
	/// Имя объекта, в который будет добавлена анимация
	CString m_strObjName;
	/// Каталог куда будет сохраняться анимация
	CString m_strAnimationSaveCatalog;
	/// продолжительность кадра
	float m_fDuration;

	/// Указатель на прогресс-диалог
	IShowProgress* m_ptrShowProgress;
	/// указатель на функцию, которая увеличивает шаг
	pmfnAddStep m_ptrAddStep;

	/// 
	float m_fProgress;
	int m_iProgress;

	/// Создавать или нет объект
	bool m_bCrtObj;
	qdGameDispatcher* m_ptrGameDisp;
	/// описывает текущий каталог анимации
	qdGameDispatcherBase* m_ptrDispBase;

	bool SetAnimationFileName(qdAnimation* ptrAnim, const CString& strFileName);
	std::auto_ptr<qdAnimation> make_anim_from_files(CStringVect_t::const_iterator _itr, 
											const CStringVect_t::const_iterator& _end,
											const CString& strFileName);
	std::auto_ptr<qdAnimation> make_anim_from_folder(const CString& strFolder, 
		const CString& strFileName);

	/// обходит все подкаталоги и добавляет в соответствующие сцены анимацию
	bool LoadToScenes(const CString &strFolder);

	/// обходит перечисленные каталоги, собирает анимацию и кладёт ее в ptrDisp
	bool LoadCatalogs(const CStringVect_t& vecCatalogs, qdGameDispatcherBase* ptrDisp);

	bool LoadExisting(const CStringVect_t& vecFiles, qdGameDispatcherBase* ptrDisp);

	qdGameScene* GetSceneByName(LPCTSTR lpszName);

	bool Animation2Disp(qdGameDispatcherBase* ptrDisp,
		const CString & strDirName, LPCTSTR strAnimName, 
		qdGameObjectAnimated* ptrAnimObj = NULL);

	/// складывает анимацию для ненайденной сцены в буфер для последующей сортировки
	bool Animation2Buff(const CString & strDirName, 
		const CString &strSceneName, LPCTSTR strAnimName);

	qdGameScene* CreateNewScene(LPCTSTR strName);

	qdGameObjectAnimated* AddObj2Scene(qdGameScene* ptrScene, CString& strName);

	void AddStep();

	/**
		Загружает сцену, где все строго разложено по подкаталогам
		в каталоге сцены лежат каталоги объектов. В каталоге объекта 
		лежат каталоги с анимацией для этого объекта
	*/
	bool LoadStrictSubDirs(const CString& strFolder);

	bool LoadObjectCatalogs(qdGameScene* ptrScene, const CString & strDirName);

	bool AddAnimation2Obj(qdGameScene* ptrScene, 
		qdGameObjectAnimated* ptrObj, 
		const CString &strDir,
		LPCTSTR strCatalogName);

	bool LoadGlobalAnimCatalog(CString strFolder);

	/// загружает каталоги, в которых лежат кадры для нескольких объектов сразу
	/// и количество кадров для объектов разное
	bool LoadCustomCadrCntObjects(qdGameScene* ptrScene, 
		const CString& strDirName, qdGameObjectAnimated* ptrObj);

	/// принимает полный путь к каталогу сцены
	bool LoadSceneCatalog(const CStringVect_t& folders);
	/// принимает полный путь к каталогу сцены и имя каталога сцены
	bool LoadSceneCatalog(const CString& strFolderX, LPCTSTR lpszDirName);

	const CString GetObjName(qdGameDispatcherBase* ptrDisp, LPCTSTR str);
	const CString GetUniqQDAFileName(const CString& strMuster);
public:
	AddAnimation(IShowProgress* psp);
	~AddAnimation();

	bool DO(qdGameDispatcher* ptrGameDisp, qdGameDispatcherBase* ptrBase,
		const CStringVect_t& vecStrs, int iAction, bool bCrtObj, float fDur);
};

#endif
