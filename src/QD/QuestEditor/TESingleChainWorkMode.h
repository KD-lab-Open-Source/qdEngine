/********************************************************************
	created:	2003/05/23
	created:	23:5:2003   12:23
	filename: 	d:\Projects\Quest\QuestEditor\TESingleChainWorkMode.h
	file path:	d:\Projects\Quest\QuestEditor
	file base:	TESingleChainWorkMode
	file ext:	h
	Powerd by:	Илюха  при идейном руководстве и моральной поддержке of Scmorr
	
	purpose:	Базовый объект для режимов, которые работают с одной цепочкой
*********************************************************************/
#pragma once
#include "teworkmodebase.h"
#include "IDrawData.h"
#include "qd_trigger_element.h"

#include "SelectionManager.h"

class TEWnd;

/**
	РР для работы с одной цепочкой
 */
class TESingleChainWorkMode :public TEWorkModeBase
{
protected:
	/**
		Содержит данные для проприсовки
	 */
	class DataKeeper : public IDrawData
	{
		/// Цепочка для отрисовки
		class qdTriggerChain const* m_ptrChain;
	public:
		DataKeeper(qdTriggerChain const* pchain) : m_ptrChain(pchain)
		{}
		/// Назначает цепочку
		void setTriggerChain(qdTriggerChain const* pchain){
			m_ptrChain = pchain;
		}
		std::pair<qdTriggerChain const* const*, int> getDrawedChains() const{
			return std::make_pair(&m_ptrChain, 1);
		}
	};
protected:
	TESingleChainWorkMode(SelectionManager& selectionManger);
	~TESingleChainWorkMode(void);

public:
	/**
		Обрабатывает целых 2 команды
		IDC_TE_SHOW_CONDITION и IDC_TE_PROPETIES. В остальных случаях возвращает FALSE
	 */
	BOOL OnCommand(TEWnd* pwnd, WPARAM wParam, LPARAM lParam);
	/**
		Если окно показывается, то запускает тултип, если скрывается, 
		то сбрасывает тултип
	 */
	void OnShowWindow(TEWnd* pwnd, BOOL bShow, UINT nStatus);

	/// Назначает цепочку
	virtual void setChain(qdTriggerChain* pchain);
	/// Возвращает цепочку
	virtual qdTriggerChain* getChain() const{
		return m_ptrChain;
	}
protected:

	/**
		Показывает свойства объекта, хранящегося в элементе, выбранным последним
		Весь выборс сбрасывается, за исключением объекта, на который кликнули
	 */
	void OnTeProperties(TEWnd* pwnd);
	/**
		Показывает условия для объекта, хранящегося в элементе, выбранным последним
		Весь выборс сбрасывается, за исключением объекта, на который кликнули
	 */
	void OnTeShowCondition(TEWnd* pwnd);
	/**
		Находит элемент для точки. Если он не совпадет с предыдущим элементом,
		то меняем текст тултипа.
		Показываем тултип в точке point
	 */
	virtual void trackToolTip(TEWnd* pwnd, CPoint point);
	/// Возвращает selectionManager_
	SelectionManager& getSelectionManager();
	/// Возвращает константный selectionManager_
	SelectionManager const& getSelectionManager() const;

	/// Назначает базовый элемент
	void setBaseElement(qdTriggerElementPtr const& element);
	/// Возвращает базовый элемент
	qdTriggerElementPtr const& getBaseElement() const;

protected:
	/**
		При своем разрушении выставляет базовый элемент в NULL.
		Нужен, чтобы не зыбывать это делать самому
	 */
	class BaseElementGuard{
		TESingleChainWorkMode* workMode_;
	public:
		BaseElementGuard(TESingleChainWorkMode* workMode):workMode_(workMode){}
		~BaseElementGuard(){
			workMode_->setBaseElement(NULL);
		}
	};
private:
	/// цепочка, с которой выполняется работа
	qdTriggerChain *m_ptrChain;
	/// Объект управляющий выбранностью элементов
	SelectionManager& selectionManager_;
	//! Элемент, на котором кликнули мышкой
	qdTriggerElementPtr baseElement_;
};
