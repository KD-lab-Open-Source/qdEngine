#pragma once

/**
	создает и показывает визард добавления сейва интерфейсному экрану
 */
class AddInterfSaveWizShow
{
public:
	AddInterfSaveWizShow();
	~AddInterfSaveWizShow();

	bool show(CWnd *parent, class qdInterfaceScreen* pscr);
};
