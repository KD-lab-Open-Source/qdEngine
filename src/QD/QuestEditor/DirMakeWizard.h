#pragma once
#include "multiwizard/mwwizard.h"
#include <memory>

class DirMakeWizardData;
class SelectFolderMDSPP;
class SceneCountMDSPP;
class ProgressMDSPP;
class DoneMDSPP;

typedef std::auto_ptr<DirMakeWizardData> DirMakeWizardDataPtr;
typedef std::auto_ptr<SelectFolderMDSPP> SelectFolderMDSPPPtr;
typedef std::auto_ptr<SceneCountMDSPP> SceneCountMDSPPPtr;
typedef std::auto_ptr<ProgressMDSPP> ProgressMDSPPPtr;
typedef std::auto_ptr<DoneMDSPP> DoneMDSPPPtr;

///	\brief ��������� ������ �������� ���������  �����
///	����� ���� ������ � ������� ������ ��������
class DirMakeWizard : public MWWizard
{
public:
	DirMakeWizard(void);
	~DirMakeWizard(void);

	DirMakeWizardData* data() const{
		return m_ptrData.get();
	}
	using MWWizard::set_changes;

	//������������� ������, ���������� ������, 
	//���� ��� ������� �������
	bool SuspendWizard();
	//��������� �� �����. ������������ �������� �� ������������(����)
	bool ResumeWizard();

	bool Construct();
private:
	/// ������ �������
	DirMakeWizardDataPtr m_ptrData;
	/// ��������� ������ ��������, � ������� ����� ������� ��������� ���������
	SelectFolderMDSPPPtr m_ptrSelFolderPage;
	/// ���������, �� ������� ��������� ���������� ���� ��� ��������
	SceneCountMDSPPPtr	 m_ptrSceneCountPage;
	/// ���������, ���������� �������� ��������
	ProgressMDSPPPtr	 m_ptrProgresPage;
	/// �������������� ���������
	DoneMDSPPPtr		 m_ptrDonePage;
};
