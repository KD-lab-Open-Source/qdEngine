#if !defined(AFX_SELANIMOBJ_H__822A0BF7_490C_4BF5_9214_46889EF15953__INCLUDED_)
#define AFX_SELANIMOBJ_H__822A0BF7_490C_4BF5_9214_46889EF15953__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SelAnimObj.h : header file
//

class qdGameScene;
class qdGameObjectAnimated;

/////////////////////////////////////////////////////////////////////////////
// SelAnimObj dialog

/**
	Класс диалог. Позволяет из объектов определенныго типа, принадлежащих сцене,
	выбрать один или создать новый
 */
class SelGameObj : public CDialog
{
	/// Сцена
	qdGameScene		*m_ptrScene;
	/// Объект, который выбрали в списке
	qdGameObject	*m_ptrObj;
	/// Тип объектов (например QD_NAMED_OBJECT_STATIC_OBJ)
	int				m_type;
	/// Можно ли создать новый
	bool			m_bCanCrtNew;
// Construction
public:
	SelGameObj(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(SelAnimObj)
	enum { IDD = IDD_DLG_SEL_ANIM_OBJ };
	CListCtrl	m_list;
	//}}AFX_DATA

	/// Setter for m_ptrScene
	bool set_data(qdGameScene* ptrScene);
	/// Setter for m_type
	void set_type(int type);
	/// Setter for m_bCanCrtNew
	void set_can_crt_new(bool bf);

	/// Возвращает выбранный объект. NULL - надо создать объект
	qdGameObject* get_object(){return m_ptrObj;}

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SelAnimObj)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(SelAnimObj)
	afx_msg void OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP();

	/// Загружает список объектов
	bool LoadList();
private:
	/// Проверяет является ли объект правильного типа
	class is_right_type{
		/// правильный тип
		int type_;
	public:
		is_right_type(int type):type_(type){}
		bool operator()(qdGameObject const* pobj) const{
			return (pobj->named_object_type()==type_);
		}
	};
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //!defined(AFX_SELANIMOBJ_H__822A0BF7_490C_4BF5_9214_46889EF15953__INCLUDED_)
