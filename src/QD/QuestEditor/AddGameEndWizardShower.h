#pragma once
interface ITreeNode;
/**
	������� � ���������� ������ ��� �������� "���������� ������"
 */
class AddGameEndWizardShower
{
public:
	AddGameEndWizardShower(void);
	~AddGameEndWizardShower(void);

	/// ������ ��� ������ ������
	bool show(CWnd *parent, class qdGameDispatcher* pdisp, ITreeNode* parentNode);
	/// ������ ��� ������� ������
	bool show(CWnd *parent, class qdGameDispatcher* pdisp, HTREEITEM hParent);
};
