// AnimAddMgr.h: interface for the AnimAddMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ANIMADDMGR_H__85489DAF_EE25_4326_A61B_00EAF031D82A__INCLUDED_)
#define AFX_ANIMADDMGR_H__85489DAF_EE25_4326_A61B_00EAF031D82A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class qdGameDispatcherBase;
class qdGameObjectAnimated;
class qdGameDispatcher;
class qdAnimation;
class qdGameScene;

#include <map>

class CProgressDlg;

/**
	Не используется
 */
class AnimAddMgr  
{
	class name_tmpl{
		int m_iNameLen;
		CString m_lpszMuster;

	private:
	public:
		name_tmpl():m_iNameLen(0){}

		name_tmpl(const name_tmpl&);
		const name_tmpl& operator = (const name_tmpl&);

		const CString name(){
			return m_lpszMuster.Left(m_iNameLen);
		}

		void add_first(LPCTSTR lpszStr);

		bool test_name(LPCTSTR lpszStr) const;

		//для того, чтобы использовать в стандартных алгоритмах
		bool operator() (LPCTSTR lpszStr) {
			if (m_lpszMuster.IsEmpty()) {
				add_first(lpszStr);
				return true;
			}
			return test_name(lpszStr);
		}
	};

	CString m_strProjDir;
	CString m_strSceneCatalog;
	CString m_strDynObjCatalog;
	CString m_strResCatalog;
	CString m_strSaveCatalog;//m_strProjDir + m_strResCatalog
	CString m_strObjName;
	CString m_strAnimationSaveCatalog;

	CProgressDlg *m_ptrProgDlg;
	float m_fProgress;
	int m_iProgress;

	bool m_bCrtObj;
	qdGameDispatcher* m_ptrGameDisp;
	//описывает текущий каталог
	qdGameDispatcherBase* m_ptrDispBase;

	bool SetAnimationFileName(qdAnimation* ptrAnim, const CString& strFileName);
	std::auto_ptr<qdAnimation> make_anim_from_files(const CStringVect_t* ptrFiles, const CString& strFolder, const CString& strFileName);
	std::auto_ptr<qdAnimation> make_anim_from_folder(const CString& strFolder, 
										const CString& strFileName);

	//обходит все подкаталоги и добавляет в соответствующие
	//сцены анимацию
	bool LoadToScenes(const CString &strFolder);

	//обходит перечисленные каталоги, собирает анимацию
	//и кладёт ее в ptrDisp
	bool LoadCatalogs(const CStringVect_t& vecCatalogs, qdGameDispatcherBase* ptrDisp);

	bool LoadExisting(const CStringVect_t& vecFiles, qdGameDispatcherBase* ptrDisp);

	qdGameScene* GetSceneByName(LPCTSTR lpszName);

	bool Animation2Disp(qdGameDispatcherBase* ptrDisp,
						const CString & strDirName, LPCTSTR strAnimName, qdGameObjectAnimated* ptrAnimObj = NULL);
	//  [6/14/2002]
	//складывает анимацию для ненайденной сцены в буфер для последующей сортировки
	bool Animation2Buff(const CString & strDirName, 
						const CString &strSceneName, LPCTSTR strAnimName);

	qdGameScene* CreateNewScene(LPCTSTR strName);

	qdGameObjectAnimated* AddObj2Scene(qdGameScene* ptrScene, CString& strName);

	void AddStep();

	//загружает сцену, где все строго разложено по подкаталогам
	//в кталоге сцены лежат каталоги объектов. В каталоге объекта 
	//лежат каталоги с анимацией для этого объекта
	bool LoadStrictSubDirs(const CString& strFolder);

	bool LoadObjectCatalogs(qdGameScene* ptrScene,
		const CString & strDirName, LPCTSTR strCatalogName);

	bool AddAnimation2Obj(qdGameScene* ptrScene, 
					qdGameObjectAnimated* ptrObj, 
					const CString &strDir,
					LPCTSTR strCatalogName);

	bool LoadGlobalAnimCatalog(CString strFolder);
		
	//загружает каталоги, в которых лежат кадры для нескольких объектов сразу
	bool LoadCustomCadrCntObjects(qdGameScene* ptrScene, 
								const CString& strDirName,
								qdGameObjectAnimated* ptrObj);

	const CString GetObjName(qdGameDispatcherBase* ptrDisp, LPCTSTR str);
	const CString GetUniqQDAFileName(const CString& strMuster);
public:
	AnimAddMgr();
	~AnimAddMgr();

	bool DO(qdGameDispatcher* ptrGameDisp, qdGameDispatcherBase* ptrBase,
			const CStringVect_t& vecStrs, int iAction, bool bCrtObj);
};

#endif //!defined(AFX_ANIMADDMGR_H__85489DAF_EE25_4326_A61B_00EAF031D82A__INCLUDED_)
