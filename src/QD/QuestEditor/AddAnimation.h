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
/// ������� ������������ �������� � ��������� �
const int AAA_EXISTING												= 1;
/// ������ ��� ���������� ����
const int AAA_CONV_FOR_SCENES										= 2;
/// ������ ��� ����� �����
const int AAA_CONV_FOR_SCENE										= 3;
/// ������ �� ������� ��������� ��������� 
const int AAA_CONV_FOR_SCENES_AND_CRT_USE_STRICT_FLDR_STRUCT		= 4;

// Declare a pointer to member function Func2.

/**
	��������� ������ �� ������ ��������
 */
class AddAnimation  
{
	typedef void (AddAnimation::*pmfnAddStep)();

	/// ����� ���������� ������������� ��� ��������� �������
	/**
		������ - ��� ��������� ���� �������, � ����� �����.
		��� ����� ������������� �������, ���� ��������� ��������� �����
	 */
	class name_tmpl{
		/// ���������� ���� 
		int m_iNameLen;
		/// ������� �������
		CString m_lpszMuster;

	public:
		name_tmpl():m_iNameLen(0){}

		name_tmpl(const name_tmpl&);
		const name_tmpl& operator = (const name_tmpl&);

		/// ���������� ��������� �����
		const CString name(){
			return m_lpszMuster.Left(m_iNameLen);
		}

		/// �������������� ������� � ���������� ���������� ����
		void add_first(LPCTSTR lpszStr);

		/// ��������� ���������� ��������� ����� ������� � ��������� �������
		bool test_name(LPCTSTR lpszStr) const;

		/// ��� ����, ����� ������������ � ����������� ����������
		bool operator() (LPCTSTR lpszStr) {
			if (m_lpszMuster.IsEmpty()) {
				add_first(lpszStr);
				return true;
			}
			return test_name(lpszStr);
		}
	};

	/// ��������� ����� ������ ����� ���������
	enum WHAT_CREATE{WC_NONE, ///< �� ����� ���������
		WC_ANIMATED, ///< ����� ��������� ������������� �������
		WC_MOVING///< ����� ��������� ��������
	};
	/// ������, ������� ����� ��������������, ����� ������ ������ "������"
	class break_process{
	};

	/// ��� ����� ��������� 
	WHAT_CREATE m_eWhatCreate;
	/// ������� �������
	CString m_strProjDir;
	/// ��� �������� �����, � ������� ����� ��������� ����������� ��������
	CString m_strSceneCatalog;
	/// ��� �������� � ������������� ���������
	CString m_strDynObjCatalog;
	/// ��� ������� � �����������
	CString m_strMovObjCatalog;
	/// �������, ��� ����������� �������
	CString m_strResCatalog;
	CString m_strSaveCatalog;///<  = m_strProjDir + m_strResCatalog
	/// ��� �������, � ������� ����� ��������� ��������
	CString m_strObjName;
	/// ������� ���� ����� ����������� ��������
	CString m_strAnimationSaveCatalog;
	/// ����������������� �����
	float m_fDuration;

	/// ��������� �� ��������-������
	IShowProgress* m_ptrShowProgress;
	/// ��������� �� �������, ������� ����������� ���
	pmfnAddStep m_ptrAddStep;

	/// 
	float m_fProgress;
	int m_iProgress;

	/// ��������� ��� ��� ������
	bool m_bCrtObj;
	qdGameDispatcher* m_ptrGameDisp;
	/// ��������� ������� ������� ��������
	qdGameDispatcherBase* m_ptrDispBase;

	bool SetAnimationFileName(qdAnimation* ptrAnim, const CString& strFileName);
	std::auto_ptr<qdAnimation> make_anim_from_files(CStringVect_t::const_iterator _itr, 
											const CStringVect_t::const_iterator& _end,
											const CString& strFileName);
	std::auto_ptr<qdAnimation> make_anim_from_folder(const CString& strFolder, 
		const CString& strFileName);

	/// ������� ��� ����������� � ��������� � ��������������� ����� ��������
	bool LoadToScenes(const CString &strFolder);

	/// ������� ������������� ��������, �������� �������� � ����� �� � ptrDisp
	bool LoadCatalogs(const CStringVect_t& vecCatalogs, qdGameDispatcherBase* ptrDisp);

	bool LoadExisting(const CStringVect_t& vecFiles, qdGameDispatcherBase* ptrDisp);

	qdGameScene* GetSceneByName(LPCTSTR lpszName);

	bool Animation2Disp(qdGameDispatcherBase* ptrDisp,
		const CString & strDirName, LPCTSTR strAnimName, 
		qdGameObjectAnimated* ptrAnimObj = NULL);

	/// ���������� �������� ��� ����������� ����� � ����� ��� ����������� ����������
	bool Animation2Buff(const CString & strDirName, 
		const CString &strSceneName, LPCTSTR strAnimName);

	qdGameScene* CreateNewScene(LPCTSTR strName);

	qdGameObjectAnimated* AddObj2Scene(qdGameScene* ptrScene, CString& strName);

	void AddStep();

	/**
		��������� �����, ��� ��� ������ ��������� �� ������������
		� �������� ����� ����� �������� ��������. � �������� ������� 
		����� �������� � ��������� ��� ����� �������
	*/
	bool LoadStrictSubDirs(const CString& strFolder);

	bool LoadObjectCatalogs(qdGameScene* ptrScene, const CString & strDirName);

	bool AddAnimation2Obj(qdGameScene* ptrScene, 
		qdGameObjectAnimated* ptrObj, 
		const CString &strDir,
		LPCTSTR strCatalogName);

	bool LoadGlobalAnimCatalog(CString strFolder);

	/// ��������� ��������, � ������� ����� ����� ��� ���������� �������� �����
	/// � ���������� ������ ��� �������� ������
	bool LoadCustomCadrCntObjects(qdGameScene* ptrScene, 
		const CString& strDirName, qdGameObjectAnimated* ptrObj);

	/// ��������� ������ ���� � �������� �����
	bool LoadSceneCatalog(const CStringVect_t& folders);
	/// ��������� ������ ���� � �������� ����� � ��� �������� �����
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
