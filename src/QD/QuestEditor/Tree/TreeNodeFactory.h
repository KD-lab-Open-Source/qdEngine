#pragma once

#include "ITreeNodeFactory.h"

/// Имплементация ITreeNodeFactory
class TreeNodeFactory : public ITreeNodeFactory
{
public:
	TreeNodeFactory(void);
	~TreeNodeFactory(void);
	std::auto_ptr<ITreeNode> createGameObjectNode(qdNamedObject* object);
	std::auto_ptr<ITreeNode> createGlobalGameObjectNode(qdNamedObject* object);
	std::auto_ptr<ITreeNode> createInterfaceObjectNode(qdInterfaceObjectBase* object);
	std::auto_ptr<ITreeNode> createFolderNode(DWORD id);
	std::auto_ptr<ITreeNode> createGridZoneStateNode(LPCTSTR name,qdNamedObject* object);
	std::auto_ptr<ITreeNode> createInterfaceElementStateModeNode(
										qdInterfaceElementState* object, DWORD modeId);
};
