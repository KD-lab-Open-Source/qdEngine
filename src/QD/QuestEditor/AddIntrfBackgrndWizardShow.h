#pragma once

/**
	Создает и запускат визард создания фона для экрана интерфейса
 */
class AddIntrfBackgrndWizardShow
{
public:
	AddIntrfBackgrndWizardShow(void);
	~AddIntrfBackgrndWizardShow(void);
	bool show(CWnd* parent, class qdInterfaceScreen* pscr, UINT idCap = 0);
};
