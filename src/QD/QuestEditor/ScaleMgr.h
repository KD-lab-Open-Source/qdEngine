#pragma once
#include "scaleinterfaces.h"
#include <memory>

class CScaleBar;

/**
	��������� �������� � ���������� ��� ���������������
 */
class ScaleMgr : public IScaleMgr
{
public:
	/// ��������� ����, ���� ����� ��������������
	void SetScalable(IScalable* pscalable);
	/// ��������� ���������� � ��������
	void UpdateScaleInfo();

	ScaleMgr(void);
	~ScaleMgr(void);

	/// ������� ������
	bool Create(CFrameWnd* pParent, DWORD barid);
	/// ��������
	void Show() const;
	/// ������
	void Hide() const;
	/// ����� ��?
	bool IsVisible() const;

	/// ���������� ��������� �� ���� �������
	CWnd* GetView();
private:
	/// ��������� �� ������
	std::auto_ptr<CScaleBar> m_ptrScaleBar;
	/// ����, � ������� ������ �����
	CFrameWnd* m_pFrameWnd;
};
