#pragma once

class qdNamedObject;
class qdInterfaceObjectBase;
class qdInterfaceElementState;

#include <memory>
interface ITreeNode;
typedef std::auto_ptr<ITreeNode> ITreeNodePtr;

/// Интерфейс фабрики узлов дерева
struct ITreeNodeFactory
{
	/// Создает узлы для объектов сцены
	virtual ITreeNodePtr createGameObjectNode(qdNamedObject* object) = 0;
	/// Создает узлы для глобальных объектов
	virtual ITreeNodePtr createGlobalGameObjectNode(
															qdNamedObject* object) = 0;
	/// Создает узлы для интерфейсных объектов
	virtual ITreeNodePtr createInterfaceObjectNode(
													qdInterfaceObjectBase* object) = 0;
	/// Создает узлы папки
	virtual ITreeNodePtr createFolderNode(DWORD id) = 0;
	/// Создает узлы состояний для зон на сетке
	virtual ITreeNodePtr createGridZoneStateNode(LPCTSTR name, 
															qdNamedObject* object) = 0;
	/// Создает узлы для состояний интерфейсных объектов
	virtual ITreeNodePtr createInterfaceElementStateModeNode(
								qdInterfaceElementState* object, DWORD modeId) = 0;
};