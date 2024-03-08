#pragma once
#include ".\conditionalobjecttreenode.h"

/// ����� ����, ���������������� �����
class GameSceneTreeNode : public ConditionalObjectTreeNode
{
public:
	GameSceneTreeNode(qdNamedObject* object);
	~GameSceneTreeNode(void);

	bool load(ITreeNodeFactory* factory, TreeLogic& treeLogic, ITreeNode * parent, 
		qdNamedObjectBase const* owner,
		ITreeNode* insertAfter);
	//! ��������� ����������� �����
	virtual bool reload(ITreeNodeFactory* factory, 
						TreeLogic& logic,
						ITreeNode const* parent, 
						qdNamedObjectBase const* owner);
	//! ���������� ������������� ������� ���� ��� ����
	DWORD getMenuResId() const;
	bool showProperties(TreeLogic& logic, int iActive);
	virtual bool onStateChange(TreeLogic& logic, DWORD oldState, DWORD newState);
	virtual bool deleteObject(TreeLogic& logic, bool ask, bool statDepends);

	virtual bool rename(TreeLogic& logic, LPCTSTR newName);
protected:
/*
	class InsertObject;
	class InsertGameObject;
*/
	void modifyMenu(TreeLogic& treeLogic, CMenu* pm) const;

	/// ��������� � ������ ���������� �����
	void loadContent(ITreeNodeFactory* factory, TreeLogic& treeLogic);
	/// ��������� ����
	bool loadZones(ITreeNodeFactory* factory, TreeLogic& treeLogic);
	/// ��������� ����������� �����
	bool loadMusicTracks(ITreeNodeFactory* factory, TreeLogic& treeLogic);
	/// ��������� ������ ��������
	bool loadGameObjects(ITreeNodeFactory* factory, TreeLogic& treeLogic);

	/// ������������� ������ �����-���� �������� � ������
	template<typename Range>
	bool reloadSimpleCatalog(ITreeNodeFactory* factory, 
							TreeLogic& logic,
							ITreeNode * folder,
							ITreeNode* insertAfter,
							Range r);

	/// ������������� ���������� ����� � ������
	bool reloadContent(ITreeNodeFactory* factroy, TreeLogic& logic);
	/// ������������� ������ ���
	bool reloadZones(ITreeNodeFactory* factory, TreeLogic& logic);
	/// ������������� ������ ����������� ������
	bool reloadMusicTracks(ITreeNodeFactory* factory, TreeLogic& logic);
	/// ������������� ������ ������� ��������
	bool reloadGameObjects(ITreeNodeFactory* factory, TreeLogic& logic);

	/// \brief ����� ������� � ����� ��������� ��� �������� ��������� �����
	bool onSelectInSceneView(TreeLogic& logic, bool bSelect);
	/// \brief ����� ������� � ����� ��������� ��� �������� ��������� ���������
	bool onSelectInTriggerView(TreeLogic& logic, bool bSelect);

	/// ������� ����
	void deleteZones(TreeLogic& logic);
	/// ������� ������� �������
	void deleteObjects(TreeLogic& logic);
	/// ������� ����������� �����
	void deleteMusicTracks(TreeLogic& logic);
	/// ������� ������ �����
	void deleteSubObject(TreeLogic& logic, qdNamedObject const* object);

	/// ���������� true, ���� newName ���������
	bool nameTest(TreeLogic& logic, LPCTSTR newName);

	/// ��������� ��������� ��������� ��� ���� �������� �����
	void updateTriggerElementTitlesForSceneObjects(qdGameDispatcher* gameDisp);
};
