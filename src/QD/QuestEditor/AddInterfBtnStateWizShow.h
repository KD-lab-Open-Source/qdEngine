#pragma once

/**
	Создает и показывает визард добавления состояния кнопке
 */
class AddInterfBtnStateWizShow
{
public:
	AddInterfBtnStateWizShow(void);
	~AddInterfBtnStateWizShow(void);
	bool show(CWnd *parent, class qdInterfaceButton* pbtn);
};
