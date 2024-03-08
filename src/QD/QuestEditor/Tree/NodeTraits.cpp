#include "stdafx.h"
#include ".\NodeTraits.h"
#include "ITreeNode.h"
#include "ptree_consts.h"
#include "Main/ObjectTraits.h"
#include "qd_named_object.h"

bool TreeNodeTraits::isSceneNode(ITreeNode const* node){
	if (!isGameNode(node))
		return false;
	return ObjectTraits::is_obj_scene(node->getContent().gameObject);
}

//!�������� �� node ������ �� ����� ��������, ����������� �� qdNamedObject
bool TreeNodeTraits::isGameNode(ITreeNode const* node){
	return (node->getContentClassificator() == CC_NAMED_OBJECT);
}
//!�������� �� node ������ �� ����� ��������, ����������� �� qdInterfaceObjectBase
bool TreeNodeTraits::isInterfaceNode(ITreeNode const* node){
	return (node->getContentClassificator() == CC_INTERFACE_OBJECT);
}

//! �������� �� ������ ����� �� �����
bool TreeNodeTraits::isFolderNode(ITreeNode const* node){
	return (node->getContentClassificator() == CC_FOLDER);
}

//! �������� �� ������ ��������� ����������� ���������
bool TreeNodeTraits::isStaticStateCatalog(ITreeNode const* node)
{
	if (!isFolderNode(node))
		return false;
	return (node->getContent().folderId == QE_STATE_CATALOG);
}

//! �������� �� ������ ��������� ���������� ���������
bool TreeNodeTraits::isWalkStateCatalog(ITreeNode const* node)
{
	if (!isFolderNode(node))
		return false;
	return (node->getContent().folderId == QE_MOVEMENT_STATE_CATALOG);
}

bool TreeNodeTraits::isTrigger(ITreeNode const* node)
{
	if (!isGameNode(node))
		return false;
	return ObjectTraits::is_trigger(node->getContent().gameObject);
}

//! �������� �� ���� ����� �������� ���������
bool TreeNodeTraits::isTriggers(ITreeNode const* node)
{
	if (!isFolderNode(node))
		return false;
	return (node->getContent().folderId == QD_NAMED_OBJECT_TRIGGER_CHAIN);
}

//! �������� �� ���� ����� qdGameObjectAnimated ��� qdGameObjectMoving
bool TreeNodeTraits::isAnimated(ITreeNode const* node)
{
	if (!isGameNode(node))
		return false;
	return (ObjectTraits::is_obj_moving(node->getContent().gameObject)
		||ObjectTraits::is_obj_animated(node->getContent().gameObject)
		||ObjectTraits::is_obj_mouse(node->getContent().gameObject));
}

bool TreeNodeTraits::isNet(ITreeNode const* node)
{
	if (!isFolderNode(node))
		return false;
	return (node->getContent().folderId == QD_NAMED_OBJECT_GRID_ZONE);
}

bool TreeNodeTraits::isVideoCatalog(ITreeNode const* node)
{
	if (!isFolderNode(node))
		return false;
	return (node->getContent().folderId == QD_NAMED_OBJECT_VIDEO);
}

bool TreeNodeTraits::isInventoryCatalog(ITreeNode const* node)
{
	if (!isFolderNode(node))
		return false;
	return (node->getContent().folderId == QD_NAMED_OBJECT_INVENTORY);
}

bool TreeNodeTraits::isInventory(ITreeNode const* node)
{
	if (!isGameNode(node))
		return false;
	return ObjectTraits::is_obj_inventory(node->getContent().gameObject);
}

//! �������� �� ���� ����� ���������
bool TreeNodeTraits::isStateNode(ITreeNode const* node)
{
	if (!isGameNode(node))
		return false;
	return ObjectTraits::is_obj_state(node->getContent().gameObject);
}

bool TreeNodeTraits::isInterfaceRoot(ITreeNode const* node)
{
	if (!isInterfaceNode(node))
		return false;
	return ObjectTraits::is_obj_interface_disp(node->getContent().interfaceObject);
}

//! �������� �� ���� ��������� �������� ���������� ����
bool TreeNodeTraits::isContainerType(ITreeNode const* node, DWORD type)
{
	if (!isFolderNode(node))
		return false;
	return (node->getContent().folderId == type);
}

bool TreeNodeTraits::isNodeType(ITreeNode const* node, DWORD type)
{
	if (!isGameNode(node))
		return false;
	return (node->getContent().gameObject->named_object_type() == type);
}
