#pragma once

/**
	������� � �������� ������ �������� ���� ��� ������ ����������
 */
class AddIntrfBackgrndWizardShow
{
public:
	AddIntrfBackgrndWizardShow(void);
	~AddIntrfBackgrndWizardShow(void);
	bool show(CWnd* parent, class qdInterfaceScreen* pscr, UINT idCap = 0);
};
