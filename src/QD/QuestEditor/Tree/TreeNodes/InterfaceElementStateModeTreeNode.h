#pragma once
#include "interfaceobjecttreenode.h"

#include "qd_interface_element_state.h"
		  
/// Узел режима состояния интерфейсного элемента
class InterfaceElementStateModeTreeNode : public InterfaceObjectTreeNode
{
public:
	InterfaceElementStateModeTreeNode(qdInterfaceElementState* object,
			qdInterfaceElementState::state_mode_t state_mode);
	~InterfaceElementStateModeTreeNode(void);
	bool load(ITreeNodeFactory* factory, TreeLogic& treeLogic,
		ITreeNode * parent, qdNamedObjectBase const* owner,
		ITreeNode* insertAfter);
	//! Возвращает идентификатор ресурса меню для узла
	DWORD getMenuResId() const;
	virtual bool showProperties(TreeLogic& logic, int activePage);
	virtual bool onStateChange(TreeLogic& logic, DWORD oldState, DWORD newState);
protected:
	/// Возвращает идентификатор режима состояния
	qdInterfaceElementState::state_mode_t getStateMode() const;
	/// Возвращает имя режима состояния
	static CString const selectName(qdInterfaceElementState::state_mode_t);
private:
	/// Идентификатор режима состояния
	qdInterfaceElementState::state_mode_t stateMode_;
};
