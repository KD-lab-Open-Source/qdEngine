/********************************************************************
	created:	2002/11/27
	created:	27:11:2002   16:52
	filename: 	d:\projects\quest\questeditor\addanimation.cpp
	file path:	d:\projects\quest\questeditor
	file base:	addanimation
	file ext:	cpp
	Powered by:	Илюха
	
	purpose:	добавление анимации
*********************************************************************/
#include "stdafx.h"
#include "questeditor.h"
#include "AddAnimation.h"
#include "ishowprogress.h"

#include "xutil.h"
#include "qd_game_dispatcher.h"

#include "name_op.h"
#include "relativepath.h"

#include "curdirsafer.h"

#include <io.h>
#include "file_enumerator.h"

#include "scenelist.h"
#include "ptree_api.h"

#include "progdlg.h"

#include "qd_game_object_state.h"
#include "qd_animation_maker.h"
#include "qd_game_object_moving.h"
#include <Shlwapi.h>

#include "disk_op.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

TCHAR __CURRENT_DIR__[] = _T(".");
TCHAR __PARENT_DIR__[] = _T("..");
TCHAR NAME_INDEX_SEPARATOR = _T('_');

//максимум натурального логарифма от числа операций
//для 1 000 000 000 это значение должно быть 21 и больше
//для 1 000 000 - 14
//для 10 000 000 - 17
const float MAX_LOG = 15.f;

//////////////////////////////////////////////////////////////////////
// Внешние объявления
//////////////////////////////////////////////////////////////////////
bool AskForObject(qdGameScene* ptrScene, qdGameObjectAnimated*& pRes);
void ShowError(HWND hPar, DWORD dwMsgResID, DWORD dwCapResID = -1);
void ShowError(HWND hPar, LPCTSTR strMsg, DWORD dwCapResID = -1);

/************************************************************************/
/* Глобальные                                                           */
/************************************************************************/
namespace{

	/// Создает статическое состояние. Кладет в него анимацию, а состояние в объект
	inline bool AddAnim2Obj(qdGameObjectAnimated* ptrObj, qdAnimation* ptrAnim)
	{
		std::auto_ptr<qdGameObjectStateStatic> ptrState ( new qdGameObjectStateStatic);
		if (!ptrState.get()) {
			return false;
		}

		ptrState->set_name(get_unique_name(ptrAnim->name(), ptrObj->state_vector()));
		ptrState->animation_info()->set_animation_name(ptrAnim->name());
		ptrObj->add_state(ptrState.get());
		ptrObj->set_state(0);
		ptrObj->quant(0);
		ptrState.release();
		return true;
	}

	/// Получаем относительный путь к файлу анимации
	/**
		@param strProjDir - каталог проекта
		@param strFullPath - путь к файлу или каталогу, к которому нужен отностиельный
							путь
	 */
	inline CString make_relative_path(const CString& strProjDir, const CString& strFullPath)
	{
		CString res;
		r_p::RelativePath(strFullPath, strProjDir, res);
		return res;
	}

	/// Описывает форматы файлов
	enum eFileType{FT_UNKNOWN, ///< Незивестный
		FT_QDA, ///< Файл анимаций
		FT_TGA///< Картинка 
	};
	/// Определяет тип файла TGA, QDA или незивестно что.
	inline eFileType DetectFileType(const CString& strFileName){
		LPCTSTR res = PathFindExtension(strFileName);
		if (!res) 
			return FT_UNKNOWN;
		if (!_tcsicmp(res, _T(".qda"))) 
			return FT_QDA;
		if (!_tcsicmp(res, _T(".tga")))
			return FT_TGA;
		return FT_UNKNOWN;
	}
}
//////////////////////////////////////////////////////////////////////////
/************************************************************************/
/* Subclasses                                                           */
/************************************************************************/
void AddAnimation::name_tmpl::add_first(LPCTSTR lpszStr)
{
	m_lpszMuster = lpszStr;
	TCHAR strDigs[] = _T("0123456789");
	int n = _tcslen(lpszStr);
	LPCTSTR res = std::find_first_of(lpszStr, lpszStr + n, 
						strDigs, strDigs + sizeof(strDigs)/sizeof(TCHAR));
	LPCTSTR res2 = _tcsrchr(lpszStr, _T('.'));
	m_iNameLen = ::min(res - lpszStr, res2 - lpszStr);
}

bool AddAnimation::name_tmpl::test_name(LPCTSTR lpszStr) const
{
	return (_tcsnicmp(lpszStr, m_lpszMuster, m_iNameLen) == 0);
}

//////////////////////////////////////////////////////////////////////////

AddAnimation::AddAnimation(IShowProgress* psp)
{
	m_iProgress = 0;
	m_fProgress = 0.f;
	m_strAnimationSaveCatalog.LoadString(IDS_ANIMATION_SAVE_CATALOG);
	m_ptrShowProgress = psp;
	m_ptrAddStep = NULL;
	m_eWhatCreate = WC_NONE;
}

AddAnimation::~AddAnimation()
{

}

/// 	Входная функция
/**
	В зависимости от iAction выбирает алгоритм сборки и как будут использоваться строки
	в vecStrs
	@param ptrGameDisp	- диспетчер проекта
	@param ptrBase		- это может быть сцена или диспетчер проекта
	@param vecStrs		- хранит либо набор каталогов либо имя одного каталога. Все 
							зависи от iAction
	@param iAction		- определят алгоритм сборки
	@param bCrtObj		- создавать ли объекты
	@param fDur			- продолжительность кадра анимации
 */
bool AddAnimation::DO(qdGameDispatcher* ptrGameDisp, qdGameDispatcherBase* ptrBase,
					const CStringVect_t& vecStrs, int iAction, bool bCrtObj, float fDur)
{
	m_ptrGameDisp = ptrGameDisp;
	m_ptrDispBase = ptrBase;
	m_eWhatCreate = bCrtObj ? WC_ANIMATED : WC_NONE;
	m_fDuration = fDur;

	CurDirSafer __SafeCurDir__;
	CurDirGuard __cdg(&__SafeCurDir__);

	m_strProjDir = CurDirSafer::GetCurrentDirectory() + _T("\\");

	m_strResCatalog.LoadString(IDS_RESOURCE_CATALOG);
	m_strDynObjCatalog.LoadString(IDS_DISK_DYNOBJ_CATALOG);
	m_strMovObjCatalog.LoadString(IDS_DISK_CHARACTERS_CATALOG);

	m_strSaveCatalog = m_strProjDir + m_strResCatalog + _T("\\");

	m_ptrAddStep = &AddAnimation::AddStep;
	switch(iAction) {
	case AAA_EXISTING:
		{
			m_ptrAddStep = NULL;
			LoadExisting(vecStrs, ptrBase);
			if (ptrGameDisp!=ptrBase&&m_eWhatCreate != WC_NONE) 
			{
				ptree_api::UpdateObjCatalogInScene(QD_NAMED_OBJECT_ANIMATED_OBJ,
					static_cast<qdGameScene*>(ptrBase));
				ptree_api::UpdateObjCatalogInScene(QD_NAMED_OBJECT_MOVING_OBJ,
					static_cast<qdGameScene*>(ptrBase));
			}
		}
		break;
	case AAA_CONV_CATALOG:
		{
			LoadCatalogs(vecStrs, ptrBase);
			if (ptrGameDisp!=ptrBase&&m_eWhatCreate != WC_NONE) 
			{
				ptree_api::UpdateObjCatalogInScene(QD_NAMED_OBJECT_ANIMATED_OBJ,
					static_cast<qdGameScene*>(ptrBase));
				ptree_api::UpdateObjCatalogInScene(QD_NAMED_OBJECT_MOVING_OBJ,
					static_cast<qdGameScene*>(ptrBase));
			}
		}
		break;
	case AAA_CONV_FOR_SCENE:
		LoadSceneCatalog(vecStrs);
		ptree_api::UpdateCatalogsInScenes(QD_NAMED_OBJECT_ANIMATED_OBJ);
		ptree_api::UpdateCatalogsInScenes(QD_NAMED_OBJECT_MOVING_OBJ);
		break;
	case AAA_CONV_FOR_SCENES:
		LoadToScenes(vecStrs.front());
		if (m_eWhatCreate != WC_NONE) 
		{
			ptree_api::UpdateCatalogsInScenes(QD_NAMED_OBJECT_ANIMATED_OBJ);
			ptree_api::UpdateCatalogsInScenes(QD_NAMED_OBJECT_MOVING_OBJ);
		}
		break;

	case AAA_CONV_FOR_SCENES_AND_CRT_USE_STRICT_FLDR_STRUCT:
		LoadStrictSubDirs(vecStrs.front());
		ptree_api::UpdateCatalogsInScenes(QD_NAMED_OBJECT_ANIMATED_OBJ);
		ptree_api::UpdateCatalogsInScenes(QD_NAMED_OBJECT_MOVING_OBJ);
		break;
	default:	
		ASSERT(("DO default", 0));
	}
//	AfxMessageBox("Операция завешена");
	return true;
}

/**
	Возвращает уникальное имя объекта на основе str
 */
const CString AddAnimation::GetObjName(qdGameDispatcherBase* ptrDisp, LPCTSTR str)
{
	qdGameScene* ptrScene = static_cast<qdGameScene*>(ptrDisp);
	return get_unique_name(str, ptrScene->object_list());
}

/**
	Возвращает по образцу уникальное в каталоге имя файла.
	Файлы ищутся в текущем каталоге
 */
const CString AddAnimation::GetUniqQDAFileName(const CString& strMuster)
{
	int i = -1;
	CString strTestName;
	do {
		if (i != -1) {
			strTestName.Format(_T("%s%d.qda"), (LPCTSTR) strMuster, i);
		}
		else
			strTestName.Format(_T("%s.qda"), (LPCTSTR) strMuster);

		++i;
	} while(PathFileExists(strTestName));
	return strTestName;
}

/**
	Создает каталоги для сохранения.
	Заетм назначает имя файлу анимации и сохраняет его.
 */
bool AddAnimation::SetAnimationFileName(qdAnimation* ptrAnim, const CString& strFileName)
{
	CurDirSafer __SafeCurDir__(m_strProjDir);
	CurDirGuard __cdg(&__SafeCurDir__);

	/// Создаем катало с именем сцены
	CString strSaveDir = m_strSaveCatalog + m_strSceneCatalog;
	if (!PathFileExists(strSaveDir)) {
		CreateDirectory(strSaveDir, NULL);
	}
	strSaveDir += _T("\\");
	strSaveDir += m_strAnimationSaveCatalog;

	/// Создаем спец каталог для анимации
	if (!PathFileExists(strSaveDir)) {
		CreateDirectory(strSaveDir, NULL);
	}

	strSaveDir += _T("\\");

	CString strFN = m_strObjName;
	if(!strFN.IsEmpty())
		strFN += _T("_");

	/// пулчили полное имя файла анимации
	strFN += strFileName;

	CString rel_name = make_relative_path(m_strProjDir, 
		strSaveDir + strFN);
	if(rel_name.IsEmpty())
		return false;

	rel_name = GetUniqQDAFileName(rel_name);

	ptrAnim->qda_set_file(rel_name);
	ptrAnim->qda_save(ptrAnim->qda_file());
	ptrAnim->free_resources();

	return true;
}

/**
	Создает анимацию и вставляет в нее кадры адын за другым.
	Назначает имя файлу анимации.
	\return std::auto_ptr<qdAnimation>(), т.е. NULL если анимация пустая
	или не удалось назначить имя файлу
 */
std::auto_ptr<qdAnimation> AddAnimation::make_anim_from_files(
	CStringVect_t::const_iterator _itr, const CStringVect_t::const_iterator& _end, 
	const CString& strFileName)
{
	std::auto_ptr<qdAnimation> ptrAnim(new qdAnimation);
	ptrAnim->set_flag(QD_ANIMATION_FLAG_CROP|QD_ANIMATION_FLAG_COMPRESS);

	qdAnimationMaker anim_maker;
	anim_maker.set_default_frame_length(m_fDuration);

	int pos = -1;
	for(;_itr != _end; ++_itr)
	{
		anim_maker.insert_frame(ptrAnim.get(),*_itr, pos, true, true);
		if (m_ptrAddStep) 
			(this->*m_ptrAddStep)();
	}

	if(!ptrAnim->num_frames()||!SetAnimationFileName(ptrAnim.get(), strFileName))
		return std::auto_ptr<qdAnimation>();

	return ptrAnim;
}

/**
	Просто собирает все графические файлы из папки в анимацию
 */
std::auto_ptr<qdAnimation> AddAnimation::make_anim_from_folder(
	const CString& strFolder, 
	const CString& strFileName)
{
	CStringVect_t vecFiles;

	file_enumerator fe(_T("*.tga"));
	if (!fe.good()) {
		return std::auto_ptr<qdAnimation>();
	}

	std::copy(fe.begin(), fe.end(), std::back_inserter(vecFiles));

	if(vecFiles.empty())
		return std::auto_ptr<qdAnimation>();

	std::sort(vecFiles.begin(), vecFiles.end());

	return make_anim_from_files(vecFiles.begin(), vecFiles.end(), strFileName);
}

/**
	Получает каталог, каждый подкаталог которого содержит ресурсы для сцены 
	имя, которой совпадает с именем подкаталога. Если такой сцены нет, то она создается

	Для каждого подкаталога вызывается Animatin2Disp, которая собирает анимацию в сцены
 */
bool AddAnimation::LoadToScenes(const CString&strFolder)
{
	CurDirSafer __SafeCurDir__(strFolder);
	CurDirGuard __cdg(&__SafeCurDir__);


	//выбираем поочереди все поддиректории
	_finddata_t fData;
	LONG hFile = _findfirst(_T("*.*"),&fData);
	CString strFolderX = strFolder + _T("\\");

	file_enumerator fe(_T("*.*"));
	if (!fe) {
		return false;
	}

	m_ptrShowProgress->SetPrecents(0);

	file_enumerator::iterator _itr = fe.begin(), _end = fe.end();
	try{
		for(; _itr != _end; ++_itr)
		{
			const file_find_data& fData = *_itr;

			if(fData.attrib&_A_SUBDIR)
			{
				if(_tcscmp(fData.name(), __CURRENT_DIR__)
					&& _tcscmp(fData.name(), __PARENT_DIR__)
					&& _tcscmp(fData.name(), m_strResCatalog))
				{
					qdGameScene* ptrScene = GetSceneByName(fData.name());
					m_strSceneCatalog = fData.name();
					if(ptrScene)
						Animation2Disp(ptrScene, 
							strFolderX + fData.name() + _T("\\"), fData.name());
					else
					{
						CString strCap((LPTSTR)IDS_ERR_MSG_CAPTION);
						CString strMsg;
						strMsg.Format(IDS_NO_SCENE_FOUND, fData.name());
						if(::MessageBox(NULL, strMsg, strCap, MB_OKCANCEL|MB_ICONWARNING) 
							== IDOK)
						{
							SceneList sl;
							sl.set_data(m_ptrGameDisp);
							if(sl.DoModal() == IDOK)
							{
								qdGameScene* ptrScene = sl.get_scene();
								if(!ptrScene)
									ptrScene = CreateNewScene(fData.name());

								m_strSceneCatalog = ptrScene->name();
								if(ptrScene)
									Animation2Disp(ptrScene, 
										strFolderX + fData.name() + _T("\\"), fData.name());
							}
						}
					}
				}
			}
			AddStep();
			if (m_ptrShowProgress->BreakProcess())
				break;
		}
	}
	catch (break_process&) {
	}
	m_ptrShowProgress->SetPrecents(100);
	return true;
}

/**
	Возвращает объект сцены по имени
 */
qdGameScene* AddAnimation::GetSceneByName(LPCTSTR lpszName)
{
	return m_ptrGameDisp->get_scene(lpszName);
}

/**
	@param ptrDisp		- Диспетчер, куда будет добавлена анимация 
	@param strDirName	- Директория, из которой будут взяты матерьялы
	@param strAnimName	- имя анимации
	@param ptrAnimObj	- объект, в который анимация будет добавлена

	Сначала собирает анимацию из текущей папки. Потом идет глубже
*/
bool AddAnimation::Animation2Disp(qdGameDispatcherBase* ptrDisp,
								const CString & strDirName, 
								LPCTSTR strAnimName, 
								qdGameObjectAnimated* ptrAnimObj )
{
	AddStep();
	SetCurrentDirectory(strDirName);

	if (!ptrAnimObj&&m_eWhatCreate != WC_NONE) 
		m_strObjName = GetObjName(ptrDisp, strAnimName);

	std::auto_ptr<qdAnimation> ptrAnim  = make_anim_from_folder(strDirName, 
		CString(strAnimName));
	if(ptrAnim.get())
	{
		CString strName;
		if (!m_strObjName.IsEmpty()) {
			strName = m_strObjName;
			strName += _T("_");
		}
		strName += strAnimName;

		strName = get_unique_name(strName, ptrDisp->animation_list());
		ptrAnim->set_name(strName);

		if(!ptrDisp->add_animation(ptrAnim.get()))
		{
			//error handling
		}
		else
		{
			if (ptrAnimObj) {
				CurDirSafer __safe(m_strProjDir);
				CurDirGuard __cdg(&__safe);
				AddAnim2Obj(ptrAnimObj, ptrAnim.get());
			}
			else
			{
				//если добавляем не в глобальный каталог
				if(ptrDisp != m_ptrGameDisp&&m_eWhatCreate != WC_NONE)
				{
					CurDirSafer __safe(m_strProjDir);
					CurDirGuard __cdg(&__safe);
					qdGameObjectAnimated* ptrObj = 
						AddObj2Scene(static_cast<qdGameScene*>(ptrDisp), m_strObjName);
					if(ptrObj)
						AddAnim2Obj(ptrObj, ptrAnim.get());
				}
			}
			ptrAnim.release();
		}
	}

	//выбираем поочереди все поддиректории
	file_enumerator fe(_T("*.*"));
	if(fe.good())
	{
		file_enumerator::iterator _itr = fe.begin(), _end = fe.end();

		for(;_itr != _end; ++_itr)
		{
			const file_find_data &fData = *_itr;
			if(fData.attrib&_A_SUBDIR)
			{
				if(_tcscmp(fData.name(), __CURRENT_DIR__)
					&& _tcscmp(fData.name(), __PARENT_DIR__))
				{
					Animation2Disp(ptrDisp, 
						strDirName + fData.name() + _T("\\"), fData.name(), ptrAnimObj);
				}
			}
			if (m_ptrShowProgress->BreakProcess())
				throw break_process();
		}
	}

	return true;
}

/**
	Для каждого каталога в списке вызывает Animation2Disp.
	@param ptrDisp - сцена, куда будет добавлена анимация
 */
bool AddAnimation::LoadCatalogs(const CStringVect_t& vecCatalogs, 
							  qdGameDispatcherBase* ptrDisp)
{
	CStringVect_t::const_iterator _itr = vecCatalogs.begin(), 
		_end = vecCatalogs.end();

	m_ptrShowProgress->SetPrecents(0);

	//для какой сцены работаем
	m_strSceneCatalog = ptrDisp->name();

	try{
		for(; _itr != _end; ++_itr){

			CString strCatalogName = PathFindFileName(*_itr);
			Animation2Disp(ptrDisp, *_itr + _T('\\'), strCatalogName);
		}
	}
	catch (break_process&) {}
	m_ptrShowProgress->SetPrecents(100);
	return true;
}

/**
	Создает из каждого файла анимацию. Если это ТГА, то конвертирует его в 
	QDA и создает анимацию.
	При необходимости создаем объекты требуемого типа
 */
bool AddAnimation::LoadExisting(const CStringVect_t& vecFiles, 
							  qdGameDispatcherBase* ptrDisp)
{
	m_strSceneCatalog = ptrDisp->name();

	CStringVect_t::const_iterator _itr = vecFiles.begin(), _end = vecFiles.end();

	m_ptrShowProgress->SetPrecents(0);
	int step = static_cast<int>(100.f/vecFiles.size());
	int progress = 0;

	for(; _itr != _end; ++_itr)
	{

		CString strFileName = disk_op::GetFileName(*_itr);
		eFileType ftype = DetectFileType(*_itr);
		if (ftype == FT_UNKNOWN) 
			continue;

		std::auto_ptr<qdAnimation> pa;
		if (ftype == FT_TGA)
		{
			//прежде чем добавить конвертируем
			pa = make_anim_from_files(_itr, _itr + 1, strFileName);
 		}
		else if (ftype == FT_QDA) 
		{
			//создаём и настраиваем анимацию
			pa.reset(new qdAnimation);
			if(!pa.get())
				return NULL;

			pa->qda_set_file(*_itr);
		}
		strFileName = get_unique_name(strFileName, ptrDisp->animation_list());
		pa->set_name(strFileName);
		if(ptrDisp->add_animation(pa.get()))
		{
			qdAnimation* _pa = pa.release();
			if(ptrDisp != m_ptrGameDisp// т.е. ptrDisp указывает на сцену
				&&m_eWhatCreate != WC_NONE)
			{
				qdGameObjectAnimated* ptrObj = 
					AddObj2Scene(static_cast<qdGameScene*>(ptrDisp),CString(_pa->name()));
				if(ptrObj)
					AddAnim2Obj(ptrObj, _pa);
			}
		}

		progress += step;
		m_ptrShowProgress->SetPrecents(progress);
		if (m_ptrShowProgress->BreakProcess()) break;
	}
	return true;
}

/// Создает по имени сцену
qdGameScene* AddAnimation::CreateNewScene(LPCTSTR strName)
{
	CString strScene(strName);
	if(ptree_api::AddScene(strScene, false))
		return m_ptrGameDisp->get_scene(strScene);

	return NULL;
}

/// Добавляет в сцену объект требумоего типа с указанным именем
qdGameObjectAnimated* AddAnimation::AddObj2Scene(qdGameScene* ptrScene, CString& strName)
{
	if (m_eWhatCreate == WC_ANIMATED)
		return ptree_api::AddAnimated(ptrScene, strName, false);
	else if (m_eWhatCreate == WC_MOVING)
		return ptree_api::AddPersonage(ptrScene, strName, false);
	ASSERT(0);
	return NULL;
}

/// Функция замедляющегося прогресса.
/**
	Когда незвестно сколько делать шагов,
	делам прогресс постепенно замедляющимся, чтобы он никогда не достиг конца
 */
float get_progress(int val, float current_prog)
{
	return log(static_cast<float>(val))/MAX_LOG * 100.f;
}

/// Добавляет один шаг к прогессу
void AddAnimation::AddStep()
{
	++m_iProgress;
	m_fProgress = get_progress(m_iProgress, m_fProgress);
	m_ptrShowProgress->SetPrecents(static_cast<int>(m_fProgress));
}

/**
	Получает список каталогов. И для каждого из них вызывает
	AddAnimation::LoadSceneCatalog(const CString& strFolderX, LPCTSTR lpszDirName)
 */
bool AddAnimation::LoadSceneCatalog(const CStringVect_t& folders)
{
	m_ptrShowProgress->SetPrecents(0);
	CStringVect_t::const_iterator itr = folders.begin(), end = folders.end();
	try{
		for(; itr != end; ++itr){
			const CString& s = *itr;
			LoadSceneCatalog(s, PathFindFileName(s));
		}
	}catch (break_process&) {
	}
	m_ptrShowProgress->SetPrecents(100);
	return true;
}

/**
	Ищет сцену с именем lpszDirName, если не находит, то создает с таким именем.
	Находит каталог с матерьялами для анимированных объектов, обрабатывает её
	Тоже самое для каталога с матерьялами для динамических объектов

	@param strFolderX - полный путь к каталогу
	@parm lpszDirName - имя директории. Оно служит именем для сцены
 */
bool AddAnimation::LoadSceneCatalog(const CString& strFolderX, LPCTSTR lpszDirName){
	m_strSceneCatalog = lpszDirName;
	qdGameScene* ptrScene = GetSceneByName(lpszDirName);
	if(!ptrScene)
	{
		CString strCap((LPTSTR)IDS_ERR_MSG_CAPTION);
		CString strMsg;
		strMsg.Format(IDS_NO_SCENE_FOUND, lpszDirName);
		if(::MessageBox(NULL, strMsg, strCap, MB_OKCANCEL|MB_ICONWARNING) == IDOK){
			SceneList sl;
			sl.set_data(m_ptrGameDisp);
			if(sl.DoModal() == IDOK)
			{
				ptrScene = sl.get_scene();
				if(!ptrScene)
					ptrScene = CreateNewScene(lpszDirName);
				m_strSceneCatalog = ptrScene->name();
				if(!ptrScene)
					return false;
			}
			else
				return false;
		}
		else
			return false;
	}
	CString strDirName = strFolderX + _T("\\") + m_strDynObjCatalog + _T("\\");
	if(!PathFileExists(strDirName))
	{
		CString s;
		s.Format(IDS_CANT_FIND_DYNAMIC_OBJ_FOLDER, 
			(LPCTSTR)m_strDynObjCatalog, lpszDirName);
		ShowError(AfxGetMainWnd()->m_hWnd, s, IDS_ERR_MSG_CAPTION);
		return false;
	}
	else 
	{
		if (m_eWhatCreate != WC_NONE)
			m_eWhatCreate = WC_ANIMATED;
		LoadObjectCatalogs(ptrScene, strDirName);
	}

	strDirName =  strFolderX + _T("\\") + m_strMovObjCatalog + _T("\\");
	if(!PathFileExists(strDirName))
	{
		CString s;
		s.Format(IDS_CANT_FIND_DYNAMIC_OBJ_FOLDER, 
				(LPCTSTR)m_strMovObjCatalog, lpszDirName);
		ShowError(AfxGetMainWnd()->m_hWnd, s, IDS_ERR_MSG_CAPTION);
		return false;
	}
	else 
	{
		if (m_eWhatCreate != WC_NONE)
			m_eWhatCreate = WC_MOVING;
		LoadObjectCatalogs(ptrScene, strDirName);
	}
	return true;
}

/**
	Производит загрузку матерьялов из каталога со строгой структурой папок.
	Т.е. идут папки со сценами. В них папки с объектами и т.д.
 */
bool AddAnimation::LoadStrictSubDirs(const CString &strFolder)
{
	CurDirSafer __SafeCurDir__(strFolder);
	CurDirGuard __cdg(&__SafeCurDir__);

	file_enumerator fe(_T("*.*"));
	if(!fe)
		return false;

	m_ptrShowProgress->SetPrecents(0);

	CString strGlobalAnimCatalog((LPTSTR)IDS_GLOBAL_ANIMATION_CATALOG);

	//выбираем поочереди все поддиректории

	file_enumerator::iterator _itr(fe.begin()), _end(fe.end());

	CString strFolderX = strFolder + _T("\\");

	try
	{
		for(; _itr != _end; ++_itr)
		{
			const file_find_data &fData = *_itr;
			if(fData.attrib&_A_SUBDIR)
			{
				if(_tcscmp(fData.name(), __CURRENT_DIR__)
					&& _tcscmp(fData.name(), __PARENT_DIR__)
					&& _tcscmp(fData.name(), m_strResCatalog))
				{
					if (!stricmp(fData.name(), strGlobalAnimCatalog)) 
						/// Каталог с глобальной анимацией
						LoadGlobalAnimCatalog(strFolderX + strGlobalAnimCatalog + _T("\\"));
					else
						LoadSceneCatalog(strFolderX, fData.name());
				}
				if (m_ptrShowProgress->BreakProcess())
					break;
			}
		}
	}catch (break_process&) {
	}
	m_ptrShowProgress->SetPrecents(100);
	return true;
}


/**
	Рекурсивно обрабатываем подкталоги. 
	В каждом подкаталоге перечисляем все файлы и собираем из них анимацию
	Вся собранная анимация добавляется в сцену ptrScene. 
	И в объект добавляются состояния с этой анимацией
	@param ptrScene - сцена, в которую будет добавлена анимация
	@param ptrObj	- объект, в который будут добавлены состояния
	@param strDir	- каталог, который нужно считать
*/
bool AddAnimation::AddAnimation2Obj(qdGameScene* ptrScene, qdGameObjectAnimated* ptrObj, 
								  const CString &strDir, LPCTSTR strCatalogName)
{
	SetCurrentDirectory(strDir);
	TRACE0(CurDirSafer::GetCurrentDirectory());
	TRACE0("\n");

	/// собираем анимацию из каталога
	LoadCustomCadrCntObjects(ptrScene, strDir, ptrObj);

	file_enumerator fe("*.*");
	if (!fe) 
		return false;
	file_enumerator::iterator _itr(fe.begin()), _end(fe.end());

	for(; _itr != _end; ++_itr)
	{
		const file_find_data & fData = *_itr;
		if(fData.attrib&_A_SUBDIR)
		{
			if(_tcscmp(fData.name(), __CURRENT_DIR__)
				&& _tcscmp(fData.name(), __PARENT_DIR__))
				Animation2Disp(ptrScene, strDir + 
									fData.name() + _T("\\"), fData.name(), ptrObj);
		}
	}

	return true;
}

/**
	Сначала считывает анимацию из указанного каталога. И распихивает её по объектам
	Затем ищет подкаталоги и обрабатывает их
	@param ptrScene - сцена, в которую будет добавлена анимация
	@param strDir	- каталог, который нужно считать
 */
bool AddAnimation::LoadObjectCatalogs(qdGameScene* ptrScene, const CString & strDirName)
{
	//загружаем объекты состоящие из одного состояния
	if(!LoadCustomCadrCntObjects(ptrScene, strDirName, NULL))
		return false;

	if(!SetCurrentDirectory(strDirName))
		return false;

	file_enumerator fe("*.*");
	if (!fe) 
		return true;

	file_enumerator::iterator _itr(fe.begin()), _end(fe.end());
	for(; _itr != _end; ++_itr)
	{
		const file_find_data& fData = *_itr;
		if(fData.attrib&_A_SUBDIR)
		{
			if(_tcscmp(fData.name(), __CURRENT_DIR__)
				&& _tcscmp(fData.name(), __PARENT_DIR__))
			{
				qdGameObjectAnimated* ptrObj = 
					static_cast<qdGameObjectAnimated*>(ptrScene->get_object(fData.name()));

				m_strObjName = fData.name();
				if(ptrObj)
					AddAnimation2Obj(ptrScene, ptrObj, 
										strDirName + fData.name() 
										+ _T("\\"), fData.name());
				else
				{
					qdGameObjectAnimated* ptrObj = 
								AddObj2Scene(ptrScene, CString(fData.name()));
					if (ptrObj) 
						AddAnimation2Obj(ptrScene, ptrObj, 
										strDirName + fData.name()
										+ _T("\\"), fData.name());

					/// если ничего не добавили в объект удаляем его
					if (ptrObj->state_vector().empty()) 
						ptree_api::DeleteObject(ptrObj, false);
				}
			}
		}
	}
	return true;
}

/**
	Собирает все TGA-файлы. Файлы, имена которых удовлетворяют шаблону, собираются
	в анимацию. Анимация кладется в сцену, а потом состояние с этой анимацией
	добавляется в объект ptrObj. Если ptrObj == NULL, то для 
	каждой анимации создается новый объект
	@param ptrScene - сцена, в которую будет добавлена анимация
	@param ptrObj	- объект, в который будут добавлены состояния
	@param strDir	- каталог, который нужно считать
 */
bool AddAnimation::LoadCustomCadrCntObjects(qdGameScene* ptrScene, 
											const CString& strDirName, 
											qdGameObjectAnimated* ptrObj)
{
	if(!SetCurrentDirectory(strDirName))
		return false;

	file_enumerator fe(_T("*.tga"));
	if (!fe) 
		return true;

	file_enumerator::iterator _itr = fe.begin(), _end = fe.end();

	if(!ptrObj)
		m_strObjName.Empty();

	CStringVect_t vecFiles;

	for(; _itr != _end;)
	{
		name_tmpl nt;

		while (_itr != _end&&nt((*_itr).name())) {
			vecFiles.push_back(*_itr);
			++_itr;
		}

		ASSERT(!vecFiles.empty());

		std::sort(vecFiles.begin(), vecFiles.end());

		CString strFileName = nt.name();

		std::auto_ptr<qdAnimation> ptrAnim = 
			make_anim_from_files(vecFiles.begin(), vecFiles.end(), strFileName);

		if (ptrAnim.get()) {
			//вставляем имя
			CString strName;
			if (!m_strObjName.IsEmpty()) {
				strName = m_strObjName;
				strName += _T("_");
			}
			strName += strFileName;
			strName = get_unique_name(strName, ptrScene->animation_list());
			ptrAnim->set_name(strName);

			//добавляем в сцену
			if(ptrScene->add_animation(ptrAnim.get()))
			{
				//избавляемся от услуг auto_ptr, 
				//потому что анимация уже добавлена в сцену
				//и в случае чего её удалять нельзя
				qdAnimation * __pa = ptrAnim.release();

				if (ptrObj) {
					CurDirSafer __safe(m_strProjDir);
					CurDirGuard __cdg(&__safe);
					AddAnim2Obj(ptrObj, __pa);
				}
				else
				{
					//получаем объект
					qdGameObjectAnimated* ptrObj = static_cast<qdGameObjectAnimated*>
											(ptrScene->get_object(strName));
					if (!ptrObj) 
						ptrObj = AddObj2Scene(ptrScene, strName);

					//вставляем анимацию
					if (ptrObj) {
						CurDirSafer __safe(m_strProjDir);
						CurDirGuard __cdg(&__safe);
						AddAnim2Obj(ptrObj, __pa);
					}
				}
			}
		}
		vecFiles.clear();
	}

	return true;
}

/**
	Загружает каталог с глобальной анимацией
	Собирает все TGA-файлы. Файлы, имена которых удовлетворяют шаблону, собираются
	в анимацию. Анимация кладется в глобальный каталог
 */
bool AddAnimation::LoadGlobalAnimCatalog(CString strFolder)
{
	if(!SetCurrentDirectory(strFolder))
		return false;

	file_enumerator fe(_T("*.tga"));
	if (!fe) {
		return false;
	}

	file_enumerator::iterator _itr = fe.begin(), _end = fe.end();

	CStringVect_t vecFiles;

	for(; _itr != _end;)
	{
		name_tmpl nt;

		while (_itr != _end&&nt((*_itr).name())) {
			vecFiles.push_back(*_itr);
			++_itr;
		}

		ASSERT(!vecFiles.empty());

		std::sort(vecFiles.begin(), vecFiles.end());

		CString strFileName = nt.name();

		std::auto_ptr<qdAnimation> ptrAnim = 
			make_anim_from_files(vecFiles.begin(), vecFiles.end(), strFileName);

		if (ptrAnim.get()) {
			//вставляем имя
			CString strName;

			strName += strFileName;
			strName = get_unique_name(strName, m_ptrGameDisp->animation_list());
			ptrAnim->set_name(strName);

			//добавляем в сцену
			if(m_ptrGameDisp->add_animation(ptrAnim.get()))
			{
				//избавляемся от услуг auto_ptr, потому что анимация уже добавлена в сцену
				//и в случае чего её удалять нельзя
				ptrAnim.release();
			}
		}
		vecFiles.clear();
	}

	return true;
}