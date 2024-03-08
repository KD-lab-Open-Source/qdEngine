/********************************************************************
	created:	2002/11/27
	created:	27:11:2002   17:06
	filename: 	d:\projects\quest\questeditor\ishowprogress.h
	file path:	d:\projects\quest\questeditor
	file base:	ishowprogress
	file ext:	h
	Powered by:	Илюха
	
	purpose:	Интерфейс для отброжения прогресса процесса
*********************************************************************/

#ifndef _ISHOW_PROGRESS_
#define _ISHOW_PROGRESS_
#pragma  once

/// Интерфейс для отброжения прогресса процесса
interface IShowProgress
{
	/// говорит сколько процентво сделано
	virtual void SetPrecents(int prec) = 0;
	/// нужно ли прервать процесс(пр.: после нажатия стоп)
	virtual bool BreakProcess() = 0;
	/// устанавливает строку сообщения
	virtual void SetText(LPCTSTR lpszMsg) = 0;
};

#endif