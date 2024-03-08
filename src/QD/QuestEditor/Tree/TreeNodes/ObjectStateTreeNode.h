#pragma once
#include ".\conditionalobjecttreenode.h"

class qdGameObjectState;
/// ���� ��������� �������������� �������
class ObjectStateTreeNode : public ConditionalObjectTreeNode
{
protected:
public:
	ObjectStateTreeNode(qdNamedObject* object);
	~ObjectStateTreeNode(void);
	bool load(ITreeNodeFactory* factory, TreeLogic& treeLogic, ITreeNode * parent, 
		qdNamedObjectBase const* owner,
		ITreeNode* insertAfter);
	//! ���������� ������������� ������� ���� ��� ����
	DWORD getMenuResId() const;
	void modifyMenu(TreeLogic& treeLogic, CMenu* pm) const;

	bool showProperties(TreeLogic& logic, int iActive);
	bool showConditions(TreeLogic& logic);

	virtual void updateItemImage(TreeLogic& treeLogic);
	virtual bool onStateChange(TreeLogic& logic, DWORD oldState, DWORD newState);

	virtual bool deleteObject(TreeLogic& logic, bool ask, bool statDepends);
	//! \brief ����������, ����� ����� ������� �������, ������������ � ������,
	//! ��� ��� ��������(��������, ����� ��������� ������������� ������. 
	//! ��� �������, ����� ��������� �����)
	virtual bool deleteAsSubObject(TreeLogic& logic);
	//! ����� �� ������ ���� �� ���, ��� ������� �����
	virtual DROPEFFECT canDrop(TreeLogic& logic, ITreeNode* overNode);
	virtual DROPEFFECT onDropData(TreeLogic& logic, 
						ITreeNode* dropTarget, 
						DROPEFFECT dropEffect);
	//! ���������� true, ���� ������� ������������, false � ��������� ������
	virtual bool rename(TreeLogic& logic, LPCTSTR newName);
	//! ����� �� ������� ��� ����
	virtual bool canRename(TreeLogic& logic);

	void* operator new(size_t size);
	void operator delete(void* p);

	void* operator new(size_t size, LPCTSTR file, unsigned line);
	void operator delete(void* p, LPCTSTR file, unsigned line);

	//! ����� � ���������� ����� ��������� ������, �������� ��� �������� ���������
	void resetDefaultStateIcon(TreeLogic& logic);
protected:
	/// �������(����� �����) ��� �������� ��������� �����
	bool onSelectInSceneView(TreeLogic& logic, bool bSelect);
	/// �������(����� �����) ��� �������� ��������� ���������
	bool onSelectInTriggerView(TreeLogic& logic, bool bSelect);
	/// ���������� �������� ��� ���� ���������
	static int selectPicture(qdGameObjectState const& state, qdNamedObject const& owner);
	/// ���������� �������� ��� ���� ��������� ����
	static int selectMouseStatePicture(qdGameObjectMouse const& mouse, 
										qdNamedObject const& state);

	//! ������������, ���� �������� ��������� ����
	bool deleteMouseState(TreeLogic& logic);
	//! ������������, ���� �������� ��������� ������������� ������
	bool deleteAnimatedObjectState(TreeLogic& logic, bool ask);
	virtual bool preDelete(TreeLogic& logic, bool ask, bool statDepends);
	/// ������ �������� ����� �������� ���������� ��������� 
	virtual bool postDeleteNotMouseState(TreeLogic& logic);

	/// ����� �� ������� ���������� ���������
	DROPEFFECT cdGlobalState(TreeLogic& logic, ITreeNode* overNode);
	/// ����� �� ������� ������������ ���������
	DROPEFFECT cdOwnState(TreeLogic& logic, ITreeNode* overNode);

	/// ����� �� ������� ��������� ��� ������������� ������
	bool canDropUnderAnim(TreeLogic& logic, ITreeNode* node);
	/// ����� �� ������� ������������ ��������� ��� ��������
	DROPEFFECT cdOwnStateUnderMoving(TreeLogic& logic,
//	DROPEFFECT ObjectStateTreeNode::cdOwnStateUnderMoving(TreeLogic& logic,
															ITreeNode* overNode,
															ITreeNode* animatedNode);
	/// ����� �� ������� ��������� ��� ��������
	bool canDropUnderMoving(TreeLogic& logic, ITreeNode* movingNode);

	/// ������� ������������ ��������� ��� ����
	DROPEFFECT oddOwnStateMouseObj(TreeLogic& logic, 
							ITreeNode* animated,
							ITreeNode* dropTarget, 
							DROPEFFECT dropEffect);
	/// ������� ������������ ��������� ��� ������������� ������
	DROPEFFECT oddOwnStateAnimObj(TreeLogic& logic, 
							ITreeNode* animated,
							ITreeNode* dropTarget, 
							DROPEFFECT dropEffect);
	/// ������� ������������ ��������� ��� ��������
	DROPEFFECT oddOwnStateMovingObj(TreeLogic& logic, 
									ITreeNode* animated,
									ITreeNode* dropTarget, 
									DROPEFFECT dropEffect);
	/// ���������� true, ���� newName ���������
	bool nameTest(TreeLogic& logic, LPCTSTR newName);
};
