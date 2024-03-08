#pragma once

/**
	—оздает визард создани€ структуры каталогов и запускает его
 */
class DirMakeWizardShower
{
public:
	DirMakeWizardShower(void);
	~DirMakeWizardShower(void);
	/// —оздает и запускает
	/**
		¬озвращаемое значение смысла не имеет
	 */
	bool show(CWnd* parent);
};
