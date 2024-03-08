#pragma once

class qdNamedObject;
class qdInterfaceObjectBase;
class qdInterfaceElementState;

#include <memory>
interface ITreeNode;
typedef std::auto_ptr<ITreeNode> ITreeNodePtr;

/// ��������� ������� ����� ������
struct ITreeNodeFactory
{
	/// ������� ���� ��� �������� �����
	virtual ITreeNodePtr createGameObjectNode(qdNamedObject* object) = 0;
	/// ������� ���� ��� ���������� ��������
	virtual ITreeNodePtr createGlobalGameObjectNode(
															qdNamedObject* object) = 0;
	/// ������� ���� ��� ������������ ��������
	virtual ITreeNodePtr createInterfaceObjectNode(
													qdInterfaceObjectBase* object) = 0;
	/// ������� ���� �����
	virtual ITreeNodePtr createFolderNode(DWORD id) = 0;
	/// ������� ���� ��������� ��� ��� �� �����
	virtual ITreeNodePtr createGridZoneStateNode(LPCTSTR name, 
															qdNamedObject* object) = 0;
	/// ������� ���� ��� ��������� ������������ ��������
	virtual ITreeNodePtr createInterfaceElementStateModeNode(
								qdInterfaceElementState* object, DWORD modeId) = 0;
};