#pragma once

interface ITreeNode;

/// Ќабор функций, определ€ющих, что содержит узел
class TreeNodeTraits
{
public:
	/// явл€етс€ ли узел узлом сцены
	static bool isSceneNode(ITreeNode const* node);
	//! —одержит ли node объект из ветви объектов, порожденной от qdNamedObject
	static bool isGameNode(ITreeNode const* node);
	//! —одержит ли node объект из ветви объектов, порожденной от qdInterfaceObjectBase
	static bool isInterfaceNode(ITreeNode const* node);
	//! явл€етс€ ли узел одной из папок
	static bool isFolderNode(ITreeNode const* node);
	//! явл€етс€ ли узел каталогом статических состо€ний
	static bool isStaticStateCatalog(ITreeNode const* node);
	//! явл€етс€ ли узел каталогом походочных состо€ний
	static bool isWalkStateCatalog(ITreeNode const* node);
	//! явл€етс€ ли узел узлом триггера
	static bool isTrigger(ITreeNode const* node);
	//! явл€етс€ ли узел узлом каталога триггеров
	static bool isTriggers(ITreeNode const* node);
	//! явл€етс€ ли узел узлом qdGameObjectAnimated, qdGameObjectMoving, qdGameObjectMouse
	static bool isAnimated(ITreeNode const* node);
	//! явл€етс€ ли узел узлом сетки
	static bool isNet(ITreeNode const* node);
	//! явл€етс€ ли узел каталогом видео
	static bool isVideoCatalog(ITreeNode const* node);
	//! явл€етс€ ли узел каталогом иневентори
	static bool isInventoryCatalog(ITreeNode const* node);
	//! явл€етс€ ли узел узлом объекта иневентори
	static bool isInventory(ITreeNode const* node);
	//! явл€етс€ ли узел узлом состо€ни€
	static bool isStateNode(ITreeNode const* node);
	//! явл€етс€ ли узел узлом диспетчера интерфейса
	static bool isInterfaceRoot(ITreeNode const* node);

	//! явл€етс€ ли узел каталогом объектов указанного типа
	static bool isContainerType(ITreeNode const* node, DWORD type);
	/// ќпредел€ет совпадает ли тип узла с type
	static bool isNodeType(ITreeNode const* node, DWORD type);
};
