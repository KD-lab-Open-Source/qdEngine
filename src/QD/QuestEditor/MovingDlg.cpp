		// MovingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "questeditor.h"

#include "gdi_GR_dispatcher.h"
#include "qd_animation_set_preview.h"
#include "qd_game_dispatcher.h"

#include "directionnamebyangle.h"

#include "MovingDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// MovingDlg dialog

void AdjustListWidth(CCJFlatComboBox* pCombo);

const int CAM_PITCH_MIN = 1;
const int CAM_PITCH_MAX = 70;
const int CAM_FOCUS_MIN = 10;
const int CAM_FOCUS_MAX = 10000;
const int CAM_PERS_HEIGHT_MIN = 1;
const int CAM_PERS_HEIGHT_MAX = 10000;
const int CAM_GRID_SIZE_MIN = 1;
const int CAM_GRID_SIZE_MAX = 1000;

MovingDlg::MovingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(MovingDlg::IDD, pParent)
	, m_brGridColor(0x00ffffff)
	, m_brBackColor((COLORREF)0x0)
	, m_fStartAngle(0.f)
{
	//{{AFX_DATA_INIT(MovingDlg)
	m_dirs = 0;
	m_bRunAnim = FALSE;
	m_iCamPitch = 0;
	m_iCamFocus = 0;
	m_iGridSize = 0;
	m_iPersHeight = 300;
	m_bFlipVert = FALSE;
	m_bLoop = FALSE;
	m_bFlipHorz = FALSE;
	//}}AFX_DATA_INIT
	m_pbShowAnimation = NULL;
	m_bInitDialog = true;
	m_ptrCurrAnimInfo = NULL;
}

bool MovingDlg::set_data(qdGameDispatcher* ptrDips, qdAnimationSet* ptrObj)
{
	m_ptrGameDisp = ptrDips;
	m_ptrObj = ptrObj;
	m_ptrGameDispBase = (qdGameDispatcherBase*)ptrObj->owner();
	m_BackAnimSet = *ptrObj;
	return true;
}

void MovingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(MovingDlg)
	DDX_Control(pDX, IDC_LIST, m_list);
	DDX_Control(pDX, IDC_COMBO_ANIMATION, animations_);
	DDX_CBIndex(pDX, IDC_COMBO_ANIMATION, animation_);
	DDX_Text(pDX, IDC_EDIT_DIR, m_dirs);
	DDX_Check(pDX, IDC_CHECK_SHOW, m_bRunAnim);
	DDX_Text(pDX, IDC_EDIT_CAM_PITCH, m_iCamPitch);
	DDX_Text(pDX, IDC_EDIT_CAM_FOCUS, m_iCamFocus);
	DDX_Text(pDX, IDC_EDIT_GRID_SIZE, m_iGridSize);
	DDX_Text(pDX, IDC_EDIT_PERS_HEIGHT, m_iPersHeight);
	DDX_Check(pDX, IDC_CHECK_FLIP_VERT, m_bFlipVert);
	DDX_Check(pDX, IDC_CHECK_LOOP, m_bLoop);
	DDX_Check(pDX, IDC_CHECK_FLIP_HORZ, m_bFlipHorz);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_ANIM_PHASE, m_wndAnimPhase);
	DDX_Text(pDX, IDC_EDIT_START_ANGLE, m_fStartAngle);
	DDV_MinMaxFloat(pDX, m_fStartAngle, 0.f, 360.f);
}


BEGIN_MESSAGE_MAP(MovingDlg, CDialog)
//{{AFX_MSG_MAP(MovingDlg)
ON_BN_CLICKED(IDC_DIR_SET, OnDirSet)
	ON_BN_CLICKED(IDC_CHECK_SHOW, OnCheckShow)
	ON_EN_CHANGE(IDC_EDIT_PERS_HEIGHT, OnChangeEditPersHeight)
	ON_EN_CHANGE(IDC_EDIT_GRID_SIZE, OnChangeEditGridSize)
	ON_EN_CHANGE(IDC_EDIT_CAM_PITCH, OnChangeEditCamPitch)
	ON_EN_CHANGE(IDC_EDIT_CAM_FOCUS, OnChangeEditCamFocus)
	ON_BN_CLICKED(IDC_BTN_BACK_COLOR, OnBtnBackColor)
	ON_BN_CLICKED(IDC_BTN_COLOR, OnBtnColor)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_CHECK_FLIP_VERT, OnCheckFlipVert)
	ON_BN_CLICKED(IDC_CHECK_LOOP, OnCheckLoop)
	ON_BN_CLICKED(IDC_CHECK_FLIP_HORZ, OnCheckFlipHorz)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST, OnDblclkList)
	ON_NOTIFY(NM_CLICK, IDC_LIST, OnClickList)
	//}}AFX_MSG_MAP
	ON_NOTIFY(NM_LCE_SETUP_CTRL, IDC_LIST, OnSetupCtrlList)
	ON_NOTIFY(NM_LCE_ENDEDIT, IDC_LIST, OnEndEditList)
	ON_WM_PAINT()
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_BTN_TO_BEG, OnBnClickedBtnToBeg)
	ON_BN_CLICKED(IDC_BTN_PREV, OnBnClickedBtnPrev)
	ON_BN_CLICKED(IDC_BTN_NEXT, OnBnClickedBtnNext)
	ON_BN_CLICKED(IDC_BTN_TO_END, OnBnClickedBtnToEnd)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_START_ANGLE, OnDeltaposSpinStartAngle)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// MovingDlg message handlers

void MovingDlg::OnDirSet() 
{
	if(!UpdateData())
		return;

	m_ptrObj->resize(m_dirs);
	for(int i = 0; i < m_ptrObj->size();++i)
		m_ptrObj->get_animation_info(i)->set_flag(QD_ANIMATION_FLAG_LOOP);

	m_ptrASP->start();
	LoadMovings();
	AdjustColumnsWidth();
}

BOOL MovingDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_wndAnimPhase.SetRange(0, 0);
	InsertColumns();
	//загружаем список анимаций походок и остановок
	LoadMovings();
	MakeAnimationList();

	AdjustColumnsWidth();

	animations_.ResetContent();
	
	CStringSet_t::iterator _itr = m_ptrAnimList->begin(), _end = m_ptrAnimList->end();
	for(; _itr != _end; ++_itr)
		animations_.AddString(*_itr);
	CString strNoThing((LPTSTR)IDS_NOTHING);
	animations_.InsertString(0, strNoThing);
	AdjustListWidth(&animations_);

	if(!CreateXGraph())
		return FALSE;
	if(!CreateAnimPreview())
		return FALSE;

	AnimPreview2Wnd();

	AdjustSpints();
	
	m_fStartAngle = m_ptrObj->start_angle();
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_START_ANGLE))->SetRange(1, 100);

	UpdateData(FALSE);

	LoadBitmaps();
	AccessToAnimCtrls();

	m_bInitDialog = false;	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

bool MovingDlg::InsertColumns()
{
	CString str;
	str.LoadString(IDS_CLMN_DIR_NAME);
	m_list.InsertColumn(0,str, LVCFMT_LEFT, 80);

	str.LoadString(IDS_CLMN_MOVING_NAME);
	m_list.InsertColumn(1,str, LVCFMT_LEFT, 80, 1);
	m_list.SetEditCtrlType(1, ELC_ITEM_EDIT_CTRL_COMBO);

	str.LoadString(IDS_CLMN_STOPS);
	m_list.InsertColumn(2,str, LVCFMT_LEFT, 80, 2);
	m_list.SetEditCtrlType(2, ELC_ITEM_EDIT_CTRL_COMBO);

	str.LoadString(IDS_CLMN_STARTS);
	m_list.InsertColumn(3,str, LVCFMT_LEFT, 80, 3);
	m_list.SetEditCtrlType(3, ELC_ITEM_EDIT_CTRL_COMBO);
	
	str.LoadString(IDS_CLMN_ENDS);
	m_list.InsertColumn(4,str, LVCFMT_LEFT, 80, 4);
	m_list.SetEditCtrlType(4, ELC_ITEM_EDIT_CTRL_COMBO);

	str.LoadString(IDS_CLMN_SPEED_NAME);
	m_list.InsertColumn(5,str, LVCFMT_LEFT, 80, 5);
	m_list.SetEditCtrlType(5, ELC_ITEM_EDIT_CTRL_EDIT);

	str.LoadString(IDS_CLMN_ANIM_SPEED_NAME);
	m_list.InsertColumn(6,str, LVCFMT_LEFT, 80, 5);
	m_list.SetEditCtrlType(6, ELC_ITEM_EDIT_CTRL_EDIT);

	str.LoadString(IDS_CLMN_SOUND_FREQUENCY_NAME);
	m_list.InsertColumn(7,str, LVCFMT_LEFT, 80, 5);
	m_list.SetEditCtrlType(7, ELC_ITEM_EDIT_CTRL_EDIT);

	return true;
}

//создаем единый список имен анимаций без повторений
bool MovingDlg::MakeAnimationList()
{
	m_ptrAnimList = 
		std::auto_ptr<CStringSet_t>(merge_lists(m_ptrGameDisp,m_ptrGameDispBase, 
				qdGameDispatcherBase::animation_list));
	m_ptrAnimList->insert("");
	return bool(m_ptrAnimList.get());
}

bool MovingDlg::InsertAnimInfo(int iDir, qdAnimationInfo* walk_info, qdAnimationInfo* stand_info, qdAnimationInfo* start_info, qdAnimationInfo* stop_info)
{
	float fAngle = 360.f/m_dirs * iDir;

	UINT uiStrID = 0;
	CString strText;
	strText.Format(IDS_GAIT_ANGLE_FORMAT, fAngle);
	strText += DirectionNameByAngle::get(fAngle);

	int iItem = m_list.InsertItem(iDir, strText);
	if(iItem == -1)
		return false;
	
	m_list.SetItemText(iItem, 1, walk_info->animation_name());
	m_list.SetItemText(iItem, 2, stand_info->animation_name());
	m_list.SetItemText(iItem, 3, start_info->animation_name());
	m_list.SetItemText(iItem, 4, stop_info->animation_name());
	strText.Format(_T("%f"), walk_info->speed());
	m_list.SetItemText(iItem, 5, strText);
	strText.Format(_T("%f"), walk_info->animation_speed());
	m_list.SetItemText(iItem, 6, strText);

	strText.Format(_T("%f"), m_ptrObj->walk_sound_frequency(iDir));
	m_list.SetItemText(iItem, 7, strText);
	return true;
}

//загружаем список анимаций походок
bool MovingDlg::LoadMovings()
{
	m_list.DeleteAllItems();
	m_dirs = m_ptrObj->size();
	for(int i = 0; i < m_dirs; ++i)
	{
		InsertAnimInfo(i, m_ptrObj->get_animation_info(i), 
			m_ptrObj->get_static_animation_info(i),
			m_ptrObj->get_start_animation_info(i),
			m_ptrObj->get_stop_animation_info(i));
	}
	if (m_dirs)
		m_list.SetItemState(0, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
	return true;
}

void MovingDlg::AdjustColumnsWidth()
{
	int const cnt = m_list.GetHeaderCtrl()->GetItemCount();
	for(int i = 0; i < cnt; ++i)
		m_list.SetColumnWidth(i, LVSCW_AUTOSIZE_USEHEADER);
}

void MovingDlg::OnEndEditList(NMHDR * pNMHDR, LRESULT* pResult)
{
	NM_LCE_INFO * pInfo = (NM_LCE_INFO*)pNMHDR;
	if(pInfo->iEditCtrlType == ELC_ITEM_EDIT_CTRL_COMBO)
	{
		qdAnimation* pA = m_ptrGameDisp->get_animation(pInfo->item.pszText);

		if(pA && !pA->is_resource_loaded())
			pA->load_resource();

		if(pInfo->item.iSubItem == 1)
		{
			m_ptrObj->get_animation_info(pInfo->item.iItem)
				->set_animation_name(pInfo->item.pszText);
		}
		else if(pInfo->item.iSubItem == 2)
		{
			m_ptrObj->get_static_animation_info(pInfo->item.iItem)
				->set_animation_name(pInfo->item.pszText);
		}
		else if(pInfo->item.iSubItem == 3)
		{
			m_ptrObj->get_start_animation_info(pInfo->item.iItem)
				->set_animation_name(pInfo->item.pszText);
		}
		else if(pInfo->item.iSubItem == 4)
		{
			m_ptrObj->get_stop_animation_info(pInfo->item.iItem)
				->set_animation_name(pInfo->item.pszText);
		}

		*pResult = TRUE;
	}
	else if (pInfo->iEditCtrlType == ELC_ITEM_EDIT_CTRL_EDIT) {
		double dSpeed = 0.0;
		char* pEnd = NULL;
		CString str(pInfo->item.pszText);
		str.TrimLeft();
		str.TrimRight();
		dSpeed = strtod(str, &pEnd);
		if(*pEnd != '\0')
		{
			*pResult = FALSE;
			return;
		}
		if (pInfo->item.iSubItem == 5)
		{
			m_ptrObj->get_animation_info(pInfo->item.iItem)
				->set_speed((float)dSpeed);
			m_ptrASP->set_personage_speed((float)dSpeed);
		}
		else if (pInfo->item.iSubItem == 6)
		{
			if (dSpeed < 0.0001)
			{
				*pResult = FALSE;
				return;
			}
			m_ptrObj->get_animation_info(pInfo->item.iItem)
				->set_animation_speed((float)dSpeed);
		}
		else if (pInfo->item.iSubItem == 7)
		{
			if (dSpeed < 0.0001)
			{
				*pResult = FALSE;
				return;
			}
			m_ptrObj->set_walk_sound_frequency(pInfo->item.iItem, (float)dSpeed);
		}
		strcpy(pInfo->item.pszText, str);
		*pResult = TRUE;
	}

	m_ptrASP->set_direction(pInfo->item.iItem);
	m_ptrASP->redraw();

	AdjustColumnsWidth();
	SetProjChange();
}

void MovingDlg::OnSetupCtrlList(NMHDR * pNMHDR, LRESULT* pResult)
{
	NM_LCE_INFO * pInfo = (NM_LCE_INFO*)pNMHDR;

	if(pInfo->iEditCtrlType == ELC_ITEM_EDIT_CTRL_COMBO)
	{
		CStringSet_t::iterator _itr = m_ptrAnimList->begin(), 
			_end = m_ptrAnimList->end();

		CCJFlatComboBox* ptrCombo = (CCJFlatComboBox*)pInfo->pCtrl;
		for(; _itr != _end; ++_itr)
		{
			ptrCombo->AddString(*_itr);
		}
		if(pInfo->item.iSubItem == 1)
			ptrCombo->SelectString(-1, 
				m_ptrObj->get_animation_info
					(pInfo->item.iItem)->animation_name());
		else if(pInfo->item.iSubItem == 2)
			ptrCombo->SelectString(-1, 
				m_ptrObj->get_static_animation_info
								(pInfo->item.iItem)->animation_name());
		else if(pInfo->item.iSubItem == 3)
			ptrCombo->SelectString(-1, 
				m_ptrObj->get_start_animation_info
								(pInfo->item.iItem)->animation_name());
		else if(pInfo->item.iSubItem == 4)
			ptrCombo->SelectString(-1, 
				m_ptrObj->get_stop_animation_info
								(pInfo->item.iItem)->animation_name());
	}
}

void MovingDlg::OnCheckShow() 
{
	UpdateData();

	m_wndAnimPhase.EnableWindow(!m_bRunAnim&&m_ptrCurrAnimInfo);
	if(m_bRunAnim)
	{
		ShowAnimation();
	}
	else
	{
		_ASSERT(m_pbShowAnimation);
		*m_pbShowAnimation = false;
		m_pbShowAnimation = NULL;
		SetASPPhase();
	}
}

bool MovingDlg::ShowAnimation()
{
	static bool s_bShow = true;

	s_bShow = true;
	m_pbShowAnimation = &s_bShow;

	m_ptrASP->start();

	while (s_bShow) {
		MSG msg;
		while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if(msg.message == WM_QUIT)
			{
				ExitProcess(0);
			}
			CWinThread* pthr = AfxGetThread();
			if (!pthr || !pthr->PreTranslateMessage(&msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		m_ptrASP->quant();
		m_ptrASP->redraw();
	}
	m_pbShowAnimation = NULL;
	return true;
}

void MovingDlg::OnClickList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LVHITTESTINFO hti;
	memset(&hti, 0, sizeof(hti));
	GetCursorPos(&hti.pt);
	m_list.ScreenToClient(&hti.pt);
	int res = ListView_SubItemHitTest(m_list.m_hWnd, &hti);
	
	
	
	if(hti.iSubItem == 2) { // столбец остановок
		m_ptrASP->
			set_preview_mode(qdAnimationSetPreview::VIEW_STATIC_ANIMATIONS);
		m_ptrCurrAnimInfo = m_ptrObj->get_static_animation_info(hti.iItem);
	}
	else if(hti.iSubItem == 3) {// столбец стартов
		m_ptrASP->
			set_preview_mode(qdAnimationSetPreview::VIEW_START_ANIMATIONS);
		m_ptrCurrAnimInfo = m_ptrObj->get_start_animation_info(hti.iItem);
	}
	else if(hti.iSubItem == 4) {// столбец стопов
		m_ptrASP->
			set_preview_mode(qdAnimationSetPreview::VIEW_STOP_ANIMATIONS);
		m_ptrCurrAnimInfo = m_ptrObj->get_stop_animation_info(hti.iItem);
	}
	else
	{
		m_ptrASP->
			set_preview_mode(qdAnimationSetPreview::VIEW_WALK_ANIMATIONS);
		m_ptrCurrAnimInfo = m_ptrObj->get_animation_info(hti.iItem);
	}
	
	SetChecksState(m_ptrCurrAnimInfo);
	m_ptrASP->set_direction(hti.iItem);

	SetPhaseRange();
	SetASPPhase();

	if(!m_pbShowAnimation)
		m_ptrASP->redraw();


	*pResult = 0;
}


bool MovingDlg::CreateXGraph()
{
	m_ptrXGR = std::auto_ptr<GDI_grDispatcher>(new GDI_grDispatcher);
	if(!m_ptrXGR.get())
		return false;
	CRect r;
	CWnd * wnd = GetDlgItem(IDC_STATIC_VIEW);
	wnd->GetClientRect(&r);

	m_ptrXGR->init(
		r.Width(), r.Height(),
		GR_RGB888,
		wnd->m_hWnd);

	m_ptrXGR->SetClip();
	m_ptrXGR->SetClipMode(1);
	
	m_ptrXGR->Fill(0);

	return true;
}

bool MovingDlg::CreateAnimPreview()
{
	m_ptrASP = std::auto_ptr<qdAnimationSetPreview>(new qdAnimationSetPreview(m_ptrObj));
	if(!m_ptrASP.get())
		return false;
	m_ptrASP->set_graph_dispatcher(m_ptrXGR.get());
	qdAnimationInfo* ptrAI = m_ptrObj->get_animation_info(0);
	if(ptrAI)
	{
		qdAnimation* ptrA = ptrAI->animation();
		if(ptrA)
			m_ptrASP->set_personage_height(ptrA->size_y());
		else
			m_ptrASP->set_personage_height(300);
	}
	else
		m_ptrASP->set_personage_height(300);

	m_ptrASP->set_direction(0);
	m_ptrASP->quant();
	return true;
}

void MovingDlg::PostNcDestroy() 
{
	if(m_pbShowAnimation)
		*m_pbShowAnimation = false;

	CDialog::PostNcDestroy();
}

void MovingDlg::OnChangeEditPersHeight() 
{
	if(m_bInitDialog)
		return;
	if(!UpdateData())
		return;
	m_ptrASP->set_personage_height((float)m_iPersHeight);
	m_ptrASP->start();
	if(!m_pbShowAnimation)
		m_ptrASP->redraw();
}

void MovingDlg::OnChangeEditGridSize() 
{
	if(m_bInitDialog)
		return;
	if(!UpdateData())
		return;
	m_ptrASP->set_cell_size(m_iGridSize);
	m_ptrASP->start();
	if(!m_pbShowAnimation)
		m_ptrASP->redraw();
}

void MovingDlg::OnChangeEditCamPitch() 
{
	if(m_bInitDialog)
		return;
	if(!UpdateData())
		return;
	m_ptrASP->set_camera_angle((float)m_iCamPitch);
	m_ptrASP->start();
	if(!m_pbShowAnimation)
		m_ptrASP->redraw();
}

void MovingDlg::OnChangeEditCamFocus() 
{
	if(m_bInitDialog)
		return;
	if(!UpdateData())
		return;
	m_ptrASP->set_camera_focus((float)m_iCamFocus);
	m_ptrASP->start();
	if(!m_pbShowAnimation)
		m_ptrASP->redraw();
}

void MovingDlg::OnBtnBackColor() 
{
	CColorDialog dlg;
	dlg.m_cc.Flags |= CC_FULLOPEN | CC_RGBINIT;
	LOGBRUSH lb;
	m_brBackColor.GetLogBrush(&lb);
	dlg.m_cc.rgbResult = lb.lbColor;
	if(dlg.DoModal() == IDOK)
	{
		m_brBackColor.DeleteObject();
		m_brBackColor.CreateSolidBrush(dlg.GetColor());

		m_ptrASP->set_back_color(dlg.GetColor());
		m_ptrASP->redraw();
		
		GetDlgItem(IDC_STATIC_BACK_COLOR)->Invalidate();
	}
}

void MovingDlg::OnBtnColor() 
{
	CColorDialog dlg;
	dlg.m_cc.Flags |= CC_FULLOPEN | CC_RGBINIT;
	LOGBRUSH lb;
	m_brGridColor.GetLogBrush(&lb);
	dlg.m_cc.rgbResult = lb.lbColor;
	if(dlg.DoModal() == IDOK)
	{
		m_brGridColor.DeleteObject();
		m_brGridColor.CreateSolidBrush(dlg.GetColor());

		m_ptrASP->set_grid_color(dlg.GetColor());
		m_ptrASP->redraw();

		GetDlgItem(IDC_STATIC_GRID_COLOR)->Invalidate();
	}
}

HBRUSH MovingDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	if(	pWnd->GetDlgCtrlID() == IDC_STATIC_GRID_COLOR)
		return m_brGridColor;
	else if(pWnd->GetDlgCtrlID() == IDC_STATIC_BACK_COLOR)
		return m_brBackColor;
	return hbr;
}

void MovingDlg::AnimPreview2Wnd()
{
	animation_ = 0;
	if(const char* name = m_ptrObj->get_turn_animation_info()->animation_name()){
		CString str(name);
	
		CStringSet_t::const_iterator _itr = m_ptrAnimList->find(str);

		if(_itr != m_ptrAnimList->end())
			animation_ = 1 + std::distance<CStringSet_t::const_iterator>(m_ptrAnimList->begin(), _itr);
	}

	m_iGridSize			= m_ptrASP->cell_size();
	m_iPersHeight		= m_ptrASP->personage_height();
	m_iCamPitch			= (int)m_ptrASP->camera_angle();
	m_iCamFocus			= (int)m_ptrASP->camera_focus();

	m_brBackColor.DeleteObject();
	m_brBackColor.CreateSolidBrush(m_ptrASP->back_color());

	m_brGridColor.DeleteObject();
	m_brGridColor.CreateSolidBrush(m_ptrASP->grid_color());
	SetPhaseRange();
}

void MovingDlg::AdjustSpints()
{
	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_CAM_PITCH))->
		SetRange(CAM_PITCH_MIN, CAM_PITCH_MAX);
	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_CAM_FOCUS))->
		SetRange(CAM_FOCUS_MIN, CAM_FOCUS_MAX);
	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_PERS_HEIGHT))->
				SetRange(CAM_PERS_HEIGHT_MIN, CAM_PERS_HEIGHT_MAX);
	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_GRID_SIZE))->
		SetRange(CAM_GRID_SIZE_MIN, CAM_GRID_SIZE_MAX);
}

BOOL MovingDlg::DestroyWindow() 
{
	if(m_pbShowAnimation)
		*m_pbShowAnimation = false;
	return CDialog::DestroyWindow();
}

void MovingDlg::OnCancel() 
{
	if(m_pbShowAnimation)
		*m_pbShowAnimation = false;

	*m_ptrObj = m_BackAnimSet;
	CDialog::OnCancel();
}

void MovingDlg::OnOK() 
{
	if(!UpdateData())
		return;

	m_ptrObj->set_start_angle(m_fStartAngle);

	if(m_pbShowAnimation)
		*m_pbShowAnimation = false;

	CString text;
	if(animation_ != 0){
		animations_.GetLBText(animation_, text);
		m_ptrObj->set_turn_animation(text);
	}
	else
		m_ptrObj->set_turn_animation(0);
	
	CDialog::OnOK();
}

void MovingDlg::OnCheckFlipHorz() 
{
	if(!UpdateData())
		return;
	if(m_ptrCurrAnimInfo)
	{
		if(m_bFlipHorz)
			m_ptrCurrAnimInfo->set_flag(QD_ANIMATION_FLAG_FLIP_HORIZONTAL);
		else
			m_ptrCurrAnimInfo->drop_flag(QD_ANIMATION_FLAG_FLIP_HORIZONTAL);

		m_ptrASP->set_direction(m_ptrASP->direction());
		m_ptrASP->redraw();
	}
}

void MovingDlg::OnCheckFlipVert() 
{
	if(!UpdateData())
		return;
	if(m_ptrCurrAnimInfo)
	{
		if(m_bFlipVert)
			m_ptrCurrAnimInfo->set_flag(QD_ANIMATION_FLAG_FLIP_VERTICAL);
		else
			m_ptrCurrAnimInfo->drop_flag(QD_ANIMATION_FLAG_FLIP_VERTICAL);

		m_ptrASP->set_direction(m_ptrASP->direction());
		m_ptrASP->redraw();
	}
}

void MovingDlg::OnCheckLoop() 
{
	if(!UpdateData())
		return;
	if(m_ptrCurrAnimInfo)
	{
		if(m_bLoop)
			m_ptrCurrAnimInfo->set_flag(QD_ANIMATION_FLAG_LOOP);
		else
			m_ptrCurrAnimInfo->drop_flag(QD_ANIMATION_FLAG_LOOP);

		m_ptrASP->set_direction(m_ptrASP->direction());
		m_ptrASP->redraw();
	}
}

void MovingDlg::EnableCheckBoxes(BOOL bEnable)
{
	GetDlgItem(IDC_CHECK_FLIP_VERT)->EnableWindow(bEnable);
	GetDlgItem(IDC_CHECK_FLIP_HORZ)->EnableWindow(bEnable);
	GetDlgItem(IDC_CHECK_LOOP)->EnableWindow(bEnable);
}

void MovingDlg::SetChecksState(qdAnimationInfo* ptrAI)
{
	if(ptrAI)
	{
		EnableCheckBoxes(TRUE);
		int iFlags	= ptrAI->flags();
		m_bFlipVert = 
			(iFlags&QD_ANIMATION_FLAG_FLIP_VERTICAL)==QD_ANIMATION_FLAG_FLIP_VERTICAL;
		m_bFlipHorz = 
			(iFlags&QD_ANIMATION_FLAG_FLIP_HORIZONTAL)
													==QD_ANIMATION_FLAG_FLIP_HORIZONTAL;
		m_bLoop		= (iFlags&QD_ANIMATION_FLAG_LOOP)==QD_ANIMATION_FLAG_LOOP;
	}
	else
	{
		EnableCheckBoxes(FALSE);
		m_bFlipVert = FALSE;
		m_bFlipHorz = FALSE;
		m_bLoop		= FALSE;
	}
	UpdateData(FALSE);
}

void MovingDlg::OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CPoint point;
	GetCursorPos(&point);
	m_list.ScreenToClient(&point);
	
	LVHITTESTINFO lvht;
	lvht.pt = point;
	int nHitItem = ListView_SubItemHitTest(m_list.m_hWnd, &lvht);

	if(nHitItem >= 0)
		m_list.EditSubItem(nHitItem, lvht.iSubItem);

	*pResult = 0;
}

void MovingDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	if (m_ptrASP.get()&&!m_pbShowAnimation)
		m_ptrASP->redraw();
}

void MovingDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (pScrollBar->m_hWnd == m_wndAnimPhase.m_hWnd)
	{
		switch(nSBCode) {
		case SB_LINELEFT:
		case SB_LINERIGHT:
		case SB_PAGELEFT:
		case SB_PAGERIGHT:
		case SB_THUMBPOSITION: // Scroll to absolute position. nPos is the position
		case SB_THUMBTRACK:   // Drag scroll box to specified position. nPos is the
			// of the scroll box at the end of the drag operation.
			SetASPPhase();
			AccessToAnimCtrls();
			break;
		}
	}
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

float MovingDlg::GetPhase() const{
	if (!m_wndAnimPhase.GetRangeMax())
		return 0.f;
	return static_cast<float>(m_wndAnimPhase.GetPos())/m_wndAnimPhase.GetRangeMax();
}

void MovingDlg::SetPhaseRange(){
	if (qdAnimation const* const pa = m_ptrASP->cur_animation())
		m_wndAnimPhase.SetRange(0, pa->num_frames()-1, TRUE);
	else
		m_wndAnimPhase.SetRange(0, 0, TRUE);
}

void MovingDlg::SetASPPhase(){
	m_ptrASP->set_phase(GetPhase());
	if(!m_pbShowAnimation)
		m_ptrASP->redraw();
}

void MovingDlg::OnBnClickedBtnToBeg()
{
	m_wndAnimPhase.SetPos(0);
	SetASPPhase();
	AccessToAnimCtrls();
}

void MovingDlg::OnBnClickedBtnPrev()
{
	m_wndAnimPhase.SetPos(m_wndAnimPhase.GetPos() - 1);
	SetASPPhase();
	AccessToAnimCtrls();
}

void MovingDlg::OnBnClickedBtnNext()
{
	m_wndAnimPhase.SetPos(m_wndAnimPhase.GetPos() + 1);
	SetASPPhase();
	AccessToAnimCtrls();
}

void MovingDlg::OnBnClickedBtnToEnd()
{
	m_wndAnimPhase.SetPos(m_wndAnimPhase.GetRangeMax());
	SetASPPhase();
	AccessToAnimCtrls();
}

void MovingDlg::AccessToAnimCtrls()
{
	int const pos = m_wndAnimPhase.GetPos();
	int const min = m_wndAnimPhase.GetRangeMin();
	int const max = m_wndAnimPhase.GetRangeMax();

	GetDlgItem(IDC_BTN_TO_BEG)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_NEXT)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_TO_END)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_PREV)->EnableWindow(TRUE);
	if (min == pos)
	{
		GetDlgItem(IDC_BTN_TO_BEG)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_PREV)->EnableWindow(FALSE);
	}

	if (max == pos)
	{
		GetDlgItem(IDC_BTN_TO_END)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_NEXT)->EnableWindow(FALSE);
	}
}

void MovingDlg::LoadBitmaps()
{
	CBitmap bmp;
	bmp.LoadBitmap(IDB_TO_BEGIN);
	static_cast<CButton*>(GetDlgItem(IDC_BTN_TO_BEG))->SetBitmap(bmp);
	bmp.Detach();

	bmp.LoadBitmap(IDB_TO_END);
	static_cast<CButton*>(GetDlgItem(IDC_BTN_TO_END))->SetBitmap(bmp);
	bmp.Detach();

	bmp.LoadBitmap(IDB_PREV);
	static_cast<CButton*>(GetDlgItem(IDC_BTN_PREV))->SetBitmap(bmp);
	bmp.Detach();

	bmp.LoadBitmap(IDB_NEXT);
	static_cast<CButton*>(GetDlgItem(IDC_BTN_NEXT))->SetBitmap(bmp);
	bmp.Detach();
}

void MovingDlg::OnDeltaposSpinStartAngle(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	if(!UpdateData()){
		*pResult = 1;
		return;
	}
	m_fStartAngle += pNMUpDown->iDelta/100.f;
	if (m_fStartAngle < 0.f)
		m_fStartAngle = 0.f;
	if (m_fStartAngle > 360.f)
		m_fStartAngle = 360.f;
	m_ptrObj->set_start_angle(m_fStartAngle);
	m_ptrASP->set_direction(m_ptrASP->direction());
	
	UpdateData(FALSE);
	if(!m_pbShowAnimation)
		m_ptrASP->redraw();

	*pResult = 0;
}


BOOL MovingDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN&& pMsg->wParam == VK_RETURN)
	{
		CWnd* const pwnd = GetFocus();
		if (::GetDlgItem(m_hWnd, IDC_EDIT_START_ANGLE) == pwnd->m_hWnd)
		{
			if (UpdateData()) 
			{
				m_ptrObj->set_start_angle(m_fStartAngle);
				m_ptrASP->set_direction(m_ptrASP->direction());
				if(!m_pbShowAnimation)
					m_ptrASP->redraw();
			}
			return TRUE;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}


