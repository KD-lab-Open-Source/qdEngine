#pragma once
#include "interfaceobjecttreenode.h"

#include "qd_interface_element_state.h"
		  
/// ���� ������ ��������� ������������� ��������
class InterfaceElementStateModeTreeNode : public InterfaceObjectTreeNode
{
public:
	InterfaceElementStateModeTreeNode(qdInterfaceElementState* object,
			qdInterfaceElementState::state_mode_t state_mode);
	~InterfaceElementStateModeTreeNode(void);
	bool load(ITreeNodeFactory* factory, TreeLogic& treeLogic,
		ITreeNode * parent, qdNamedObjectBase const* owner,
		ITreeNode* insertAfter);
	//! ���������� ������������� ������� ���� ��� ����
	DWORD getMenuResId() const;
	virtual bool showProperties(TreeLogic& logic, int activePage);
	virtual bool onStateChange(TreeLogic& logic, DWORD oldState, DWORD newState);
protected:
	/// ���������� ������������� ������ ���������
	qdInterfaceElementState::state_mode_t getStateMode() const;
	/// ���������� ��� ������ ���������
	static CString const selectName(qdInterfaceElementState::state_mode_t);
private:
	/// ������������� ������ ���������
	qdInterfaceElementState::state_mode_t stateMode_;
};
