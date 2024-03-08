#pragma once

// �������� ����� ������� ��� �������� �������� �������
#include "stdafx.h"
#include "qd_game_scene.h"

namespace CondUsuals
{	enum eShowHide{
		SH_NONE, ///< ������ ��������� ����� 
		SH_SHOWHIDE///< \brief ��������� ����� �, ���� ����� �������� ������ 
					///< ��������� �����.
	};
	//! ��������� ����� � ���������
	bool LoadScenes(CCJFlatComboBox& box, CWnd& sceneStatic);

	//! ��������� ������� �� �������� ����� � ���������
	bool LoadAnimObjects(CCJFlatComboBox& wndCombo, const qdGameScene* ptrScene);

	//! ���������� ��������� ���������� � ���������� ��������, ��� ������� - NULL
	const void* GetSelPtr(const CCJFlatComboBox& wndCombo);
}