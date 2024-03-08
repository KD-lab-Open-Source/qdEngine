#pragma once

/*!
 Интерфейс реализуется тем, кто хочет получать это уведомление из CQuestEditorApp
*/
interface IIdleProcessor
{
	virtual BOOL OnIdle(LONG lCount) = 0;
};