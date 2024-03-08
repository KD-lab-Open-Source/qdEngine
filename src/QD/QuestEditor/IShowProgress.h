/********************************************************************
	created:	2002/11/27
	created:	27:11:2002   17:06
	filename: 	d:\projects\quest\questeditor\ishowprogress.h
	file path:	d:\projects\quest\questeditor
	file base:	ishowprogress
	file ext:	h
	Powered by:	�����
	
	purpose:	��������� ��� ���������� ��������� ��������
*********************************************************************/

#ifndef _ISHOW_PROGRESS_
#define _ISHOW_PROGRESS_
#pragma  once

/// ��������� ��� ���������� ��������� ��������
interface IShowProgress
{
	/// ������� ������� ��������� �������
	virtual void SetPrecents(int prec) = 0;
	/// ����� �� �������� �������(��.: ����� ������� ����)
	virtual bool BreakProcess() = 0;
	/// ������������� ������ ���������
	virtual void SetText(LPCTSTR lpszMsg) = 0;
};

#endif