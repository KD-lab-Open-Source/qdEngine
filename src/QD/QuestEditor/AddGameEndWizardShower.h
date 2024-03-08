#pragma once
interface ITreeNode;
/**
	Создает и показывает визард для создания "последнего экрана"
 */
class AddGameEndWizardShower
{
public:
	AddGameEndWizardShower(void);
	~AddGameEndWizardShower(void);

	/// Версия для нового дерева
	bool show(CWnd *parent, class qdGameDispatcher* pdisp, ITreeNode* parentNode);
	/// Версия для старого дерева
	bool show(CWnd *parent, class qdGameDispatcher* pdisp, HTREEITEM hParent);
};
