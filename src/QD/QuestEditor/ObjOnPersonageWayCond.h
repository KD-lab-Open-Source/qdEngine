#pragma once


// ObjOnPersonageWayCond dialog
#include "CondDlgBase.h"
#include "afxwin.h"

class qdGameScene;
class qdNamedObject;

/**
	Объект на пути персонажа
 */
class ObjOnPersonageWayCond : public CondDlgBase
{
	DECLARE_DYNAMIC(ObjOnPersonageWayCond)

public:
	ObjOnPersonageWayCond(DWORD condId, CWnd* pParent = NULL);   // standard constructor
	virtual ~ObjOnPersonageWayCond();

// Dialog Data
	enum { IDD = IDD_DLG_COND_OBJ_ON_PERSONAGE_WAY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	/// Загружает анимированные объекты
	void LoadObjects();
	/// Загружает персонажи
	void LoadPersonages();
	/// Загружает сцены
	void LoadScenes();
	/// Возвращает выбранную сцену
	qdGameScene* getScene();
	/// Возвращает выбранного перса
	qdNamedObject* getPersonage();
	/// Возвращает выбранный аним объект
	qdNamedObject* getObject();
	/// Возвращает выбранный в комбо объект
	template<class ObjType>
		ObjType* getObjFromCombo(CCJFlatComboBox& box);

	/// Перзагружает объекты и персонажей
	afx_msg void OnCbnSelchangeComboScenes();
	/// Кладет персонаж в условие
	/**
		Выбранного персонажа удаляет из списка аним. объектов. А предыдущего выбранного
		добавляет
	 */
	afx_msg void OnCbnSelchangeComboObject();
	afx_msg void OnCbnSelchangeComboObject2();

	virtual bool IsCondDataComplete();

	/// Назначает предыдущего выбранного персонажа
	void setPrevPersonage(qdNamedObject* p);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
private:
	/**
		Загружает объекты в комбо используя фильтр
	 */
	template< class Filter>
	class ObjTypeToCombo
	{
		CCJFlatComboBox& m_box;
		Filter const& m_filter;
	public:
		ObjTypeToCombo(CCJFlatComboBox& box, Filter const& f);
		void operator()(qdNamedObject* pobj);
	};
private:
	CCJFlatComboBox m_wndScenes;
	//! Список персонажей
	CCJFlatComboBox m_wndPers;
	/// Список анимированных объектов, за исключением этого персонажа
	CCJFlatComboBox m_wndObjs;
	
	/// предыдущий выбранный пероснаж
	qdNamedObject* m_pPrevPers;
	//! Максимальное расстояние между объектами
	float m_fMaxDist;
public:
	afx_msg void OnDeltaposSpinMaxDist(NMHDR *pNMHDR, LRESULT *pResult);
};
