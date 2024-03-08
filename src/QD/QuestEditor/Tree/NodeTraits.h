#pragma once

interface ITreeNode;

/// ����� �������, ������������, ��� �������� ����
class TreeNodeTraits
{
public:
	/// �������� �� ���� ����� �����
	static bool isSceneNode(ITreeNode const* node);
	//! �������� �� node ������ �� ����� ��������, ����������� �� qdNamedObject
	static bool isGameNode(ITreeNode const* node);
	//! �������� �� node ������ �� ����� ��������, ����������� �� qdInterfaceObjectBase
	static bool isInterfaceNode(ITreeNode const* node);
	//! �������� �� ���� ����� �� �����
	static bool isFolderNode(ITreeNode const* node);
	//! �������� �� ���� ��������� ����������� ���������
	static bool isStaticStateCatalog(ITreeNode const* node);
	//! �������� �� ���� ��������� ���������� ���������
	static bool isWalkStateCatalog(ITreeNode const* node);
	//! �������� �� ���� ����� ��������
	static bool isTrigger(ITreeNode const* node);
	//! �������� �� ���� ����� �������� ���������
	static bool isTriggers(ITreeNode const* node);
	//! �������� �� ���� ����� qdGameObjectAnimated, qdGameObjectMoving, qdGameObjectMouse
	static bool isAnimated(ITreeNode const* node);
	//! �������� �� ���� ����� �����
	static bool isNet(ITreeNode const* node);
	//! �������� �� ���� ��������� �����
	static bool isVideoCatalog(ITreeNode const* node);
	//! �������� �� ���� ��������� ����������
	static bool isInventoryCatalog(ITreeNode const* node);
	//! �������� �� ���� ����� ������� ����������
	static bool isInventory(ITreeNode const* node);
	//! �������� �� ���� ����� ���������
	static bool isStateNode(ITreeNode const* node);
	//! �������� �� ���� ����� ���������� ����������
	static bool isInterfaceRoot(ITreeNode const* node);

	//! �������� �� ���� ��������� �������� ���������� ����
	static bool isContainerType(ITreeNode const* node, DWORD type);
	/// ���������� ��������� �� ��� ���� � type
	static bool isNodeType(ITreeNode const* node, DWORD type);
};
