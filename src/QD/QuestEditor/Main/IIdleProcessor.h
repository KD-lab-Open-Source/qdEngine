#pragma once

/*!
 ��������� ����������� ���, ��� ����� �������� ��� ����������� �� CQuestEditorApp
*/
interface IIdleProcessor
{
	virtual BOOL OnIdle(LONG lCount) = 0;
};