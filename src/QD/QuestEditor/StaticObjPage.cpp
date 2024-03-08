// StaticObjPage.cpp : implementation file
//

#include "stdafx.h"
#include "questeditor.h"

#include <vector>

#include "StaticObjPage.h"

#include "relativepath.h"

#include "qd_game_object_static.h"
#include "qd_game_scene.h"
#include "qd_game_dispatcher.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Максимум и минумум для экранного Х
const int SCR_X_MAX = 20000;
const int SCR_X_MIN = -20000;

// Максимум и минумум для экранных Y и Z 
const int SCR_Y_MAX = 20000;
const int SCR_Z_MAX = 20000;

const int SCR_Y_MIN = -20000;
const int SCR_Z_MIN = -20000;

// Максимум и минумум для глубины проекции на сетку
const int NET_PROJ_D_MAX = 32000;
const int NET_PROJ_D_MIN = -32000;

// Максимум и минумум для мировых Х, Y и Z 
const int WORLD_X_MAX = 20000;
const int WORLD_Y_MAX = 20000;
const int WORLD_Z_MAX = 20000;
const int WORLD_X_MIN = -20000;
const int WORLD_Y_MIN = -20000;
const int WORLD_Z_MIN = -20000;

CurDirSafer g_curDirSafe;

/////////////////////////////////////////////////////////////////////////////
// StaticObjPage property page

namespace helpers{
	double calcAngle(qdCamera const* pcam, Vect3f const& p1);
	int calcNetProjDepth(qdCamera const* pcam, Vect3f pos);
}
IMPLEMENT_DYNCREATE(StaticObjPage, CBasePage)

StaticObjPage::StaticObjPage() : CBasePage(StaticObjPage::IDD)
{
	m_bInitDialog = true;
	//{{AFX_DATA_INIT(StaticObjPage)
	m_iScrX = 0;
	m_iScrY = 0;
	m_strPicFileName = _T("");
	m_iX = 0;
	m_iY = 0;
	m_iZ = 0;
	m_iDepth = 0;
	m_bUseLeftTop = FALSE;
	m_bImmovable = FALSE;
	//}}AFX_DATA_INIT
	m_ptrGameDisp	= NULL;
	m_ptrScene		= NULL;
	m_ptrObj		= NULL;
	m_bRedraw		= false;
}

StaticObjPage::~StaticObjPage()
{
}

bool StaticObjPage::set_data(qdGameDispatcher* ptrGD, qdGameObjectStatic* ptrObj)
{
	m_ptrGameDisp = ptrGD;
	ASSERT(m_ptrGameDisp);

	m_ptrObj = ptrObj;
	m_ptrScene = (qdGameScene*)ptrObj->owner();
	ASSERT(m_ptrScene);

	m_bRedraw = (m_ptrScene == m_ptrGameDisp->get_active_scene());
	return true;
}

void StaticObjPage::DoDataExchange(CDataExchange* pDX)
{
	CBasePage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(StaticObjPage)
	DDX_Text(pDX, IDC_EDIT_SCR_X, m_iScrX);
	DDX_Text(pDX, IDC_EDIT_SCR_Y, m_iScrY);
	DDX_Text(pDX, IDC_EDIT6, m_strPicFileName);
	DDX_Text(pDX, IDC_EDIT_X, m_iX);
	DDX_Text(pDX, IDC_EDIT_Y, m_iY);
	DDX_Text(pDX, IDC_EDIT_Z, m_iZ);
	DDX_Text(pDX, IDC_EDIT_SCR_D, m_iDepth);
	DDX_Check(pDX, IDC_USE_LEFT_TOP, m_bUseLeftTop);
	DDX_Check(pDX, IDC_CHECK_IMMOVABLE, m_bImmovable);
	DDX_Text(pDX, IDC_EDIT_NET_PROJ_SCR_D, m_iNetProjD);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(StaticObjPage, CBasePage)
	//{{AFX_MSG_MAP(StaticObjPage)
/*
	ON_EN_CHANGE(IDC_EDIT_X, OnChangeWorldPos)
	ON_EN_CHANGE(IDC_EDIT_Y, OnChangeWorldPos)
	ON_EN_CHANGE(IDC_EDIT_Z, OnChangeWorldPos)
	ON_EN_CHANGE(IDC_EDIT_SCR_X, OnChangeScrPos)
	ON_EN_CHANGE(IDC_EDIT_SCR_Y, OnChangeScrPos)
	ON_EN_CHANGE(IDC_EDIT_SCR_D, OnChangeScrPos)
*/
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_SELECT_BTN, OnSelectBtn)
	ON_BN_CLICKED(IDC_USE_LEFT_TOP, OnUseLeftTop)
	ON_BN_CLICKED(IDC_CHECK_IMMOVABLE, OnCheckImmovable)
	//}}AFX_MSG_MAP
	ON_WM_VSCROLL()
	ON_EN_KILLFOCUS(IDC_EDIT_SCR_D, OnEnKillfocusInScrPos)
	ON_EN_KILLFOCUS(IDC_EDIT_SCR_X, OnEnKillfocusInScrPos)
	ON_EN_KILLFOCUS(IDC_EDIT_SCR_Y, OnEnKillfocusInScrPos)
	ON_EN_KILLFOCUS(IDC_EDIT_X, OnEnKillfocusInWorldPos)
	ON_EN_KILLFOCUS(IDC_EDIT_Y, OnEnKillfocusInWorldPos)
	ON_EN_KILLFOCUS(IDC_EDIT_Z, OnEnKillfocusInWorldPos)

	ON_EN_CHANGE(IDC_EDIT_SCR_D, OnEnChangeInScrPos)
	ON_EN_CHANGE(IDC_EDIT_SCR_X, OnEnChangeInScrPos)
	ON_EN_CHANGE(IDC_EDIT_SCR_Y, OnEnChangeInScrPos)
	ON_EN_CHANGE(IDC_EDIT_X, OnEnChangeInWorldPos)
	ON_EN_CHANGE(IDC_EDIT_Y, OnEnChangeInWorldPos)
	ON_EN_CHANGE(IDC_EDIT_Z, OnEnChangeInWorldPos)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_NET_PROJ_SCR_D, OnDeltaposSpinNetProjScrD)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// StaticObjPage message handlers

BOOL StaticObjPage::OnApply() 
{
	if(!UpdateData())
		return FALSE;

	real2bak();

	apply_params();
	redraw();
	return CBasePage::OnApply();
}

bool StaticObjPage::redraw()
{
	if(m_bRedraw)
		redraw_scene(true);
	return true;
}

void StaticObjPage::OnEnterInScrPos() 
{
	if(m_bInitDialog)
		return;

	if(!UpdateData())
		return;
	SetModified(TRUE);
	screen2world();
	redraw();
	UpdateData(FALSE);
}

void StaticObjPage::OnEnterInWorldPos() 
{
	if(m_bInitDialog)
		return;
	
	if(!UpdateData())
		return;
	SetModified(TRUE);
	
	world2screen();
	redraw();
	UpdateData(FALSE);
}

void StaticObjPage::changeNetProjScrD(int delta)
{
	qdCamera const* const pcam = m_ptrScene->get_camera();
	Vect3f objPos = m_ptrObj->R();
	double const phi = helpers::calcAngle(pcam, objPos);
	float const fObjDepthDelta = static_cast<double>(delta)/(cos(phi)*cos(phi));
	objPos = pcam->global2camera_coord(objPos);
	Vect2s rscr = pcam->camera_coord2rscr(objPos);

	objPos = pcam->rscr2global(rscr, objPos.z + fObjDepthDelta);
	m_iX = round(objPos.x);
	m_iY = round(objPos.y);
	m_iZ = round(objPos.z);
	world2screen();
	redraw();
}

void StaticObjPage::OnEnterInNetProjScrD()
{
	if(m_bInitDialog)
		return;
	int iOld = m_iNetProjD;
	if(!UpdateData())
		return;
	if (m_iNetProjD - iOld) 
	{
		changeNetProjScrD(m_iNetProjD - iOld);
		SetModified(TRUE);
	}

	UpdateData(FALSE);
}

void StaticObjPage::obj2scr()
{
	m_strPicFileName = m_ptrObj->get_sprite()->file();
	
	Vect3f pos = m_ptrObj->R();	
	m_iX = round(pos.x);
	m_iY = round(pos.y);
	m_iZ = round(pos.z);

	m_bImmovable = 
		static_cast<int>(m_ptrObj->check_flag(QD_OBJ_DISABLE_MOVEMENT_FLAG));
	
	world2screen();
}

Vect3f StaticObjPage::lt2c(Vect3f pos)
{
	qdCamera * ptrCam = m_ptrScene->get_camera();
	pos	= ptrCam->global2camera_coord(pos);
	Vect2s rscr = ptrCam->camera_coord2scr(pos);
	
	Vect2s size = m_ptrObj->screen_size();
	
	//получили координаты лв. угла на экране.
	rscr.x += (size.x>>1);
	rscr.y += (size.y>>1);
	
	//получаем координаты лв. в мировых
	rscr = ptrCam->scr2rscr(rscr);

	Vect3f res(ptrCam->rscr2global(rscr, pos.z));
	res.x = round(res.x);
	res.y = round(res.y);
	res.z = round(res.z);
	return res;
}

Vect3f StaticObjPage::c2lt(Vect3f pos)
{
	qdCamera * ptrCam = m_ptrScene->get_camera();
	pos	= ptrCam->global2camera_coord(pos);
	Vect2s rscr = ptrCam->camera_coord2scr(pos);
	
	Vect2s size = m_ptrObj->screen_size();

	//получили координаты лв. угла на экране.
	rscr.x -= (size.x>>1);
	rscr.y -= (size.y>>1);

	//получаем координаты лв. в мировых
	rscr = ptrCam->scr2rscr(rscr);
	Vect3f res(ptrCam->rscr2global(rscr, pos.z));
	res.x = round(res.x);
	res.y = round(res.y);
	res.z = round(res.z);
	return res;
}

BOOL StaticObjPage::OnInitDialog() 
{
	CBasePage::OnInitDialog();
	
	if(!m_ptrObj)
		return TRUE;

	obj2scr();

	real2bak();

	UpdateData(FALSE);
	
	if(!m_bRedraw)
		DisablePositionCtrls();
	
	AdjustSpins();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void StaticObjPage::apply_params()
{
	apply_pos_params();
	apply_picture();
}

void StaticObjPage::real2bak()
{
	if(m_bUseLeftTop)//копию всегда храним в координатах центра
	{
		Vect3f pos = lt2c(Vect3f(m_iX, m_iY, m_iZ));
		m_iXBak = pos.x;
		m_iYBak = pos.y;
		m_iZBak = pos.z;
		world2screen(pos, m_iScrXBak, m_iScrYBak, m_iDepthBak);
	}
	else
	{
		m_iScrXBak	= m_iScrX; 
		m_iScrYBak	= m_iScrY; 
		m_iXBak		= m_iX;    
		m_iYBak		= m_iY;    
		m_iZBak		= m_iZ;    
		m_iDepthBak = m_iDepth;
	}
	m_strPicFileNameBak	= m_strPicFileName;
	m_bImmovableBak = m_bImmovable;
}

void StaticObjPage::bak2real()
{
	if(m_bUseLeftTop)//копию всегда храним в координатах центра
	{
		Vect3f pos = c2lt(Vect3f(m_iXBak, m_iYBak, m_iZBak));
		m_iX = pos.x;
		m_iY = pos.y;
		m_iZ = pos.z;

		world2screen(pos, m_iScrX, m_iScrY, m_iDepth);
	}
	else
	{
		m_iScrX		=  m_iScrXBak;
		m_iScrY		=  m_iScrYBak;
		m_iX   		=  m_iXBak;
		m_iY   		=  m_iYBak;
		m_iZ   		=  m_iZBak;
		m_iDepth	=  m_iDepthBak;
	}
	m_strPicFileName = m_strPicFileNameBak;
	m_bImmovable = m_bImmovableBak;
}

void StaticObjPage::OnCancel() 
{
	bak2real();
	apply_params();
	UpdateData(FALSE);
	redraw();

	CBasePage::OnCancel();
}

Vect3f StaticObjPage::screen2world(int x, int y, int z)
{
	Vect2s scr;
	scr.x = x;
	scr.y = y;
	scr = m_ptrScene->get_camera()->scr2rscr(scr);
	Vect3f res = m_ptrScene->get_camera()->rscr2global(scr, z);

	res.x = round(res.x);
	res.y = round(res.y);
	res.z = round(res.z);

	return res;
}

void StaticObjPage::screen2world()
{
	Vect3f pos = screen2world(m_iScrX, m_iScrY, m_iDepth);

	if(m_bUseLeftTop)
		m_ptrObj->set_pos(lt2c(pos));
	else
		m_ptrObj->set_pos(pos);

	m_iX = round(pos.x);
	m_iY = round(pos.y);
	m_iZ = round(pos.z);
}

void StaticObjPage::world2screen(Vect3f pos, int& x, int& y, int &z)
{
	qdCamera * ptrCam = m_ptrScene->get_camera();
	pos	= ptrCam->global2camera_coord(pos);
	Vect2s rscr = ptrCam->camera_coord2scr(pos);
	x	= rscr.x;
	y	= rscr.y;
	z	= round(pos.z);
}

void StaticObjPage::world2screen()
{
	Vect3f pos;
	pos.x = static_cast<float>(m_iX);
	pos.y = static_cast<float>(m_iY);
	pos.z = static_cast<float>(m_iZ);
	
	if(m_bUseLeftTop)
		m_ptrObj->set_pos(lt2c(pos));
	else
		m_ptrObj->set_pos(pos);

	m_iNetProjD = helpers::calcNetProjDepth(m_ptrScene->get_camera(), pos);
	world2screen(pos, m_iScrX, m_iScrY, m_iDepth);
}

void StaticObjPage::apply_picture()
{
	qdSprite* psprite = m_ptrObj->get_sprite();
	ASSERT(psprite);
	if(!m_strPicFileName.IsEmpty())
		psprite->set_file(m_strPicFileName);
	else
		psprite->set_file(NULL);

	m_ptrObj->free_resources();
	if(!m_strPicFileName.IsEmpty())
		m_ptrObj->load_resources();
}

void StaticObjPage::apply_pos_params()
{
	Vect3f pos;
	pos.x = static_cast<float>(m_iX);
	pos.y = static_cast<float>(m_iY);
	pos.z = static_cast<float>(m_iZ);
	if(m_bUseLeftTop)
		pos = lt2c(pos);

	m_ptrObj->set_pos(pos);
}

void StaticObjPage::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CBasePage::OnShowWindow(bShow, nStatus);
	
	m_bInitDialog = false;
}

void StaticObjPage::DisablePositionCtrls()
{
	GetDlgItem(IDC_EDIT_X)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_Y)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_Z)->EnableWindow(FALSE);

	GetDlgItem(IDC_EDIT_SCR_X)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_SCR_Y)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_SCR_D)->EnableWindow(FALSE);

	GetDlgItem(IDC_SPIN_WX)->EnableWindow(FALSE);
	GetDlgItem(IDC_SPIN_WY)->EnableWindow(FALSE);
	GetDlgItem(IDC_SPIN_WZ)->EnableWindow(FALSE);
	
	GetDlgItem(IDC_SPIN_SX)->EnableWindow(FALSE);
	GetDlgItem(IDC_SPIN_SY)->EnableWindow(FALSE);
	GetDlgItem(IDC_SPIN_SZ)->EnableWindow(FALSE);
}

CString StaticObjPage::SelectPicture(HWND m_hWndParent, const CString& strTitle, CStringVect_t* ptrVect)
{
	CString str;
	str.LoadString(IDS_OPEN_SPRITE_EXT);
	CString str2;
	str2.LoadString(IDS_OPEN_SPRITE_FILTER);
	
	//запомнили текущую и выставили соответствующую этому диалогу
	CurDirGuard __cdg(&g_curDirSafe);
	
	CFileDialog dlg(TRUE, str, NULL, OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST, str2);
	if(ptrVect)
	{
		dlg.m_ofn.Flags |= OFN_ALLOWMULTISELECT;
	}
	CString curDir = CurDirSafer::GetCurrentDirectory();
	dlg.m_ofn.lpstrInitialDir = curDir;
	dlg.m_ofn.lpstrTitle = strTitle;
	if(dlg.DoModal() == IDOK)
	{
		//достали сохраненную
		CString strCurrDir = __cdg.get_saved();
		strCurrDir += "\\";
		
		CString rel_path;
		if(ptrVect)
		{
			POSITION pos = dlg.GetStartPosition();
			while (pos) {
				if((!r_p::RelativePath(dlg.GetNextPathName(pos), strCurrDir, rel_path)))
				{
					strCurrDir.LoadString(IDS_ERR_MSG_CAPTION);
					rel_path.LoadString(IDS_EM_CNNT_GET_REL_PATH);
					::MessageBox(m_hWndParent, rel_path, strCurrDir, MB_OK|MB_ICONHAND);
					return CString();
				}
				ptrVect->push_back(rel_path);
			}
			rel_path = ptrVect->front();
		}
		else
		{
			if((!r_p::RelativePath(dlg.GetPathName(), strCurrDir, rel_path)))
			{
				strCurrDir.LoadString(IDS_ERR_MSG_CAPTION);
				rel_path.LoadString(IDS_EM_CNNT_GET_REL_PATH);
				::MessageBox(m_hWndParent, rel_path, strCurrDir, MB_OK|MB_ICONHAND);
				return CString();
			}
		}
		g_curDirSafe.save_current();
		return rel_path;
	}

	return CString();
}

void StaticObjPage::OnSelectBtn() 
{
	CString strTitle((LPTSTR)IDS_OSD_CAP_SEL_STAT_OBJ);
	CString rel_path = StaticObjPage::SelectPicture(*this, strTitle);
	if(!rel_path.IsEmpty())
	{
		if(_tcsicmp(m_strPicFileName,rel_path))
		{
			SetModified();
			m_strPicFileName = rel_path;

			UpdateData(FALSE);
			apply_picture();
			redraw();
		}
	}
}

void StaticObjPage::AdjustSpins()
{
	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_WX))->SetRange(WORLD_X_MIN, WORLD_X_MAX);
	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_WY))->SetRange(WORLD_Y_MIN, WORLD_Y_MAX);
	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_WZ))->SetRange(WORLD_Z_MIN, WORLD_Z_MAX);
	
	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_SX))->SetRange(SCR_X_MIN, SCR_X_MAX);
	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_SY))->SetRange(SCR_Y_MIN, SCR_Y_MAX);
	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_SZ))->SetRange(SCR_Z_MIN, SCR_Z_MAX);
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_NET_PROJ_SCR_D))->
		SetRange32(NET_PROJ_D_MIN, NET_PROJ_D_MAX);
}

void StaticObjPage::OnUseLeftTop() 
{
	if(!UpdateData())
		return;
	if (m_bUseLeftTop) 
	{
		Vect3f pos = c2lt(Vect3f(m_iX, m_iY, m_iZ));

		m_iX = pos.x;
		m_iY = pos.y;
		m_iZ = pos.z;
		world2screen();
	}
	else
	{
		obj2scr();
	}
	UpdateData(FALSE);
}

void StaticObjPage::OnCheckImmovable() 
{
	UpdateData();
	if (m_bImmovable) 
		m_ptrObj->set_flag(QD_OBJ_DISABLE_MOVEMENT_FLAG);
	else
		m_ptrObj->drop_flag(QD_OBJ_DISABLE_MOVEMENT_FLAG);
	SetModified(TRUE);
}


BOOL StaticObjPage::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		switch(pMsg->wParam) {
		case VK_RETURN:
			CWnd* pfoc = GetFocus();
			if (pfoc) 
			{
				switch(pfoc->GetDlgCtrlID())
				{
				case IDC_EDIT_X:
				case IDC_EDIT_Y:
				case IDC_EDIT_Z:
					OnEnterInWorldPos();
					break;
				case IDC_EDIT_SCR_X:
				case IDC_EDIT_SCR_Y:
				case IDC_EDIT_SCR_D:
					OnEnterInScrPos();
					break;
				case IDC_EDIT_NET_PROJ_SCR_D:
					OnEnterInNetProjScrD();
					break;
				}
				return TRUE;
			}
		}
	}

	return CBasePage::PreTranslateMessage(pMsg);
}

void StaticObjPage::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (nSBCode == SB_ENDSCROLL) 
	{
		switch(pScrollBar->GetDlgCtrlID()){
		case IDC_SPIN_WX:
		case IDC_SPIN_WY:
		case IDC_SPIN_WZ:
			OnEnterInWorldPos();
			break;
		case IDC_SPIN_SX:
		case IDC_SPIN_SY:
		case IDC_SPIN_SZ:
			OnEnterInScrPos();
			break;
		}
	}

	CBasePage::OnVScroll(nSBCode, nPos, pScrollBar);
}

void StaticObjPage::OnEnKillfocusInScrPos()
{
	if(!UpdateData())
		return;
	if (GetModified())
	{
		screen2world();
		UpdateData(FALSE);
	}
}

void StaticObjPage::OnEnKillfocusInWorldPos()
{
	if(!UpdateData())
		return;
	if (GetModified()) 
	{
		world2screen();
		UpdateData(FALSE);
	}
}

void StaticObjPage::OnEnChangeInScrPos()
{
	if (m_bInitDialog)
		return;
	SetModified();
}

void StaticObjPage::OnEnChangeInWorldPos()
{
	if (m_bInitDialog)
		return;
	SetModified();
}

void StaticObjPage::OnDeltaposSpinNetProjScrD(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	changeNetProjScrD(pNMUpDown->iDelta);
	if (pNMUpDown->iDelta)
	{
		SetModified();
		UpdateData(FALSE);
	}
	*pResult = 0;
}
