/********************************************************************
	created:	2003/04/30
	created:	30:4:2003   15:29
	filename: 	d:\Projects\Quest\QuestEditor\IViewMgr.h
	file path:	d:\Projects\Quest\QuestEditor
	file base:	IViewMgr
	file ext:	h
	Powerd by:	�����
	
	purpose:	���������� ���������� ���� ��������� ��������� � ��������� �����
*********************************************************************/
#pragma once

/// ���������� ���������� ���� ��������� ��������� � ��������� �����
interface IViewMgr
{
	virtual BOOL ShowSceneEditor() = 0;
	virtual BOOL ShowTriggerEditor() = 0;
};