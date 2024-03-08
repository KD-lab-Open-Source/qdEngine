#pragma once

#include "qd_fwd.h"
class qdGameObjectAnimated;

/// ���������� �� ���� ���������� ���������� ���������� � �������.
/**
	������ ����������� �� ���� ��������� ���������� � �������� 
	qdGameObjectState::update_cursor_id
 */
class CursorIdUpdater
{
	CursorIdUpdater(void);
	~CursorIdUpdater(void);
protected:
	/// ������������ ������ ���������� ��������
	static void processGlobalObjects(qdGameDispatcher& gameDispatcher);
	/// ������������ ������ ����
	static void processScenes(qdGameDispatcher& gameDispatcher);
	/// ������������ ��� ������� �� �����
	static void processScene(qdGameScene* scene, qdGameDispatcher& gameDispatcher);
	/// ������������ ��� ��������� � �������
	static void processObject(qdGameObjectAnimated& object, 
								qdGameDispatcher& gameDispatcher);
	/// ��������� �� ������ ��������� � ������������ ��
	static void processStates(qdGameObjectStateVector const& states, 
								qdGameDispatcher& gameDispatcher);
public:
	/// ������� �������
	static void run(qdGameDispatcher& gameDispatcher);
};
