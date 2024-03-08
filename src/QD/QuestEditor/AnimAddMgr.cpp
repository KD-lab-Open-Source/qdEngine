// AnimAddMgr.cpp: implementation of the AnimAddMgr class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "questeditor.h"
#include "AnimAddMgr.h"

#include "AddAnimFirstWPg.h"

#include "xutil.h"

#include "name_op.h"
#include "relativepath.h"

#include "curdirsafer.h"

#include <io.h>
#include "file_enumerator.h"
#include "disk_op.h"

#include "scenelist.h"
#include "ptree_api.h"

#include "progdlg.h"

#include "qd_game_object_state.h"
#include "qd_animation_maker.h"
#include "qd_game_object_animated.h"
#include <Shlwapi.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

namespace{
TCHAR __CURRENT_DIR__[] = _T(".");
TCHAR __PARENT_DIR__[] = _T("..");
TCHAR NAME_INDEX_SEPARATOR = _T('_');
}
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

	inline CString make_relative_path(CString const& strProjDir, 
		CString const& strFullPath)
	{
		CString res;
		r_p::RelativePath(strFullPath, strProjDir, res);
		return res;
	}

}
//////////////////////////////////////////////////////////////////////////
/************************************************************************/
/* Subclasses                                                           */
/************************************************************************/
void AnimAddMgr::name_tmpl::add_first(LPCTSTR lpszStr)
{
	m_lpszMuster = lpszStr;
	TCHAR strDigs[] = _T("0123456789");
	int n = _tcslen(lpszStr);
	LPCTSTR res = std::find_first_of(lpszStr, lpszStr + n, strDigs, strDigs + sizeof(strDigs)/sizeof(TCHAR));
	LPCTSTR res2 = _tcsrchr(lpszStr, _T('.'));
	m_iNameLen = ::min(res - lpszStr, res2 - lpszStr);
}

bool AnimAddMgr::name_tmpl::test_name(LPCTSTR lpszStr) const
{
	return (_tcsnicmp(lpszStr, m_lpszMuster, m_iNameLen) == 0);
}

//////////////////////////////////////////////////////////////////////////

AnimAddMgr::AnimAddMgr()
{
	m_iProgress = 0;
	m_fProgress = 0.f;
	m_strAnimationSaveCatalog.LoadString(IDS_ANIMATION_SAVE_CATALOG);
}

AnimAddMgr::~AnimAddMgr()
{
	
}

bool AnimAddMgr::DO(qdGameDispatcher* ptrGameDisp, qdGameDispatcherBase* ptrBase,
					const CStringVect_t& vecStrs, int iAction, bool bCrtObj)
{
	return false;
#if 0
	m_ptrGameDisp = ptrGameDisp;
	m_ptrDispBase = ptrBase;
	m_bCrtObj = bCrtObj;

	CurDirSafer __SafeCurDir__;
	CurDirGuard __cdg(&__SafeCurDir__);

	m_strProjDir = CurDirSafer::GetCurrentDirectory() + _T("\\");

	m_strResCatalog.LoadString(IDS_RESOURCE_CATALOG);
	m_strDynObjCatalog.LoadString(IDS_DISK_DYNOBJ_CATALOG);

	m_strSaveCatalog = m_strProjDir + m_strResCatalog + _T("\\");
	
	CWaitCursor __wait__;

	switch(iAction) {
	case AAA_EXISTING:
		{
			LoadExisting(vecStrs, ptrBase);
			if (ptrGameDisp!=ptrBase&&m_bCrtObj) {
				ptree_api::UpdateObjCatalogInScene(static_cast<qdGameScene*>(ptrBase));
			}
		}
		break;
	case AAA_CONV_CATALOG:
		{
			LoadCatalogs(vecStrs, ptrBase);
			if (ptrGameDisp!=ptrBase&&m_bCrtObj) {
				ptree_api::UpdateAnimObjCatalog(static_cast<qdGameScene*>(ptrBase));
			}
		}
		break;
	case AAA_CONV_FOR_SCENES:
		LoadToScenes(vecStrs.front());
		break;
	case AAA_CONV_FOR_SCENES_AND_CRT:
		m_bCrtObj = true;
		LoadToScenes(vecStrs.front());
		ptree_api::UpdateCatalogsInScenes(QD_NAMED_OBJECT_ANIMATED_OBJ);
		break;
	case AAA_CONV_FOR_SCENES_AND_CRT_USE_STRICT_FLDR_STRUCT:
		LoadStrictSubDirs(vecStrs.front());
		ptree_api::UpdateCatalogsInScenes(QD_NAMED_OBJECT_ANIMATED_OBJ);
		break;
	default:	
		ASSERT(("DO default", 0));
	}
	AfxMessageBox("Операция завершена");
	return true;
#endif
}

const CString AnimAddMgr::GetObjName(qdGameDispatcherBase* ptrDisp, LPCTSTR str)
{
	qdGameScene* ptrScene = static_cast<qdGameScene*>(ptrDisp);
	return get_unique_name(str, ptrScene->object_list());
}

const CString AnimAddMgr::GetUniqQDAFileName(const CString& strMuster)
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

bool AnimAddMgr::SetAnimationFileName(qdAnimation* ptrAnim, const CString& strFileName)
{
	CurDirSafer __SafeCurDir__(m_strProjDir);
	CurDirGuard __cdg(&__SafeCurDir__);

	CString strSaveDir = m_strSaveCatalog + m_strSceneCatalog;
	if (!PathFileExists(strSaveDir)) {
		CreateDirectory(strSaveDir, NULL);
	}
	strSaveDir += _T("\\");
	strSaveDir += m_strAnimationSaveCatalog;

	if (!PathFileExists(strSaveDir)) {
		CreateDirectory(strSaveDir, NULL);
	}

	strSaveDir += _T("\\");

	CString strFN = m_strObjName;
	if(!strFN.IsEmpty())
		strFN += _T("_");

	strFN += strFileName;
	
	CString rel_name = make_relative_path(m_strProjDir, 
							strSaveDir + strFN);
	if(rel_name.IsEmpty())
		return false;

	rel_name = GetUniqQDAFileName(rel_name);
	
	ptrAnim->qda_save(rel_name);
	ptrAnim->qda_set_file(rel_name);
	ptrAnim->free_resources();
	return true;
}

std::auto_ptr<qdAnimation> AnimAddMgr::make_anim_from_files(
											const CStringVect_t* ptrFiles, 
											const CString& strFolder, 
											const CString& strFileName)
{
	std::auto_ptr<qdAnimation> ptrAnim(new qdAnimation);
	ptrAnim->set_flag(QD_ANIMATION_FLAG_CROP|QD_ANIMATION_FLAG_COMPRESS);
	
	qdAnimationMaker anim_maker;
	anim_maker.set_default_frame_length(0.0417f);
	
	CStringVect_t::const_iterator _itr = ptrFiles->begin(),_end = ptrFiles->end();
	
	int pos = -1;
	for(;_itr != _end; ++_itr)
	{
		anim_maker.insert_frame(ptrAnim.get(),*_itr, pos, true, true);
		AddStep();
	}

	ptrAnim->remove_edges();

	if(!ptrAnim->num_frames()
		||!SetAnimationFileName(ptrAnim.get(), strFileName))
		return std::auto_ptr<qdAnimation>();
	
	return ptrAnim;
}

std::auto_ptr<qdAnimation> AnimAddMgr::make_anim_from_folder(
													const CString& strFolder, 
													const CString& strFileName)
{
	CStringVect_t vecFiles;

	file_enumerator fe("*.tga");
	if (!fe.good()) {
		return std::auto_ptr<qdAnimation>();
	}

	std::copy(fe.begin(), fe.end(), std::back_inserter(vecFiles));

	if(vecFiles.empty())
		return std::auto_ptr<qdAnimation>();

	std::sort(vecFiles.begin(), vecFiles.end());
	
	return make_anim_from_files(&vecFiles, strFolder, strFileName);
}

bool AnimAddMgr::LoadToScenes(const CString&strFolder)
{
	CurDirSafer __SafeCurDir__(strFolder);
	CurDirGuard __cdg(&__SafeCurDir__);
	
	CProgressDlg dlg(IDS_PRGSS_DLG_CAP_ADD_ANIM);
	m_ptrProgDlg = &dlg;
	dlg.Create();
	
	//выбираем поочереди все поддиректории
	_finddata_t fData;
	LONG hFile = _findfirst("*.*",&fData);
	CString strFolderX = strFolder + _T("\\");

	file_enumerator fe("*.*");
	if (!fe) {
		return false;
	}

	file_enumerator::iterator _itr = fe.begin(), _end = fe.end();
	for(; _itr != _end; ++_itr)
	{
		const _tfinddata_t& fData = *_itr;

		if(fData.attrib&_A_SUBDIR)
		{
			if(strcmp(fData.name, __CURRENT_DIR__)
				&& strcmp(fData.name, __PARENT_DIR__)
				&& strcmp(fData.name, m_strResCatalog))
			{
				qdGameScene* ptrScene = GetSceneByName(fData.name);
				m_strSceneCatalog = fData.name;
				if(ptrScene)
					Animation2Disp(ptrScene, 
						strFolderX + fData.name + _T("\\"), fData.name);
				else
				{
					CString strCap((LPTSTR)IDS_ERR_MSG_CAPTION);
					CString strMsg;
					strMsg.Format(IDS_NO_SCENE_FOUND, fData.name);
					if(::MessageBox(NULL, strMsg, strCap, MB_OKCANCEL|MB_ICONWARNING) 
						== IDOK)
					{
						SceneList sl;
						sl.set_data(m_ptrGameDisp);
						if(sl.DoModal() == IDOK)
						{
							qdGameScene* ptrScene = sl.get_scene();
							if(!ptrScene)
								ptrScene = CreateNewScene(fData.name);

							m_strSceneCatalog = ptrScene->name();
							if(ptrScene)
								Animation2Disp(ptrScene, 
									strFolderX + fData.name + _T("\\"), fData.name);
						}
							//заносим в список необработанных
					}
				}
			}
		}
		AddStep();
	}
	
	m_ptrProgDlg->SetPos(100);
	return true;
}

qdGameScene* AnimAddMgr::GetSceneByName(LPCTSTR lpszName)
{
	return m_ptrGameDisp->get_scene(lpszName);
}

bool AnimAddMgr::Animation2Disp(qdGameDispatcherBase* ptrDisp,
								const CString & strDirName, LPCTSTR strAnimName, qdGameObjectAnimated* ptrAnimObj )
{
	AddStep();
	SetCurrentDirectory(strDirName);

	if (!ptrAnimObj&&m_bCrtObj) {
		m_strObjName = GetObjName(ptrDisp, strAnimName);
	}
	
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
				if(ptrDisp != m_ptrGameDisp&&m_bCrtObj)//если добавляем не глобальный каталог
				{
					CurDirSafer __safe(m_strProjDir);
					CurDirGuard __cdg(&__safe);
					qdGameObjectAnimated* ptrObj = AddObj2Scene(static_cast<qdGameScene*>(ptrDisp), m_strObjName);
					if(ptrObj)
						AddAnim2Obj(ptrObj, ptrAnim.get());
				}
			}
			ptrAnim.release();
		}
	}
	
	//выбираем поочереди все поддиректории
	file_enumerator fe("*.*");
	if(fe.good())
	{
		file_enumerator::iterator _itr = fe.begin(), _end = fe.end();
		
		for(;_itr != _end; ++_itr)
		{
			const file_find_data &fData = *_itr;
			if(fData.attrib&_A_SUBDIR)
			{
				if(strcmp(fData.name(), __CURRENT_DIR__)
					&& strcmp(fData.name(), __PARENT_DIR__))
				{
					Animation2Disp(ptrDisp, 
						strDirName + fData.name() + _T("\\"), fData.name(), ptrAnimObj);
				}
			}
		}
	}
	
	return true;
}

bool AnimAddMgr::LoadCatalogs(const CStringVect_t& vecCatalogs, 
							  qdGameDispatcherBase* ptrDisp)
{
	CStringVect_t::const_iterator _itr = vecCatalogs.begin(), 
		_end = vecCatalogs.end();

	CProgressDlg dlg(IDS_PRGSS_DLG_CAP_ADD_ANIM);
	m_ptrProgDlg = &dlg;
	dlg.Create();
	
	//в первом элементе содержиться родительская директория для остальных
	CString strRootDir = *_itr + "\\";
	++_itr;
	
	m_strSceneCatalog = ptrDisp->name();

	for(; _itr != _end; ++_itr){
		Animation2Disp(ptrDisp, strRootDir + *_itr + "\\", *_itr);
	}
	m_ptrProgDlg->SetPos(100);
	return true;
}

bool AnimAddMgr::LoadExisting(const CStringVect_t& vecFiles, 
							  qdGameDispatcherBase* ptrDisp)
{

	m_strSceneCatalog = ptrDisp->name();

	CStringVect_t::const_iterator _itr = vecFiles.begin(),
								_end = vecFiles.end();

	for(; _itr != _end; ++_itr)
	{
		//создаём и настраиваем анимацию
		std::auto_ptr<qdAnimation> pa (new qdAnimation);
		if(!pa.get())
			return NULL;
		
		CString str = disk_op::GetFileName(*_itr);

		str = get_unique_name(str, ptrDisp->animation_list());
		pa->set_name(str);
		pa->qda_set_file(*_itr);
		
		if(ptrDisp->add_animation(pa.get()))
		{
			qdAnimation* _pa = pa.release();
			if(ptrDisp != m_ptrGameDisp&&m_bCrtObj)
			{
				qdGameObjectAnimated* ptrObj = AddObj2Scene(static_cast<qdGameScene*>(ptrDisp), CString(_pa->name()));
				if(ptrObj)
					AddAnim2Obj(ptrObj, _pa);
			}
		}
	}
	return true;
}

qdGameScene* AnimAddMgr::CreateNewScene(LPCTSTR strName)
{
	CString strScene(strName);
	if(ptree_api::AddScene(strScene, false))
		return m_ptrGameDisp->get_scene(strScene);

	return NULL;
}

qdGameObjectAnimated* AnimAddMgr::AddObj2Scene(qdGameScene* ptrScene, CString& strName)
{
	return ptree_api::AddAnimated(ptrScene, strName, false);
}

namespace {
float get_progress(int val, float current_prog)
{
	return log(static_cast<float>(val))/MAX_LOG * 100.f;
}
}

void AnimAddMgr::AddStep()
{
	++m_iProgress;
	m_fProgress = get_progress(m_iProgress, m_fProgress);

	if (m_ptrProgDlg) {
		m_ptrProgDlg->SetPos(static_cast<int>(m_fProgress));
	}
}

bool AnimAddMgr::LoadStrictSubDirs(const CString &strFolder)
{
	CurDirSafer __SafeCurDir__(strFolder);
	CurDirGuard __cdg(&__SafeCurDir__);
	
	file_enumerator fe("*.*");
	if(!fe)
		return false;
	
	CProgressDlg dlg(IDS_PRGSS_DLG_CAP_ADD_ANIM);
	m_ptrProgDlg = &dlg;
	dlg.Create();

	CString strGlobalAnimCatalog((LPTSTR)IDS_GLOBAL_ANIMATION_CATALOG);

	//выбираем поочереди все поддиректории
	
	file_enumerator::iterator _itr(fe.begin()), _end(fe.end());
	
	CString strFolderX = strFolder + _T("\\");
	
	for(; _itr != _end; ++_itr)
	{
		const file_find_data &fData = *_itr;
		if(fData.attrib&_A_SUBDIR)
		{
			if(strcmp(fData.name(), __CURRENT_DIR__)
				&& strcmp(fData.name(), __PARENT_DIR__)
				&& strcmp(fData.name(), m_strResCatalog))
			{
				if (!stricmp(fData.name(), strGlobalAnimCatalog)) {
					LoadGlobalAnimCatalog(strFolderX + strGlobalAnimCatalog + _T("\\"));
				}
				else
				{
					m_strSceneCatalog = fData.name();
					qdGameScene* ptrScene = GetSceneByName(fData.name());
					if(ptrScene)
					{
						CString strDirName = strFolderX + fData.name() + _T("\\") + m_strDynObjCatalog + _T("\\");
						if(!PathFileExists(strDirName))
						{
							CString strMsg;
							strMsg.Format(IDS_CANT_FIND_DYNAMIC_OBJ_FOLDER, (LPCTSTR)m_strDynObjCatalog, fData.name());
							ShowError(AfxGetMainWnd()->m_hWnd, strMsg, IDS_ERR_MSG_CAPTION);
							
						}
						LoadObjectCatalogs(ptrScene, strDirName, fData.name());
					}
					else
					{
						CString strCap((LPTSTR)IDS_ERR_MSG_CAPTION);
						CString strMsg;
						strMsg.Format(IDS_NO_SCENE_FOUND, fData.name());
						if(::MessageBox(NULL, strMsg, strCap, MB_OKCANCEL|MB_ICONWARNING) == IDOK)
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
								{
									CString strDirName = strFolderX + 
										fData.name() + _T("\\") 
										+ m_strDynObjCatalog + _T("\\");
									if(!PathFileExists(strDirName))
									{
										CString strMsg;
										strMsg.Format(IDS_CANT_FIND_DYNAMIC_OBJ_FOLDER, 
											(LPCTSTR)m_strDynObjCatalog, fData.name());
										ShowError(AfxGetMainWnd()->m_hWnd, 
											strMsg, IDS_ERR_MSG_CAPTION);
										
									}
									LoadObjectCatalogs(ptrScene, 
										strDirName, fData.name());
								}
							}
							//заносим в список необработанных
						}
					}
				}
			}
		}
	}
	
	m_ptrProgDlg->SetPos(100);
	return true;
}


//рекурсивно обрабатываем подкталоги
bool AnimAddMgr::AddAnimation2Obj(qdGameScene* ptrScene, qdGameObjectAnimated* ptrObj, 
							const CString &strDir, LPCTSTR strCatalogName)
{
	SetCurrentDirectory(strDir);
	TRACE0(CurDirSafer::GetCurrentDirectory());
	TRACE0("\n");
	
	LoadCustomCadrCntObjects(ptrScene, strDir, ptrObj);
		
	file_enumerator fe("*.*");
	if (!fe) {
		return false;
	}
	file_enumerator::iterator _itr(fe.begin()), _end(fe.end());

	for(; _itr != _end; ++_itr)
	{
		const file_find_data & fData = *_itr;
		if(fData.attrib&_A_SUBDIR)
		{
			if(_tcscmp(fData.name(), __CURRENT_DIR__)
			&& _tcscmp(fData.name(), __PARENT_DIR__))
			{
				Animation2Disp(ptrScene, 
					strDir + fData.name() + _T("\\"), 
					fData.name(), ptrObj);
			}
		}
	}
			
	return true;
}


bool AnimAddMgr::LoadObjectCatalogs(qdGameScene* ptrScene,
									const CString & strDirName, LPCTSTR strCatalogName)
{
	//загружаем объекты состоящие из одного состояния
	if(!LoadCustomCadrCntObjects(ptrScene, strDirName, NULL))
		return false;

	if(!SetCurrentDirectory(strDirName))
	{
		return false;
	}
	
	file_enumerator fe("*.*");
	if (!fe) {
		return false;
	}

	file_enumerator::iterator _itr(fe.begin()), _end(fe.end());

	for(; _itr != _end; ++_itr)
	{
		const _tfinddata_t & fData = *_itr;
		if(fData.attrib&_A_SUBDIR)
		{
			if(strcmp(fData.name, __CURRENT_DIR__)
				&& strcmp(fData.name, __PARENT_DIR__))
			{
				qdGameObjectAnimated* ptrObj = static_cast<qdGameObjectAnimated*>(ptrScene->get_object(fData.name));
				m_strObjName = fData.name;

				if(ptrObj)
					AddAnimation2Obj(ptrScene, ptrObj, strDirName + fData.name + _T("\\"), fData.name);
				else
				{
					qdGameObjectAnimated* ptrObj = AddObj2Scene(ptrScene, CString(fData.name));
					if (ptrObj) {
						AddAnimation2Obj(ptrScene, ptrObj, 
								strDirName + fData.name + _T("\\"), 
								fData.name);
					}
					if (ptrObj->state_vector().empty()) {
						ptree_api::DeleteObject(ptrObj, false);
					}
				}
			}
		}
	}
	return true;
}

bool AnimAddMgr::LoadCustomCadrCntObjects(qdGameScene* ptrScene, const CString& strDirName, qdGameObjectAnimated* ptrObj)
{
	if(!SetCurrentDirectory(strDirName))
		return false;
	
	file_enumerator fe("*.tga");
	if (!fe) {
		return false;
	}
	
	file_enumerator::iterator _itr = fe.begin(), _end = fe.end();

	if(!ptrObj)
		m_strObjName.Empty();
	
	CStringVect_t vecFiles;
	
	for(; _itr != _end;)
	{
		name_tmpl nt;

		while (_itr != _end&&nt(_itr->name())) {
			vecFiles.push_back(*_itr);
			++_itr;
		}
		
		ASSERT(!vecFiles.empty());
		
		std::sort(vecFiles.begin(), vecFiles.end());
		
		CString strFileName = nt.name();

		std::auto_ptr<qdAnimation> ptrAnim = 
			make_anim_from_files(&vecFiles, strDirName, strFileName);
		
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
					qdGameObjectAnimated* ptrObj = 
						static_cast<qdGameObjectAnimated*>
											(ptrScene->get_object(strName));
					if (!ptrObj) {
						ptrObj = AddObj2Scene(ptrScene, strName);
					}
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

bool AnimAddMgr::LoadGlobalAnimCatalog(CString strFolder)
{
	if(!SetCurrentDirectory(strFolder))
		return false;

	file_enumerator fe("*.tga");
	if (!fe) {
		return false;
	}
	
	file_enumerator::iterator _itr = fe.begin(), _end = fe.end();

	CStringVect_t vecFiles;
	
	for(; _itr != _end;)
	{
		name_tmpl nt;

		while (_itr != _end&&nt(_itr->name())) {
			vecFiles.push_back(*_itr);
			++_itr;
		}
		
		ASSERT(!vecFiles.empty());
		
		std::sort(vecFiles.begin(), vecFiles.end());
		
		CString strFileName = nt.name();
		
		std::auto_ptr<qdAnimation> ptrAnim = make_anim_from_files(&vecFiles, strFolder, strFileName);
		
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