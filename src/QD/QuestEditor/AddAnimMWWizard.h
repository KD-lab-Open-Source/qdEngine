#pragma once
#include "multiwizard/mwwizard.h"
#include <memory>

class MWAddAnimFirst;
class MWConvertWayPage;
class MWAAProgressPage;
class MWSelectExstFilePage;
class MWAADone;
class MWAACreateObjPage;
class MWAACadrDuration;

class add_anim_wiz_data;

typedef std::auto_ptr<MWAddAnimFirst> MWAddAnimFirstPtr;
typedef std::auto_ptr<MWConvertWayPage> MWConvertWayPagePtr;
typedef std::auto_ptr<MWAAProgressPage> MWAAProgressPagePtr;
typedef std::auto_ptr<MWSelectExstFilePage> MWSelectExstFilePagePtr;
typedef std::auto_ptr<MWAACreateObjPage> MWAACreateObjPagePtr;
typedef std::auto_ptr<MWAACadrDuration> MWAACadrDurationPtr;
typedef std::auto_ptr<MWAADone> MWAADonePtr;
typedef std::auto_ptr<add_anim_wiz_data> add_anim_wiz_dataPtr;

/// ������-������ ��� ������ ��������
class AddAnimMWWizard :public MWWizard
{
public:
	AddAnimMWWizard(void);
	~AddAnimMWWizard(void);
	//������������� ������, ���������� ������, 
	//���� ��� ������� �������
	bool SuspendWizard();
	//��������� �� �����. ������������ �������� �� ������������(����)
	bool ResumeWizard();

	/// ������������ ��������� � ��������� �� � �������
	bool Construct(qdGameDispatcher* ptrDisp, qdGameDispatcherBase* ptrGDB);

	add_anim_wiz_data* data() const{
		return m_ptrData.get();
	}
	using MWWizard::set_changes;
private:
	/// �������� ������
	void reset_data();
private://members
	/// ������ ��������
	MWAddAnimFirstPtr m_ptrAAFirst;
	/// \name ����� ������� �������
	//@{
	// �� - add animation
	/// ���������, ��� ������ �������� �������� ��� ��������� ������������
	MWSelectExstFilePagePtr m_ptrAASelExistFiles;
	/// ���������, �� ������� ���������� ������ ������������
	MWConvertWayPagePtr m_ptrAAConvertWay;
	/// ��������� ���������� �������� �������� ������
	MWAAProgressPagePtr m_ptrAAProgress;
	/// ��������, �� ������� ������ ��������� �� ����� �������
	MWAACreateObjPagePtr m_ptrAACrtObj;
	/// ���������, �� ������� �������� ������������ �����
	MWAACadrDurationPtr m_ptrAACadrDur;
	/// ��������� ���������
	MWAADonePtr	m_ptrAADone;
	//@}
	/// ������ ��� �������
	add_anim_wiz_dataPtr m_ptrData;
};

typedef std::auto_ptr<AddAnimMWWizard> AddAnimMWWizardPtr;