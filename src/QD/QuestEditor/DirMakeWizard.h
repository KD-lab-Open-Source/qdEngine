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

///	\brief –еализует логику создани€ структуры  папок
///	может быть вклчен в цепочку других визардов
class DirMakeWizard : public MWWizard
{
public:
	DirMakeWizard(void);
	~DirMakeWizard(void);

	DirMakeWizardData* data() const{
		return m_ptrData.get();
	}
	using MWWizard::set_changes;

	//останавливает визард, возвращает истину, 
	//если это удалось сделать
	bool SuspendWizard();
	//запускает по новой. возвращаемое значение не используетс€(пока)
	bool ResumeWizard();

	bool Construct();
private:
	/// ƒанные визарда
	DirMakeWizardDataPtr m_ptrData;
	/// —траничка выбора каталога, в котором будет создана структура каталогов
	SelectFolderMDSPPPtr m_ptrSelFolderPage;
	/// —траничка, на которой указывают количество сцен дл€ создани€
	SceneCountMDSPPPtr	 m_ptrSceneCountPage;
	/// —траничка, отражающа€ прогресс процесса
	ProgressMDSPPPtr	 m_ptrProgresPage;
	/// «аключительна€ страничка
	DoneMDSPPPtr		 m_ptrDonePage;
};
