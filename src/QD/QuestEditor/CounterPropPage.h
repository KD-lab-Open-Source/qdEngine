#pragma once


#include "BasePage.h"
#include "Main/QETreeListCtrl.h"
#include <TreeListCtrl/Null Listeners/NullItemDragdropNotifyListener.h>
#include "qd_counter.h"
#include <boost/shared_ptr.hpp>
// CounterPropPage dialog

class qdGameDispatcher;
class qdCounter;

/*!
	Класс страници свойств счетчика
*/

class CounterPropPage : public CBasePage
					  , public NullItemDragdropNotifyListener
{
	DECLARE_DYNAMIC(CounterPropPage)

	typedef std::vector<boost::shared_ptr<qdCounterElement> > ElementsContainer;
public:
	CounterPropPage();
	virtual ~CounterPropPage();

// Dialog Data
	enum { IDD = IDD_PROPPAGE_COUNTER };

	//! Назначает диспетчер игры и счетчик
	void set_data(qdGameDispatcher* ptrDisp, qdCounter* ptrCounter);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	//! Обработчик нажатия "Применить"
	virtual BOOL OnApply();
	//! Инициализация диалога
	virtual BOOL OnInitDialog();
	//! Обработчик нажатия "Отменить"
	virtual void OnCancel();

	DECLARE_MESSAGE_MAP()
private:
	friend class eleCopy;
	
	/*!
	    Функтор. Копирует qdCounterelement и складывает его в контейнер
	*/
	class eleCopy
	{
		ElementsContainer &m_cont;
	public:
		eleCopy(ElementsContainer& cont);
		void operator()(qdCounterElement const& pce);
	};

	/*!
	    Функтор. Вставляет сцену в месте с её объектами в дерево
	*/
	friend class InsertScene;
	class InsertScene
	{
		QETreeListCtrl& m_wndTree;
	public:
		InsertScene(QETreeListCtrl& tree);
		void operator()(class qdGameScene const* pscene);
	};
	/*!
	    Функтор. Вставляет состояния объекта в дерево
	*/
	friend class InsertObject;
	class InsertObject
	{
		QETreeListCtrl& m_wndTree;
		CTreeListItem *m_ptrParent;
	public:
		InsertObject(QETreeListCtrl& tree, CTreeListItem * pParent);
		void operator()(class qdGameObject const* pobj);
	};

	/*!
	    Функтор. Вставляет состояние в дерево
	*/
	friend class InsertState;
	class InsertState
	{
		QETreeListCtrl& m_wndTree;
		CTreeListItem *m_ptrParent;
		bool m_bLoadGlobal;
	public:
		InsertState(QETreeListCtrl& tree, 
			CTreeListItem * pParent, bool bLoadGlobal);
		void operator()(class qdGameObjectState const* pstate);
	};

	/*!
	    Вставляет состояния в списки состояний, которые наращивают или уменьшают
		счетчик
	*/
	friend class InsertIncDecState;
	class InsertIncDecState
	{
		CounterPropPage* m_pOwnerPage;
	private:
		//! Вставляет состояние из элемената в указанный список
		void AddItem2List(QETreeListCtrl& lst, qdCounterElement const* pele);
	public:
		InsertIncDecState(CounterPropPage* page);
		//! \brief Определяет куда буде вставлено состояние.
		//! Выставляет картинку для состояния в общем дереве
		void operator()(boost::shared_ptr<qdCounterElement>& pele);
	};
private:
	//! Выставляет стили для списков уменьшающих/наращивающих состояний
	void setTreeListCtrlListStyles(QETreeListCtrl& ctrl);
	//! Выставляет стили для общего дерева состояний
	void setTreeListCtrlTreeStyles(QETreeListCtrl& ctrl);
	//! Копирует элеметы счетчика в собственный список
	void real2back();
	//! Копирует данные из собственного списка в объект.Устанавливает предельное значение
	void back2real();
	//! Загружает в дерево сцены с состояниями объектов, принадлежащих сцене
	void loadAllStates();
	//! Загружает состояния в списки состояний, уменьшающих/увеличивающих счетчик
	void loadIncDecStates();
	//! Наращивает ли состояние счетчик
	qdCounterElement* is_inc_state(qdGameObjectState const* pstate);
	//! Уменьшает ли состояние счетчик
	qdCounterElement* is_dec_state(qdGameObjectState const* pstate);
	//! Воздействет ли состояние на счетчик
	qdCounterElement* is_inc_or_dec_state(qdGameObjectState const* pstate);
	//! Воздействет ли состояние на счетчик
	static ElementsContainer::iterator 
		is_inc_or_dec_state_itr(ElementsContainer & lst,
									qdGameObjectState const* pstate);

	enum eAddStates{AS_ADD_INC, AS_ADD_DEC};
	//! добавляем состояния из дерева в соответствующий список
	void addStates(eAddStates eAct);
	//! удаляем из списка и отражаем удаление в дереве
	void deleteStates(QETreeListCtrl& list);
	//! отражает удаление состояния из какого-либо списка
	void reflectRemoveInTree(qdGameObjectState const* pstate);
	//! находит узел состояния в дереве состояний
	CTreeListItem* findStateItem(qdGameObjectState const* pstate);
	//! Находит узел в дереве по тексту
	CTreeListItem* findTreeItemByText(QETreeListCtrl& tree, 
		CTreeListItem* pParent, LPCTSTR lpszText);
	//! ищем узел по значению Item Data
	CTreeListItem* findTreeItemByData(QETreeListCtrl& tree, 
		CTreeListItem* pParent, DWORD_PTR lpszText);
	//! Выравнивает ширину первого стобца по ширине контрола
	void _1stcolumnWidth2ClientWidth(QETreeListCtrl& lst);
	//! Загружает глобальные состояния в дерево
	void loadGlobalStates();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	afx_msg void OnBnClickedCheckOnlyPositive();
	//! кликнули по кнопке добавить наращивающее состояние
	afx_msg void OnBnClickedButtonAddInc();
	//! кликнули по кнопке удалить наращивающее состояние
	afx_msg void OnBnClickedButtonDelInc();
	//! кликнули по кнопке добавить уменьшающее состояние
	afx_msg void OnBnClickedButtonAddDec();
	//! кликнули по кнопке удалить уменьшающее состояние
	afx_msg void OnBnClickedButtonDelDec();

	//! Уведомляет о том, что курсор вошел в область окна
	virtual bool onDragEnter(CTreeListCtrl& source, TREELISTDRAGDROP* ptldd);
	//! Уведомляет о том, что тянем курсор над окном
	virtual bool onDragOver(CTreeListCtrl& source, TREELISTDRAGDROP* ptldd);
	//! Уведомляет о том, что отжали кнопку мыши
	virtual bool onDrop(CTreeListCtrl& source, TREELISTDRAGDROP* ptldd);

	afx_msg void OnDeltaposSpinMaxValue(NMHDR *pNMHDR, LRESULT *pResult);
	void OnChangeLimitValue();

	QETreeListCtrl& getIncStates(){
		return m_wndIncStates;
	}
	QETreeListCtrl& getDecStates(){
		return m_wndDecStates;
	}
	QETreeListCtrl& getAllStates(){
		return m_wndAllStates;
	}
private:
	//! все состояния проекта
	QETreeListCtrl m_wndAllStates;
	//! состояния, которые наращивают счетчик
	QETreeListCtrl m_wndIncStates;
	//! состояния, которые уменьшают счетчик
	QETreeListCtrl m_wndDecStates;

	//! Собственный список элементов
	ElementsContainer m_elementsBackup;

	//! Подопытный счетчик
	qdCounter* m_ptrCounter;
	//! Диспетчер игры
	qdGameDispatcher* m_ptrGameDisp;

	CImageList m_treeImages;

	int m_iLimitValue;
	//! Счетчик принимает только положительные значения
	BOOL m_bOnlyPositive;
};
