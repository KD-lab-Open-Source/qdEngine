#pragma once

/**
	Создает и запускает визард создания интерфейсной кнопки
 */
class AddInterfBtnWizShow
{
public:
	AddInterfBtnWizShow(void);
	~AddInterfBtnWizShow(void);

	bool show(CWnd *parent, class qdInterfaceScreen* pscr);
};
