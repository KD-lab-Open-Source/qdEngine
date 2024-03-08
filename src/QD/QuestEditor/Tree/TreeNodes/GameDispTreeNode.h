#pragma once

#include "NamedObjectTreeNode.h"

/// ����� ����, ���������������� ���������� �������. ��� �������� ����
class GameDispTreeNode : public NamedObjectTreeNode
{
public:
	GameDispTreeNode(qdNamedObject* object);
	~GameDispTreeNode(void);

	virtual bool load(ITreeNodeFactory* factory,
						TreeLogic& treeLogic, 
						ITreeNode * parent, 
						qdNamedObjectBase const* owner,
						ITreeNode* insertAfter);
	virtual bool reload(ITreeNodeFactory* factory,
						TreeLogic& treeLogic, 
						ITreeNode const* parent, 
						qdNamedObjectBase const* owner);
	//! ���������� ������������� ������� ���� ��� ����
	DWORD getMenuResId() const;
	virtual bool onStateChange(TreeLogic& logic, DWORD oldState, DWORD newState);
	bool showProperties(TreeLogic& logic, int activePage);
	bool canRename(TreeLogic& logic);
protected:
	/// ��������� � ������ �����
	bool loadScenes(ITreeNodeFactory* factory, TreeLogic& logic);
	/// ��������� � ������ ���������� �������
	bool loadGlobalGameObjects(ITreeNodeFactory* factory, TreeLogic& logic);
	/// ��������� � ������ ��������
	bool loadTriggers(ITreeNodeFactory* factory, TreeLogic& logic);
	/// ��������� � ������ ����
	bool loadMouse(ITreeNodeFactory* factory, TreeLogic& logic);
	/// ��������� � ������ ���������
	bool loadInventory(ITreeNodeFactory* factory, TreeLogic& logic);
	/// ��������� � ������ ��������
	bool loadCounters(ITreeNodeFactory* factory, TreeLogic& logic);
	/// ��������� � ������ �����
	bool loadFonts(ITreeNodeFactory* factory, TreeLogic& logic);
	/// ��������� � ������ �����
	bool loadVideo(ITreeNodeFactory* factory, TreeLogic& logic);
	/// ��������� � ������ ��������
	bool loadMinigames(ITreeNodeFactory* factory, TreeLogic& logic);

	/// ��������� � ������ ���������
	bool loadInterface(ITreeNodeFactory* factory, TreeLogic& logic);
	/// ��������� � ������ ��������� ������
	bool loadGameEnds(ITreeNodeFactory* factory, TreeLogic& logic, ITreeNode * parent);
	/// \brief ������������� (��� ���� ������� ���������, 
	/// ��� ��������� - �����������) � ������ �����
	bool reloadScenes(ITreeNodeFactory* factory, TreeLogic& logic);
	/// \brief ������������� (��� ���� ������� ���������, 
	/// ��� ��������� - �����������) � ������ ���������� �������
	bool reloadGlobalGameObjects(ITreeNodeFactory* factory, TreeLogic& logic);
	/// \brief ������������� (��� ���� ������� ���������, 
	/// ��� ��������� - �����������) � ������ ��������
	bool reloadTriggers(ITreeNodeFactory* factory, TreeLogic& logic);
	/// \brief ������������� (��� ���� ������� ���������, 
	/// ��� ��������� - �����������) � ������ ����
	bool reloadMouse(ITreeNodeFactory* factory, TreeLogic& logic);
	/// \brief ������������� (��� ���� ������� ���������, 
	/// ��� ��������� - �����������) � ������ ���������
	bool reloadInventory(ITreeNodeFactory* factory, TreeLogic& logic);
	/// \brief ������������� (��� ���� ������� ���������, 
	/// ��� ��������� - �����������) � ������ ��������
	bool reloadCounters(ITreeNodeFactory* factory, TreeLogic& logic);
	/// \brief ������������� (��� ���� ������� ���������, 
	/// ��� ��������� - �����������) � ������ �����
	bool reloadFonts(ITreeNodeFactory* factory, TreeLogic& logic);
	/// \brief ������������� (��� ���� ������� ���������, 
	/// ��� ��������� - �����������) � ������ �����
	bool reloadVideo(ITreeNodeFactory* factory, TreeLogic& logic);
	/// \brief ������������� (��� ���� ������� ���������, 
	/// ��� ��������� - �����������) � ������ ��������
	bool reloadMinigames(ITreeNodeFactory* factory, TreeLogic& logic);
	/// \brief ������������� (��� ���� ������� ���������, 
	/// ��� ��������� - �����������) � ������ ������������ �������
	bool reloadInterface(ITreeNodeFactory* factory, TreeLogic& logic);
	/// \brief ������������� (��� ���� ������� ���������, 
	/// ��� ��������� - �����������) � ������ �������� ������
	bool reloadGameEnds(ITreeNodeFactory* factory, TreeLogic& logic, ITreeNode * parent);
};
