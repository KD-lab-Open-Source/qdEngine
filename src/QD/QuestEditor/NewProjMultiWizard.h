// NewProjMultiWizard.h: interface for the NewProjMultiWizard class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NEWPROJMULTIWIZARD_H__7C33989F_0953_4BBA_B479_ECD2BDC35AD6__INCLUDED_)
#define AFX_NEWPROJMULTIWIZARD_H__7C33989F_0953_4BBA_B479_ECD2BDC35AD6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <memory>

class MultiWizard;
class MWBasePage;
class MWDataBase;
class WizardBasePage;
class MWWizard;

typedef std::auto_ptr<MWBasePage> MWBasePagePtr;
typedef std::auto_ptr<MWDataBase> MWDataBasePtr;
typedef std::auto_ptr<WizardBasePage> WizardBasePagePtr;
typedef std::auto_ptr<MWWizard> MWWizardPtr;

/// \brief  ласс скрывает в себе создание мультивизарда и прив€зки к нему
/// визарда глобальных настроек и создани€ анимации
class NewProjMultiWizard  
{
public:
	NewProjMultiWizard();
	virtual ~NewProjMultiWizard();

	bool show(CWnd * parent, class qdGameDispatcher* ptrDisp, UINT idCap = 0);
private:

	/// ƒобал€ет визард глобальных настроек
	MWWizardPtr AddGameGlobalSettingWizard();
	/// ƒобал€ет визард добавлени€ анимации
	MWWizardPtr AddAnimationWizard();

private:
	/// «апускаемый мультивизард
	std::auto_ptr<MultiWizard> m_ptrMultiWizard;
	/// ’ранит указатель на единственную страничку визарда глобальных настроек
	WizardBasePagePtr m_ptrGlobalGameSetting;

	qdGameDispatcher* m_ptrGDisp;
};

#endif //!defined(AFX_NEWPROJMULTIWIZARD_H__7C33989F_0953_4BBA_B479_ECD2BDC35AD6__INCLUDED_)
