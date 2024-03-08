// ObjPosPropPage.cpp : implementation file
//

#include "stdafx.h"
#include "questeditor.h"

#include "qd_game_object_moving.h"
#include "qd_game_scene.h"
#include "qd_game_dispatcher.h"

#include "qd_d3dutils.h"
#include "ObjPosPropPage.h"
#include "custom_ddx.h"
#include <cmath>

#include "ObjectDepthDeltaFromNetProjDepthDelta.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const int SCR_X_MAX = 32000;
const int SCR_X_MIN = -32000;
const int SCR_Y_MAX = 32000;
const int SCR_Z_MAX = 32000;
const int SCR_Y_MIN = -32000;
const int SCR_Z_MIN = -32000;

const int NET_PROJ_D_MAX = 32000;
const int NET_PROJ_D_MIN = -32000;

const int WORLD_X_MAX = 50000;
const int WORLD_Y_MAX = 50000;
const int WORLD_Z_MAX = 50000;
const int WORLD_X_MIN = -50000;
const int WORLD_Y_MIN = -50000;
const int WORLD_Z_MIN = -50000;
/************************************************************************/
/* Внешние объявления                                                   */
/************************************************************************/
namespace helpers{
	/// Назначает позицию объекту, учитывая тип объекта и наличие КА
	void set_obj_pos(qdGameObject* pobj, Vect3f const& pos)
	{
		if (pobj->named_object_type()!=QD_NAMED_OBJECT_STATIC_OBJ)
		{
			qdGameObjectAnimated* const paobj = 
				static_cast<qdGameObjectAnimated*>(pobj);
			qdGameObjectState* const pcur_state = 
				paobj->get_state(paobj->cur_state());
			if (pcur_state&&!pcur_state->coords_animation()->is_empty())
			{
				pcur_state->coords_animation()->get_point()->set_dest_pos(pos);
				pcur_state->coords_animation()->reset_cur_point();
				return;
			}
			paobj->set_default_pos(pos);
		}
		pobj->set_pos(pos);
	}

	/// Возвращает правильную позицию объекта, учитывая тип объекта и наличие КА
	const Vect3f& get_obj_pos(qdGameObject* pobj)
	{
		if (pobj->named_object_type()!=QD_NAMED_OBJECT_STATIC_OBJ)
		{
			qdGameObjectAnimated* const paobj = static_cast<qdGameObjectAnimated*>(pobj);
			qdGameObjectState* const pcur_state = paobj->get_state(paobj->cur_state());
			if (pcur_state&&!pcur_state->coords_animation()->is_empty())
				return pcur_state->coords_animation()->get_point()->dest_pos();
		}
		return pobj->R();
	}
	/// Вычисляет глубину проекции на экран
	int calcNetProjDepth(qdCamera const* pcam, Vect3f pos){
		pos.z = pcam->get_grid_center().z;
		pos = pcam->global2camera_coord(pos);
		return round(pos.z);
	}

	//! Вычисляет угол между сеткой и прямой-проекцией объекта на экран
	double calcAngle(qdCamera const* pcam, Vect3f const& p1)
	{
		Vect3f camPos = pcam->global2camera_coord(p1);
		camPos.z += 1.f;
		Vect3f p2 = pcam->camera_coord2global(camPos);
		p2 = p1 - p2;

		double const sinPh = 
			static_cast<double>(p2.z)*static_cast<double>(p2.z)
					/static_cast<double>(p2.norm2());
//		return asin(sinPh);
		return sinPh;
	}
	/**
		Пытается вычислить новую глубину объекта в зависимости от изменения глубины
		проекции объекта.
		\param pos - текущая позиция объекта
		\param D - изменение глубины проекции объекта на сетку

		Работала почти правильно. Т.е. она правильно вычисляла, только если объект был 
		под сеткой, то оказывался над ней. Похоже в этом случае надо У на противоположный
		поменять
	 */
	Vect3f calcNewPos(qdCamera const* pcam, Vect3f const& pos, int D)
	{
		MATRIX3D const& M = pcam->get_view_matrix();
		float const m11 = M._11;
		float const m12 = M._12;
		float const m13 = M._13;
		float const m21 = M._21;
		float const m22 = M._22;
		float const m23 = M._23;
		float const m31 = M._31;
		float const m32 = M._32;
		float const m33 = M._33;
		float const m41 = M._41;
		float const m42 = M._42;
		float const m43 = M._43;

		float const focus = pcam->get_focus();

		float const x = pos.x;
		float const y = pos.y;
		float const z = pos.z;

		float const t3 = m23*m23;
		float const t16 = -m11*m32+m31*m12;
		float const t14 = t16*t3;
		float const t68 = y*t14;
		float const t54 = m33*z;
		float const t39 = -m43-focus;
		float const t26 = t54-t39;
		float const t67 = 2.0*y*(D-t26);
		float const t15 = -m11*m22+m21*m12;
		float const t9 = m33*t15;
		float const t66 = -m41*m12+m11*m42;
		float const t53 = m12*x;
		float const t65 = (-m42+t53)*m33;
		float const t38 = 2.0*t54;
		float const t24 = t38-t39;
		float const t22 = D-t24;
		float const t43 = m41*m33;
		float const t64 = t43-t22*m31;
		float const t63 = t43+t24*m31;
		float const t42 = m42*m33;
		float const t62 = -t42-t24*m32;
		float const t61 = -t42+t22*m32;
		float const t60 = 2.0*m42;
		float const t2 = m13*m13;
		float const t59 = x*t2;
		float const t58 = m13*x;
		float const t57 = m11*x;
		float const t56 = m21*y;
		float const t55 = m31*z;
		float const t52 = m22*y;
		float const t51 = m32*z;
		float const t50 = x*m33;
		float const t5 = x*x;
		float const t49 = m13*t5;
		float const t4 = y*y;
		float const t48 = m23*t4;
		float const t41 = m22*m33;
		float const t40 = m32*m23;
		float const t37 = z*t50;
		float const t36 = 2.0*t39;
		float const t34 = 2.0*t4;
		float const t33 = -D-t39;
		float const t32 = D+t36;
		float const t31 = 2.0*m41+t55;
		float const t30 = -t51-2.0*m42;
		float const t28 = -m41-t56;
		float const t27 = m42+t52;
		float const t25 = t33*m32;
		float const t23 = -2.0*t58+t36;
		float const t20 = t38-t32;
		float const t18 = m21*m32-m31*m22;
		float const t17 = -t58-t33;
		float const t13 = t18*t2;
		float const t12 = m23*y+t24;
		float const t11 = t20*m41;
		float const t10 = t20*m42;
		float const t8 = D*t9;
		float const t7 = -2.0*D+t54-t23;
		float const t6 = -2.0*m41*m32+2.0*m31*m42;
		float const t1 = 1/(-t68+t18*t59+(((-2.0*t55+t28)*m12+(2.0*t51+t27)*m11)*m33+t39*t16)
			*m23+(-t16*m23*x+((-t57-m41)*m33-t12*m31)*m22+((m42+t53)*m33+t12*m32)*m21)*m13);
		float const y1 = (t6*t59+(-t14-m23*t9)*t4+(t63*m12+t62*m11)*D+(-x*t13+t8+(-t64*m12-
			t61*m11)*m23)*y+(2.0*t66*t50+(2.0*m11*t37-m21*t48-t11)*m32+(-2.0*m12*t37+m22*
			t48+t10)*m31+((-t41-2.0*t40)*m41+(t40+t41)*t57+((t60-t53)*m23+t33*m22)*m31+(-
			t65-t25)*m21)*y)*m13)*t1;
		float const x1 = -((-t18*t34+y*t6)*t3+(-t13-m13*t9)*t5+(-t63*m22-t62*m21)*D+(t68+t8+(
			t64*m22+t61*m21)*m13)*x+((-m11*t49-t11+m21*t67)*m32+(m12*t49+t10-m22*t67)*m31+(
			t28*m12*m33+(-2.0*m41-3.0*t56)*m32*m13+((3.0*t52+t60)*m13+t33*m12)*m31+(t27*m33
			-t25)*m11)*x)*m23)*t1;
		float const z1 = ((t31*m22+t30*m21)*t59+(t15*t34+(t31*m12+t30*m11)*y)*t3-t15*t26*D+((
			((-t57+m41)*m33+t33*m31)*m22+(t65-t25)*m21)*z+(-m22*m41+m42*m21)*t32)*m13+(((
			t43-t17*m31)*m12+(-t42+t17*m32)*m11)*z+((t31*m13-t7*m11)*m22+(t30*m13+t7*m12)*
			m21)*y+t66*(D+t23))*m23)*t1;

		return Vect3f(x1, y1, z1);
	}
}
using namespace helpers;
/////////////////////////////////////////////////////////////////////////////
// ObjPosPropPage property page

IMPLEMENT_DYNCREATE(ObjPosPropPage, CBasePage)

ObjPosPropPage::ObjPosPropPage() : CBasePage(ObjPosPropPage::IDD)
, m_iNetProjD(0)
{
	m_bInitDialog = true;
	//{{AFX_DATA_INIT(ObjPosPropPage)
	m_iScrX = 0;
	m_iScrY = 0;
	m_iX = 0;
	m_iY = 0;
	m_iZ = 0;
	m_iDepth = 0;
	m_bUseLeftTop = FALSE;
	m_bFixedScreenCoords = FALSE;
	//}}AFX_DATA_INIT

	m_ptrGameDisp	= NULL;
	m_ptrScene		= NULL;
	m_ptrObj		= NULL;
	m_bRedraw		= false;
}

ObjPosPropPage::~ObjPosPropPage()
{
}

void ObjPosPropPage::DoDataExchange(CDataExchange* pDX)
{
	CBasePage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ObjPosPropPage)
	custom_ddx::DDX_Text(pDX, IDC_EDIT_SCR_X, m_iScrX);
	DDX_Text(pDX, IDC_EDIT_SCR_Y, m_iScrY);
	DDX_Text(pDX, IDC_EDIT_X, m_iX);
	DDX_Text(pDX, IDC_EDIT_Y, m_iY);
	DDX_Text(pDX, IDC_EDIT_Z, m_iZ);
	DDX_Text(pDX, IDC_EDIT_SCR_D, m_iDepth);
	DDX_Check(pDX, IDC_USE_LEFT_TOP, m_bUseLeftTop);
	DDX_Check(pDX, IDC_FIXED_SCREEN_COORDS, m_bFixedScreenCoords);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_EDIT_NET_PROJ_SCR_D, m_iNetProjD);
}


BEGIN_MESSAGE_MAP(ObjPosPropPage, CBasePage)
//{{AFX_MSG_MAP(ObjPosPropPage)
	ON_BN_CLICKED(IDC_USE_LEFT_TOP, OnUseLeftTop)
	ON_BN_CLICKED(IDC_FIXED_SCREEN_COORDS, OnFixedScreenCoords)
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
	ON_MESSAGE(PSM_QUERYSIBLINGS, OnQuerySiblings)
	ON_WM_VSCROLL()
	ON_EN_CHANGE(IDC_EDIT_X, OnEnChangeWorldPos)
	ON_EN_CHANGE(IDC_EDIT_Y, OnEnChangeWorldPos)
	ON_EN_CHANGE(IDC_EDIT_Z, OnEnChangeWorldPos)

	ON_EN_CHANGE(IDC_EDIT_SCR_X, OnEnChangeScrPos)
	ON_EN_CHANGE(IDC_EDIT_SCR_Y, OnEnChangeScrPos)
	ON_EN_CHANGE(IDC_EDIT_SCR_D, OnEnChangeScrPos)

	ON_EN_KILLFOCUS(IDC_EDIT_SCR_D, OnEnKillfocusInScrPos)
	ON_EN_KILLFOCUS(IDC_EDIT_SCR_X, OnEnKillfocusInScrPos)
	ON_EN_KILLFOCUS(IDC_EDIT_SCR_Y, OnEnKillfocusInScrPos)

	ON_EN_KILLFOCUS(IDC_EDIT_X, OnEnKillfocusInWorldPos)
	ON_EN_KILLFOCUS(IDC_EDIT_Y, OnEnKillfocusInWorldPos)
	ON_EN_KILLFOCUS(IDC_EDIT_Z, OnEnKillfocusInWorldPos)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_NET_PROJ_SCR_D, OnDeltaposSpinNetProjScrD)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ObjPosPropPage message handlers
void ObjPosPropPage::AdjustSpins()
{
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_WX))->
		SetRange32(WORLD_X_MIN, WORLD_X_MAX);
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_WY))->
		SetRange32(WORLD_Y_MIN, WORLD_Y_MAX);
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_WZ))->
		SetRange32(WORLD_Z_MIN, WORLD_Z_MAX);
	
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_SX))->
		SetRange32(SCR_X_MIN, SCR_X_MAX);
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_SY))->
		SetRange32(SCR_Y_MIN, SCR_Y_MAX);
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_SZ))->
		SetRange32(SCR_Z_MIN, SCR_Z_MAX);
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_NET_PROJ_SCR_D))->
		SetRange32(NET_PROJ_D_MIN, NET_PROJ_D_MAX);
}

bool ObjPosPropPage::set_data(qdGameDispatcher* ptrGD, qdGameObject* ptrObj)
{
	m_ptrGameDisp = ptrGD;
	ASSERT(m_ptrGameDisp);
	
	m_ptrObj = ptrObj;
	m_ptrScene = (qdGameScene*)ptrObj->owner();
	ASSERT(m_ptrScene);
	
	m_bRedraw = (m_ptrScene == m_ptrGameDisp->get_active_scene());
	return true;
}

//! рассовывает данные об объекте по диалогу
void ObjPosPropPage::Obj2Wnd()
{
	Vect3f pos(get_obj_pos(m_ptrObj));	
	m_iX = round(pos.x);
	m_iY = round(pos.y);
	m_iZ = round(pos.z);
	
	world2screen();
}

//! из координат левого верхнего угла координаты центра
Vect3f ObjPosPropPage::lt2c(Vect3f pos)
{
	qdCamera * const ptrCam = m_ptrScene->get_camera();
	pos	= ptrCam->global2camera_coord(pos);
	Vect2s rscr = ptrCam->camera_coord2scr(pos);
	
	Vect2s size = m_ptrObj->screen_size();
	
	//получили координаты центра угла на экране.
	rscr.x += (size.x>>1);
	rscr.y += (size.y>>1);
	
	//получаем координаты центра в мировых
	rscr = ptrCam->scr2rscr(rscr);
	
	Vect3f res(ptrCam->rscr2global(rscr, pos.z));

	res.x = round(res.x);
	res.y = round(res.y);
	res.z = round(res.z);
	return res;
}

//! из координат центра координаты левого верхнего угла
Vect3f ObjPosPropPage::c2lt(Vect3f pos)
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


BOOL ObjPosPropPage::OnInitDialog() 
{
	CBasePage::OnInitDialog();
	
	if(!m_ptrObj)
		return FALSE;
	
	Obj2Wnd();
	real2bak();
	
	UpdateData(FALSE);
	
	if(!m_bRedraw)
		DisablePositionCtrls();
	
	AdjustSpins();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL ObjPosPropPage::OnApply() 
{
	if(!UpdateData())
		return FALSE;

	real2bak();
	apply_params();
	redraw();
	
	return CBasePage::OnApply();
}

//! перерысовыет окно редактора сцены
bool ObjPosPropPage::redraw()
{
	if(m_bRedraw)
		redraw_scene(true);
	return true;
}

void ObjPosPropPage::OnEnterInScrPos() 
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

void ObjPosPropPage::OnEnterInWorldPos() 
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

//! по изменению глубины проектции на сетку вычисляет позицию объекта
void ObjPosPropPage::changeNetProjScrD(int delta)
{
	qdCamera const* const pcam = m_ptrScene->get_camera();
	Vect3f objPos = get_obj_pos(m_ptrObj);
	double const phi = calcAngle(pcam, objPos);
	float const fObjDepthDelta = static_cast<double>(delta)/(1. - phi);
	objPos = pcam->global2camera_coord(objPos);
	Vect2s rscr = pcam->camera_coord2rscr(objPos);
	objPos.z = (objPos.z*pcam->get_focus())/(objPos.z+pcam->get_focus());
	objPos.z += fObjDepthDelta;
	objPos.z = objPos.z*pcam->get_focus()/(pcam->get_focus()-objPos.z);
	objPos = pcam->rscr2global(rscr, objPos.z + fObjDepthDelta);

	int depth = calcNetProjDepth(pcam, objPos);
	m_iX = round(objPos.x);
	m_iY = round(objPos.y);
	m_iZ = round(objPos.z);
	world2screen();
	redraw();
}

void ObjPosPropPage::OnEnterInNetProjScrD()
{
	if(m_bInitDialog)
		return;
	int iOld = m_iNetProjD;
	if(!UpdateData())
		return;
	if (m_iNetProjD - iOld) 
	{
//		changeNetProjScrD(m_iNetProjD - iOld);
		qdCamera const* const pcam = m_ptrScene->get_camera();
		Vect3f objPos = get_obj_pos(m_ptrObj);
		objPos = calcNewPos(pcam, objPos, m_iNetProjD - iOld);
		m_iX = round(objPos.x);
		m_iY = round(objPos.y);
		m_iZ = round(objPos.z);
		world2screen();
		redraw();
		SetModified(TRUE);
	}

	UpdateData(FALSE);
}

void ObjPosPropPage::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CBasePage::OnShowWindow(bShow, nStatus);
	
	m_bInitDialog = false;	
}

//! Применяет параметры
void ObjPosPropPage::apply_params()
{
	apply_pos_params();
}

//! \brief Функция сохраняет исходные значения парметров
//! в резервных копиях
void ObjPosPropPage::real2bak()
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
}

//! восстанавливает параметры из резервных копий
void ObjPosPropPage::bak2real()
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
}


void ObjPosPropPage::OnCancel() 
{
	bak2real();
	apply_params();
	redraw();
	
	CBasePage::OnCancel();
}

//! переводит экранные коородинаты в координаты игрового мира
Vect3f ObjPosPropPage::screen2world(int x, int y, int z)
{
	Vect2s scr(x, y);
	scr = m_ptrScene->get_camera()->scr2rscr(scr);
	Vect3f res = m_ptrScene->get_camera()->rscr2global(scr, z);
	res.x = round(res.x);
	res.y = round(res.y);
	res.z = round(res.z);
	return res;
}

//! переводит экранные коородинаты в координаты игрового мира
void ObjPosPropPage::screen2world()
{
	Vect3f pos = screen2world(m_iScrX, m_iScrY, m_iDepth);
	
	if(m_bUseLeftTop)
		set_obj_pos(m_ptrObj,lt2c(pos));
	else
		set_obj_pos(m_ptrObj, pos);

	if (m_ptrObj->named_object_type() == QD_NAMED_OBJECT_MOVING_OBJ)
		static_cast<qdGameObjectMoving*>(m_ptrObj)->adjust_z();
	pos = get_obj_pos(m_ptrObj);

	// глубина проекции. Не работает нихера
	m_iNetProjD = calcNetProjDepth(m_ptrScene->get_camera(), pos);

	m_iX = round(pos.x);
	m_iY = round(pos.y);
	m_iZ = round(pos.z);

	// коррекция погрешности конвертации
	world2screen(pos, m_iScrX, m_iScrY, m_iDepth);
}

void ObjPosPropPage::world2screen(Vect3f pos, int& x, int& y, int &z)
{
	qdCamera * ptrCam = m_ptrScene->get_camera();
	pos	= ptrCam->global2camera_coord(pos);
	Vect2s rscr = ptrCam->camera_coord2scr(pos);

	x	= rscr.x;
	y	= rscr.y;
	z	= round(pos.z);
}

void ObjPosPropPage::world2screen()
{
	Vect3f pos(static_cast<float>(m_iX),
				static_cast<float>(m_iY),
				static_cast<float>(m_iZ));
	
	if(m_bUseLeftTop)
		set_obj_pos(m_ptrObj, lt2c(pos));
	else
		set_obj_pos(m_ptrObj, pos);

	if (m_ptrObj->named_object_type() == QD_NAMED_OBJECT_MOVING_OBJ)
		static_cast<qdGameObjectMoving*>(m_ptrObj)->adjust_z();
	pos = get_obj_pos(m_ptrObj);
	//! глубина проекции
	m_iNetProjD = calcNetProjDepth(m_ptrScene->get_camera(), pos);
	
	world2screen(pos, m_iScrX, m_iScrY, m_iDepth);
}

//! Применяет параметы, определяющие положение объекта
void ObjPosPropPage::apply_pos_params()
{
	Vect3f pos(static_cast<float>(m_iX),
		static_cast<float>(m_iY),
		static_cast<float>(m_iZ));
	if(m_bUseLeftTop)
		pos = lt2c(pos);

	set_obj_pos(m_ptrObj, pos);
}

//! Делает недоступным контролы отвечающие за позицию объекта
void ObjPosPropPage::DisablePositionCtrls()
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

LRESULT ObjPosPropPage::OnQuerySiblings( WPARAM wParam, LPARAM lParam )
{
	if(wParam == PPN_BOUND_CHANGED)
	{
		Obj2Wnd();
		if(m_bUseLeftTop)
		{
//			Vect3f pos = c2lt(m_ptrObj->R());
			Vect3f pos = c2lt(get_obj_pos(m_ptrObj));

			m_iX = round(pos.x);
			m_iY = round(pos.y);
			m_iZ = round(pos.z);
			
			world2screen();
		}
		UpdateData(FALSE);
	}
	return 0;
}

void ObjPosPropPage::OnUseLeftTop() 
{
	if(!UpdateData())
		return;
	if (m_bUseLeftTop) 
	{
		Vect3f pos = c2lt(Vect3f(m_iX, m_iY, m_iZ));
		
		m_iX = round(pos.x);
		m_iY = round(pos.y);
		m_iZ = round(pos.z);
		world2screen();
	}
	else
		Obj2Wnd();

	UpdateData(FALSE);
}

void ObjPosPropPage::OnFixedScreenCoords() 
{
	if(!UpdateData())
		return;

	if (m_bFixedScreenCoords) {
		m_ptrObj->set_flag(QD_OBJ_FIXED_SCREEN_COORDS_FLAG);
		m_ptrObj->drop_flag(QD_OBJ_SCREEN_COORDS_FLAG);
	}
	else {
		m_ptrObj->drop_flag(QD_OBJ_FIXED_SCREEN_COORDS_FLAG);
		m_ptrObj->drop_flag(QD_OBJ_SCREEN_COORDS_FLAG);
	}

	SetModified(TRUE);
	
	UpdateData(FALSE);
}

BOOL ObjPosPropPage::PreTranslateMessage(MSG* pMsg)
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

void ObjPosPropPage::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
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

void ObjPosPropPage::OnEnChangeWorldPos()
{
	if (m_bInitDialog)
		return;
	SetModified(TRUE);
}

void ObjPosPropPage::OnEnChangeScrPos()
{
	if (m_bInitDialog)
		return;
	SetModified(TRUE);
}

void ObjPosPropPage::OnEnKillfocusInScrPos()
{
	if(!UpdateData())
		return;
	if (GetModified())
	{
		screen2world();
		UpdateData(FALSE);
	}
}

void ObjPosPropPage::OnEnKillfocusInWorldPos()
{
	if(!UpdateData())
		return;			
	if (GetModified())
	{
		world2screen();
		UpdateData(FALSE);
	}
}

void ObjPosPropPage::OnDeltaposSpinNetProjScrD(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

	if (!UpdateData())
		return;
	changeNetProjScrD(pNMUpDown->iDelta);
	m_iNetProjD += pNMUpDown->iDelta;
	if (pNMUpDown->iDelta) 
		SetModified();
	UpdateData(FALSE);
	*pResult = 0;
}
