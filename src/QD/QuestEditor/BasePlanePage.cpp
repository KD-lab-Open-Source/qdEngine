// BasePlanePage.cpp : implementation file
//

#include "stdafx.h"
#include "QuestEditor.h"

#include <algorithm>

#include "qd_game_object_moving.h"
#include "qd_camera.h"
#include "qd_game_scene.h"
#include "qd_game_dispatcher.h"
#include "BasePlanePage.h"

#include <boost/function_output_iterator.hpp>
#include <boost/mem_fn.hpp>
#include <boost/bind.hpp>
#include ".\baseplanepage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/************************************************************************/
/* Внещние определения                                                  */
/************************************************************************/
//! заполняет зону ячейками попавшими в контур
bool GZFillCells(qdGridZone* ptrZone, qdCamera* ptrCam);
/////////////////////////////////////////////////////////////////////////////
// BasePlanePage property page

const int ROTATE_X_SPIN_MAX		= 80;
const int ROTATE_X_SPIN_MIN		= 1;

const int ROTATE_Y_SPIN_MAX		= 85;
const int ROTATE_Y_SPIN_MIN		= -85;

const int ROTATE_Z_SPIN_MAX		= 85;
const int ROTATE_Z_SPIN_MIN		= -85;

const int Z_SPIN_MAX			= 10000;
const int Z_SPIN_MIN			= -10000;

const int FOCUS_SPIN_MAX		= 10000;
const int FOCUS_SPIN_MIN		= 1;

const int CELL_SIZE_SPIN_MAX	= 1<<7;
const int CELL_SIZE_SPIN_MIN	= 1;

const int CELL_SX_SPIN_MAX		= 1000;
const int CELL_SX_SPIN_MIN		= 1;

const int CELL_SY_SPIN_MAX		= 1000;
const int CELL_SY_SPIN_MIN		= 1;

IMPLEMENT_DYNCREATE(BasePlanePage, CBasePage)

BasePlanePage::BasePlanePage() : CBasePage(BasePlanePage::IDD)
, scalePow_(0.f)
, zOffset_(0.f)
, scalePowBak_(0.f)
, zOffsetBak_(0.f)
{
	m_ptrGameDisp	= NULL;
	m_ptrCam		= NULL;
	m_ptrScene = NULL;
	m_bInitDialog = true;
	//{{AFX_DATA_INIT(BasePlanePage)
	m_iCellSize = 0;
	m_iCellSX = 0;
	m_iCellSY = 0;
	m_iFocus = 6000;
	m_iRotateX = 1;
	m_iRotateY = 0;
	m_iRotateZ = 0;
	m_iZ = 0;
	//}}AFX_DATA_INIT
	m_iCellSizeBak = 0;
	m_iCellSXBak = 0;
	m_iCellSYBak = 0;
	m_iFocusBak = 0;
	m_iRotateXBak = 0;
	m_iRotateYBak = 0;
	m_iRotateZBak = 0;
	m_iZBak = 0;

	m_bRedraw = false;
	m_ptrBak = NULL;
}

BasePlanePage::~BasePlanePage()
{
}

bool BasePlanePage::set_data(qdGameDispatcher* ptrGD, qdNamedObject* ptrGS)
{
	m_ptrGameDisp = ptrGD;
	m_ptrScene = static_cast<qdGameScene*>(ptrGS);
	if(!m_ptrScene)
		m_ptrScene = m_ptrGameDisp->get_active_scene();
	if(!m_ptrScene)
		return false;

	m_bRedraw = (m_ptrScene == m_ptrGameDisp->get_active_scene());
	m_ptrCam = m_ptrScene->get_camera();

	return true;
}

void BasePlanePage::DoDataExchange(CDataExchange* pDX)
{
	CBasePage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(BasePlanePage)
	DDX_Text(pDX, IDC_EDIT_CELLSIZE, m_iCellSize);
	DDX_Text(pDX, IDC_EDIT_CELLSX, m_iCellSX);
	DDX_Text(pDX, IDC_EDIT_CELLSY, m_iCellSY);
	DDX_Text(pDX, IDC_EDIT_FOCUS, m_iFocus);
	DDV_MinMaxInt(pDX, m_iFocus, 1, 10000);
	DDX_Text(pDX, IDC_EDIT_ROTATEX, m_iRotateX);
	DDV_MinMaxInt(pDX, m_iRotateX, ROTATE_X_SPIN_MIN, ROTATE_X_SPIN_MAX);
	DDX_Text(pDX, IDC_EDIT_ROTATEY, m_iRotateY);
	DDV_MinMaxInt(pDX, m_iRotateY, ROTATE_Y_SPIN_MIN, ROTATE_Y_SPIN_MAX);
	DDX_Text(pDX, IDC_EDIT_ROTATEZ, m_iRotateZ);
	DDV_MinMaxInt(pDX, m_iRotateZ, ROTATE_Z_SPIN_MIN, ROTATE_Z_SPIN_MAX);
	DDX_Text(pDX, IDC_EDIT_Z, m_iZ);
	DDV_MinMaxInt(pDX, m_iZ, Z_SPIN_MIN, Z_SPIN_MAX);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_EDIT_SCALE_POW, scalePow_);
	DDX_Text(pDX, IDC_EDIT_Z_OFFSET, zOffset_);
}


BEGIN_MESSAGE_MAP(BasePlanePage, CBasePage)
	//{{AFX_MSG_MAP(BasePlanePage)
	ON_BN_CLICKED(IDC_BUTTON_UNDO, OnButtonUndo)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BTN_LOAD_DEFS, OnBtnLoadDefs)
	ON_BN_CLICKED(IDC_BTN_SAVE_DEFS, OnBtnSaveDefs)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN8, OnDeltaposSpin8)
	ON_EN_CHANGE(IDC_EDIT_CELLSX, OnGridCellParamsChange)
	ON_EN_CHANGE(IDC_EDIT_CELLSY, OnGridCellParamsChange)
	//ON_EN_CHANGE(IDC_EDIT_FOCUS, OnGridLayoutParamsChange)
	//ON_EN_CHANGE(IDC_EDIT_ROTATEX, OnGridLayoutParamsChange)
	//ON_EN_CHANGE(IDC_EDIT_ROTATEY, OnGridLayoutParamsChange)
	//ON_EN_CHANGE(IDC_EDIT_ROTATEZ, OnGridLayoutParamsChange)
	//ON_EN_CHANGE(IDC_EDIT_Z, OnGridLayoutParamsChange)
	//}}AFX_MSG_MAP
	ON_WM_VSCROLL()
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_SCALE_POW, OnDeltaposSpinScalePow)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_Z_OFFSET, OnDeltaposSpinZOffset)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// BasePlanePage message handlers

BOOL BasePlanePage::OnApply() 
{
	if(!UpdateData())
		return FALSE;

	real2bak();
	apply_params();
	ApplyZones();
	
	return CBasePage::OnApply();
}

//! из памяти, в которой производим изменения, в резервную память
void  BasePlanePage::real2bak()
{
	m_iCellSizeBak = m_iCellSize;
	m_iCellSXBak  = m_iCellSX;
	m_iCellSYBak  = m_iCellSY;
	m_iFocusBak   = m_iFocus;
	m_iRotateXBak = m_iRotateX;
	m_iRotateYBak = m_iRotateY;
	m_iRotateZBak = m_iRotateZ;
	m_iZBak		  = m_iZ;

	zOffsetBak_		= zOffset_;
	scalePowBak_	= scalePow_;
	
	if(m_ptrBak)
		delete [] m_ptrBak;
	m_ptrBak = new sGridCell[m_iCellSX*m_iCellSY];
	_ASSERT(m_ptrBak);
	m_ptrBak = m_ptrCam->backup(m_ptrBak);
	
	//создаем резервную копию зон
	//чтобы можно было потом их востановить
	
	const qdGridZoneList& gzl = m_ptrScene->grid_zone_list();
	
	m_vecZoneBak.clear();
	m_vecZoneBak.reserve(gzl.size());
	
	qdGridZoneList::const_iterator _itr = gzl.begin(), _end = gzl.end();
	for(; _itr != _end; ++_itr)
		m_vecZoneBak.push_back(**_itr);
}

//! из резервной памяти в память, в которой производим изменения
void  BasePlanePage::bak2real()
{
	m_iCellSize	= m_iCellSizeBak;
	m_iCellSX	= m_iCellSXBak;
	m_iCellSY	= m_iCellSYBak;
	m_iFocus	= m_iFocusBak;
	m_iRotateX	= m_iRotateXBak;
	m_iRotateY	= m_iRotateYBak;
	m_iRotateZ	= m_iRotateZBak;
	m_iZ		= m_iZBak;
	zOffset_	= zOffsetBak_;
	scalePow_	= scalePowBak_;
	
	m_ptrCam->restore(m_ptrBak,m_iCellSX, m_iCellSY, m_iCellSize, m_iCellSize);
	
	const qdGridZoneList& gzl = m_ptrScene->grid_zone_list();
	
	qdGridZoneList::const_iterator _out = gzl.begin();
	gz_vector_t::iterator _itr = m_vecZoneBak.begin(), _end = m_vecZoneBak.end();
	for(; _itr != _end; ++_itr, ++_out)
	{
		**_out = *_itr;
	}
	SelectZones(true);
	ApplyZones();
}

void BasePlanePage::OnButtonUndo() 
{
	bak2real();
	UpdateData(FALSE);

	apply_params();
	redraw();

	SetModified(FALSE);
}
//обрабатывает изменение параметров сетки
void BasePlanePage::OnGridCellParamsChange() 
{
	if (m_bInitDialog)
		return;

	int iCellSX = m_iCellSX;
	int iCellSY = m_iCellSY;
	
	if(!UpdateData())
		return;

	SelectZones(false);
	apply_grid_param();


	ShiftZones((m_iCellSX - iCellSX)>>1, (m_iCellSY - iCellSY)>>1);
	SelectZones(true);

	redraw();
	SetModified(TRUE);
}
//обрабытывает изменение параметров положения сетки
void BasePlanePage::OnGridLayoutParamsChange() 
{
	if(!UpdateData())
		return;

	apply_layout_param();
	redraw();
	SetModified(TRUE);
}

void BasePlanePage::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CBasePage::OnShowWindow(bShow, nStatus);
	m_bInitDialog = false;	
}

BOOL BasePlanePage::OnInitDialog() 
{
	CBasePage::OnInitDialog();

	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN1))->
		SetRange(ROTATE_X_SPIN_MIN,ROTATE_X_SPIN_MAX);
	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN2))->
		SetRange(ROTATE_Y_SPIN_MIN,ROTATE_Y_SPIN_MAX);
	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN4))->
		SetRange(ROTATE_Z_SPIN_MIN,ROTATE_Z_SPIN_MAX);

	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN3))->
		SetRange(Z_SPIN_MIN,Z_SPIN_MAX);
	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN5))->
		SetRange(FOCUS_SPIN_MIN,FOCUS_SPIN_MAX);
	
	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN6))->
		SetRange(CELL_SX_SPIN_MIN,CELL_SX_SPIN_MAX);
	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN7))->
		SetRange(CELL_SY_SPIN_MIN,CELL_SY_SPIN_MAX);

	reinterpret_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_SCALE_POW))->SetRange(0, 100);
	reinterpret_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_Z_OFFSET))->SetRange(0, 100);

	UDACCEL accels[2];
	accels[0].nSec = 0;
	accels[0].nInc = 2;
	accels[1].nSec = 2;
	accels[1].nInc = 4;
	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN6))->SetAccel(2, accels);
	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN7))->SetAccel(2, accels);

	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN8))->
		SetRange(CELL_SIZE_SPIN_MIN,CELL_SIZE_SPIN_MAX);
	
	
	m_iCellSX	= m_ptrCam->get_grid_sx();
	m_iCellSY	= m_ptrCam->get_grid_sy();

	m_iCellSize = m_ptrCam->get_cell_sx();

	m_iFocus = (int)m_ptrCam->get_focus();

	m_iRotateX = m_ptrCam->get_x_angle();
	m_iRotateY = m_ptrCam->get_y_angle();
	m_iRotateZ = m_ptrCam->get_z_angle();

	m_iZ = calc_Z();

	zOffset_ = m_ptrCam->scale_z_offset();
	scalePow_ = m_ptrCam->scale_pow();

	CalcGridZonesNodesScrCoords();
	real2bak();

	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN8))->SetPos(m_iCellSize);

	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//! Применяет парметры положения сетки(вращение, фокус, масштаб,расстояние до камеры)
bool BasePlanePage::applied_layout_params()
{
	if (m_ptrCam->get_focus() != static_cast<float>(m_iFocus))
		return false;

	if (m_ptrCam->get_z_angle() != static_cast<float>(m_iRotateZ))
		return false;

	if (m_ptrCam->get_y_angle() != static_cast<float>(m_iRotateY))
		return false;

	if (m_ptrCam->get_x_angle() != static_cast<float>(m_iRotateX))
		return false;
	
	if (m_ptrCam->get_grid_center().z != static_cast<float>(m_iZ))
		return false;

	if (abs(m_ptrCam->scale_pow() - scalePow_) > 10e-5)
		return false;

	if (abs(m_ptrCam->scale_z_offset() - zOffset_) > 10e-5)
		return false;
	return true;
}

//! применяет к камере параметры положения
void BasePlanePage::apply_layout_param()
{
	if (!applied_layout_params())
	{
		World2Scr4ObjsCoords();
		m_ptrCam->set_focus(static_cast<float>(m_iFocus));
		apply_Z();
		m_ptrCam->rotate_and_scale(m_iRotateX, m_iRotateY,
								m_iRotateZ, 1, 1, 1);
		m_ptrCam->set_scale_pow(scalePow_);
		m_ptrCam->set_scale_z_offset(zOffset_);
		Scr2World4ObjsCoords();
		SelectZones(false);
		RecalcGridZonesNodes();
		SelectZones(true);
		m_ptrScene->quant(0.f);
	}
}
//! применяет к камере параметры сетки
void BasePlanePage::apply_grid_param()
{
	m_ptrCam->set_cell_size(m_iCellSize, m_iCellSize);
	m_ptrCam->set_grid_size(m_iCellSX, m_iCellSY);
}

//! применяет все параметры
void BasePlanePage::apply_params()
{
	apply_layout_param();
	apply_grid_param();

	redraw();
}

void BasePlanePage::OnCancel() 
{
	bak2real();

	apply_layout_param();
	apply_grid_param();
	redraw();
	
	CBasePage::OnCancel();
}


const Vect3f WorldPos2Scr(qdCamera* pcam, const Vect3f& r)
{
	Vect3f pos	= pcam->global2camera_coord(r);
	Vect2s rscr = pcam->camera_coord2rscr(pos);
	pos.x = rscr.x;
	pos.y = rscr.y;
	return pos;
}

const Vect3f ScrPos2World(qdCamera* pcam, const Vect3f& r)
{
	Vect2s scr;
	scr.x = r.x;
	scr.y = r.y;
	return pcam->rscr2global(scr, r.z);
}

typedef const Vect3f (*CONVERTOR)(qdCamera* pcam, const Vect3f& r);

//! \brief дополнительные действия по переводу коогдинат для координатной анимации
//! и начального положения
//! работает в обе стороны
void ConvertCA(qdCamera* pcam, qdCoordsAnimation* pca, CONVERTOR co)
{
	const int sz = pca->size();
	for(int i = 0; i < sz; ++i)
	{
		qdCoordsAnimationPoint *pp = pca->get_point(i);
		pp->set_dest_pos(co(pcam, pp->dest_pos()));
	}
}

/*!
  Для всех немасочных состояний анимированного объекта, содержащих 
  коородинатную анимацию, вызывает функтор \a co
  \param pcam - камера
  \param po - \a анимированный объект, с состояниями которого работаем
  \param co - применяемый функтор
*/

void AnimObjSuppl(qdCamera* pcam, qdGameObject* po, CONVERTOR co)
{
	//переводим позицию по умолчанию
	qdGameObjectAnimated* pa = static_cast<qdGameObjectAnimated*>(po);
	pa->set_default_pos(co(pcam, pa->default_R()));

	//находим состояния с координатной анимацией и ковертируем точки там.
	const qdGameObjectStateVector& vec = pa->state_vector();
	qdGameObjectStateVector::const_iterator i = vec.begin(), e = vec.end();
	for(; i != e; ++i){
		qdGameObjectState* ps = *i;
		int type = ps->state_type();
		if (type != qdGameObjectState::STATE_MASK) {
			qdCoordsAnimation* ca = ps->coords_animation();
			if (!ca->is_empty()) 
				ConvertCA(pcam, ca, co);				
		}
	}
}

//! Для персонажа сохраняет экранные координаты всех его состояний
void BasePlanePage::SaveStateLayout(qdGameObjectMoving* pm)
{
	const qdGameObjectStateVector& v = pm->state_vector();
	typedef qdGameObjectStateVector::const_iterator ci_t;
	ci_t itr = v.begin(), end = v.end();
	for(; itr != end; ++itr)
	{
		qdGameObjectState* ps = *itr;
		qdCoordsAnimation * const pca = ps->coords_animation();
		if (!pca->is_empty()&&ps->state_type() != qdGameObjectState::STATE_WALK)
		{
			pm->set_state(itr - v.begin());
			pm->update_screen_pos();
			m_state_scr_layout.insert(std::make_pair(ps, pm->screen_pos()));
		}
	}
	pm->set_default_state();
}

//! перещитывает координаты всех объектов в экранные
void BasePlanePage::World2Scr4ObjsCoords()
{
	qdGameObjectList::const_iterator _itr = m_ptrScene->object_list().begin(),
		_end = m_ptrScene->object_list().end();
	for(; _itr != _end; ++_itr)
	{
		qdGameObject* po = *_itr;
		po->set_pos(WorldPos2Scr(m_ptrCam, po->R()));
		const int type = po->named_object_type();
		if(type != QD_NAMED_OBJECT_STATIC_OBJ) 
		{
			if (type == QD_NAMED_OBJECT_MOVING_OBJ)
				SaveStateLayout(static_cast<qdGameObjectMoving*>(po));
			AnimObjSuppl(m_ptrCam, po, WorldPos2Scr);
		}
	}
}

//! \brief Восстанавливает положение состояний на экране. 
//! Он сбивается при смене положения сетки
void BasePlanePage::RestoreStateLayout()
{
	StateScrLayout::const_iterator itr = m_state_scr_layout.begin(),
									end = m_state_scr_layout.end();
	qdGameObjectMoving* owner = NULL;
	for(; itr != end;++itr)
	{
		qdGameObjectState* const ps = itr->first;
		qdGameObjectMoving* pm = static_cast<qdGameObjectMoving*>(ps->owner());
		if (owner != pm)
		{
			if (owner)
				owner->set_default_state();
			owner = pm;
		}
		owner->set_state(itr->first);
		owner->update_screen_pos();
		Vect2i new_scr = owner->screen_pos();
		new_scr -= itr->second;

		ps->set_center_offset(ps->center_offset() - new_scr);
	}
	if (owner)
		owner->set_default_state();
	m_state_scr_layout.clear();
}

//! пересчитывает экранные координаты объектов в мировые
void BasePlanePage::Scr2World4ObjsCoords()
{
	qdGameObjectList::const_iterator _itr = m_ptrScene->object_list().begin(),
		_end = m_ptrScene->object_list().end();
	for(; _itr != _end; ++_itr)
	{
		qdGameObject* po = *_itr;
		Vect2s v(po->R().x, po->R().y);
		po->set_pos(ScrPos2World(m_ptrCam, po->R()));
		if(po->named_object_type() != QD_NAMED_OBJECT_STATIC_OBJ) 
			AnimObjSuppl(m_ptrCam, po, ScrPos2World);
	}
	RestoreStateLayout();
}


//! вычисляет расстояние от центра сетки до камеры
int BasePlanePage::calc_Z()
{
	Vect3f pos = m_ptrCam->get_grid_center();
	return static_cast<int>(pos.z);
}

//! применяем к камере расстояние от центра сетки до камеры
void BasePlanePage::apply_Z()
{
	Vect3f pos = m_ptrCam->get_grid_center();
	pos.z = static_cast<float>(m_iZ);
	m_ptrCam->set_grid_center(pos);
}

//! сравнивает *Bak версии параметров с изменяемыми
bool BasePlanePage::IsCopiesEqual()
{
	if(m_iCellSize != m_iCellSizeBak||
		m_iCellSX != m_iCellSXBak||
		m_iCellSY != m_iCellSYBak||
		m_iFocus != m_iFocusBak||
		m_iRotateX != m_iRotateXBak||
		m_iRotateY != m_iRotateYBak||
		m_iRotateZ != m_iRotateZBak||
		m_iZ != m_iZBak)
		return false;
	return true;
}


void BasePlanePage::redraw()
{
	if(m_bRedraw)
		redraw_scene(true);
}

void BasePlanePage::DivideZones(int iShift)
{
	const qdGridZoneList& gzl = m_ptrScene->grid_zone_list();
	qdGridZoneList::const_iterator _itr = gzl.begin(), _end = gzl.end();
	for(; _itr != _end; ++_itr)
	{
		qdGridZone* ptrZone = *_itr;
		ptrZone->divide_contour(iShift);
		GZFillCells(ptrZone, m_ptrCam);
	}
	
}

void BasePlanePage::MultZones(int iShift /* = 1 */)
{
	const qdGridZoneList& gzl = m_ptrScene->grid_zone_list();
	qdGridZoneList::const_iterator _itr = gzl.begin(), _end = gzl.end();
	for(; _itr != _end; ++_itr)
	{
		qdGridZone* ptrZone = *_itr;
		ptrZone->mult_contour(iShift);
		GZFillCells(ptrZone, m_ptrCam);
	}
}

void BasePlanePage::ShiftZones(int iXShift, int iYShift)
{
	const qdGridZoneList& gzl = m_ptrScene->grid_zone_list();
	qdGridZoneList::const_iterator _itr = gzl.begin(), _end = gzl.end();
	for(; _itr != _end; ++_itr)
	{
		qdGridZone* ptrZone = *_itr;
		ptrZone->shift_contour(iXShift, iYShift);
		GZFillCells(ptrZone, m_ptrCam);
	}
}

//! Для всех зон вызывает qdGridZone::select
void BasePlanePage::SelectZones(bool bSelect)
{
	const qdGridZoneList& gzl = m_ptrScene->grid_zone_list();
	typedef bool (qdGridZone::*loc_sel)(bool) const;
	std::for_each(gzl.begin(), gzl.end(), 
		std::bind2nd(std::mem_fun(static_cast<loc_sel>(qdGridZone::select)), bSelect));
}

void BasePlanePage::ApplyZones()
{
	const qdGridZoneList& gzl = m_ptrScene->grid_zone_list();
	qdGridZoneList::const_iterator _itr = gzl.begin(), _end = gzl.end();
	for(; _itr != _end; ++_itr)
	{
		qdGridZone* ptrZone = *_itr;
		if (ptrZone->state()) {
			ptrZone->set_state(true);
		}
	}
}

void BasePlanePage::OnBtnLoadDefs() 
{
	CQuestEditorApp* app = GetMyApp();
	m_iRotateX = app->GetIntSetting(SETTING_NAME(NET_ANGLE_X));
	m_iRotateY = app->GetIntSetting(SETTING_NAME(NET_ANGLE_Y));
	m_iRotateZ = app->GetIntSetting(SETTING_NAME(NET_ANGLE_Z));
	
	m_iFocus = app->GetIntSetting(SETTING_NAME(NET_FOCUS));
	m_iZ = app->GetIntSetting(SETTING_NAME(NET_VERT_OFFSET));

	CString buff = app->GetStrSetting(SETTING_NAME(NET_SCALE_Z_OFFSET));
	zOffset_ =	atof(buff);
	buff = app->GetStrSetting(SETTING_NAME(NET_SCALE_POW));
	scalePow_ = atof(buff);
	
	m_iCellSX = app->GetIntSetting(SETTING_NAME(NET_SX));
	m_iCellSY = app->GetIntSetting(SETTING_NAME(NET_SY));
	int iCellSize = app->GetIntSetting(SETTING_NAME(NET_CELL_SZ));
	bool bMerge = iCellSize < m_iCellSize;
	int iShift = bMerge ? m_iCellSize / iCellSize
						: iCellSize / m_iCellSize;
	iShift >>= 1;

	m_iCellSize = iCellSize;

	SetModified(TRUE);
	UpdateData(FALSE);

	SelectZones(false);

	apply_grid_param();
	apply_layout_param();

	if (bMerge) {
		MultZones(iShift);
	}
	else
		DivideZones(iShift);
	CalcGridZonesNodesScrCoords();
	SelectZones(true);
	redraw();
}

void BasePlanePage::OnBtnSaveDefs() 
{
	if (!UpdateData()) {
		return;
	}
	CQuestEditorApp* app = GetMyApp();
	app->WriteIntSetting(SETTING_NAME(NET_ANGLE_X),m_iRotateX);
	app->WriteIntSetting(SETTING_NAME(NET_ANGLE_Y),m_iRotateY);
	app->WriteIntSetting(SETTING_NAME(NET_ANGLE_Z),m_iRotateZ);

	app->WriteIntSetting(SETTING_NAME(NET_FOCUS),m_iFocus);
	app->WriteIntSetting(SETTING_NAME(NET_VERT_OFFSET),m_iZ);
	
	app->WriteIntSetting(SETTING_NAME(NET_SX),m_iCellSX);
	app->WriteIntSetting(SETTING_NAME(NET_SY),m_iCellSY);
	app->WriteIntSetting(SETTING_NAME(NET_CELL_SZ),m_iCellSize);

	CString buff;
	buff.Format(_T("%.3f"), zOffset_);
	app->WriteStrSetting(SETTING_NAME(NET_SCALE_Z_OFFSET), buff);
	buff.Format(_T("%.3f"), scalePow_);
	app->WriteStrSetting(SETTING_NAME(NET_SCALE_POW), buff);

}

void BasePlanePage::OnDeltaposSpin8(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	
	if (pNMUpDown->iDelta > 0)
		UpdateCellsCount(m_iCellSize<<1);
	else
		UpdateCellsCount(m_iCellSize>>1);
	
	SetModified(TRUE);
	
	*pResult = 0;
}

//! \brief Проверяет если отрезки в контурах, которые сольются
//! при дальнейшем разрежении сетки. Если "да", то справшиваем у пользователя,
//! продолжать ли операцию
bool BasePlanePage::ZeroCheck()
{
	const qdGridZoneList& gzl = m_ptrScene->grid_zone_list();
	qdGridZoneList::const_iterator _itr = gzl.begin(), _end = gzl.end();
	for(; _itr != _end; ++_itr)
	{
		qdGridZone* ptrZone = *_itr;
		std::vector<Vect2s>::const_iterator cur = ptrZone->get_contour().begin(),
			last = ptrZone->get_contour().end();
		
		std::vector<Vect2s>::const_iterator next = cur;
		++next;
		
		for(; next != last; ++cur, ++next)
		{
			Vect2s v1 = *cur;
			Vect2s v2 = *next;
			//если до этого не совпдали, то проверяем и спрашиваем
			if (!(v1 == v2)) {
				v1.x >>= 1;
				v1.y >>= 1;
				v2.x >>= 1;
				v2.y >>= 1;
				if (v1 == v2) {
					CString strMsg;
					strMsg.Format(IDS_ZONE_VERTEX_COINCIDE, ptrZone->name());
					return (MessageBox(strMsg, NULL, 
						MB_OKCANCEL|MB_ICONWARNING) == IDOK);
				}
			}
		}
		next = ptrZone->get_contour().begin();
		Vect2s v1 = *cur;
		Vect2s v2 = *next;
		//если до этого не совпдали, то проверяем и спрашиваем
		if (!(v1 == v2)) {
			v1.x >>= 1;
			v1.y >>= 1;
			v2.x >>= 1;
			v2.y >>= 1;
			if (v1 == v2) {
				CString strMsg;
				strMsg.Format(IDS_ZONE_VERTEX_COINCIDE, ptrZone->name());
				return (MessageBox(strMsg, NULL, MB_OKCANCEL|MB_ICONWARNING) == IDOK);
			}
		}
	}

	return true;
}

//! вычисляем колличество ячеел по х и по у
void BasePlanePage::UpdateCellsCount(int iNewCellSize)
{
	if(!iNewCellSize)
		return;

	CWaitCursor __wait__;
	int new_CellSX = (m_ptrCam->get_cell_sx()*m_ptrCam->get_grid_sx())/iNewCellSize;
	int new_CellSY = (m_ptrCam->get_cell_sy()*m_ptrCam->get_grid_sy())/iNewCellSize;
	
	if (new_CellSX >2&& new_CellSY>2) {
		
		bool bDivide = iNewCellSize > m_iCellSize;
		if (bDivide) {//нам приходиться уменьшать в 2 раза координаты

			//проверяем если отрезки в контурах, которые сольются
			//справшиваем у юзера, а если не надо дальше уменьшать, 
			//то не уменьшаем
			if(!ZeroCheck())
				return;
		}
		
		// Убездаемся, что числа четные
		new_CellSX += round(fmod(new_CellSX, 2.f));
		new_CellSY += round(fmod(new_CellSY, 2.f));

		m_iCellSX = new_CellSX;
		m_iCellSY = new_CellSY;
		m_iCellSize = iNewCellSize;
		
		UpdateData(FALSE);

		apply_grid_param();

		SelectZones(false);
		if (bDivide) 
			DivideZones();
		else
			MultZones();
		
		CalcGridZonesNodesScrCoords();

		SelectZones(true);
		redraw();
	}
}


void BasePlanePage::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (nSBCode == SB_ENDSCROLL) 
	{
		int nCtrlID = pScrollBar->GetDlgCtrlID();
		switch(pScrollBar->GetDlgCtrlID()) {
		case IDC_SPIN1:
		case IDC_SPIN2:
		case IDC_SPIN3:
		case IDC_SPIN4:
		case IDC_SPIN5:
			OnGridLayoutParamsChange();
			break;
		}
	}
	CBasePage::OnVScroll(nSBCode, nPos, pScrollBar);
}


BOOL BasePlanePage::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN){
		CWnd* pfoc = GetFocus();
		if (pfoc){ 
			switch(pMsg->wParam) {
			case VK_RETURN:
				{
					switch(pfoc->GetDlgCtrlID()) {
					case IDC_EDIT_ROTATEX:
					case IDC_EDIT_ROTATEY:
					case IDC_EDIT_ROTATEZ:
					case IDC_EDIT_Z:
					case IDC_EDIT_FOCUS:
					case IDC_EDIT_SCALE_POW:
					case IDC_EDIT_Z_OFFSET:
					{
						CWaitCursor wait;
						OnGridLayoutParamsChange();
						return TRUE;
					}
					}
				}
			}
		}
	}
	return CBasePage::PreTranslateMessage(pMsg);
}

//! Для всех зон сцены вычисляет экранные координаты узлов
void BasePlanePage::CalcGridZonesNodesScrCoords()
{
	m_mapZoneScrCoords.clear();
	qdGridZoneList const& lst = m_ptrScene->grid_zone_list();
	std::for_each(lst.begin(), lst.end(), 
		boost::bind(BasePlanePage::CalcGridZoneNodesScrCoords, this, _1));
}

/*!
  Переводит индексы ячейки в экранные координаты
*/

class cell_idx_to_scr{
	qdCamera const* const pcam_;
	std::vector<Vect2s> & v_;
public:
	cell_idx_to_scr(qdCamera const* pcam, std::vector<Vect2s>& v):
	  pcam_(pcam)
	  ,v_(v){}
	void operator()(Vect2s const& v) const{
		v_.push_back(pcam_->global2scr(pcam_->get_cell_coords(v)));
	}
};

//! Для зоны вычисляет экранные координаты всех ее узлов
void BasePlanePage::CalcGridZoneNodesScrCoords(qdGridZone const* pzone){
	qdCamera const* const pcam = m_ptrScene->get_camera();
	std::vector<Vect2s> const& cntr = pzone->get_contour();

	coords_vector_t& v = m_mapZoneScrCoords[pzone];
	v.reserve(cntr.size());
	std::copy(cntr.begin(), cntr.end(), 
		boost::make_function_output_iterator(cell_idx_to_scr(pcam, v)));
}

//! Для всех зон по экранным координатам узлов вычисляет индексы ячеек
void BasePlanePage::RecalcGridZonesNodes()
{
	CoordsMap_t::const_iterator i = m_mapZoneScrCoords.begin(), 
								e = m_mapZoneScrCoords.end();
	for(;i != e; ++i)
		RecalcGridZoneNodes(const_cast<qdGridZone*>(i->first), i->second);
}

/*!
  Экранные коородинаты переводит в индексы ячейки и добавляет из к указанной зоне
*/

class scr_to_cell_idx{
	qdCamera const* const pcam_;
	qdGridZone *pzone_;
public:
	scr_to_cell_idx(qdCamera const* pcam, qdGridZone* pzone):pcam_(pcam), pzone_(pzone)
	{}
	void operator()(Vect2s const& v) const{
		pzone_->add_contour_point(pcam_->get_cell_index(pcam_->scr2plane(v), false));
	}
};

//! Для зоны по экранным координатам узлов вычисляет индексы ячеек
void BasePlanePage::RecalcGridZoneNodes(qdGridZone* pzone, 
										coords_vector_t const& vec)
{
	pzone->set_state(false);
	pzone->clear_contour();
	std::copy(vec.begin(), vec.end(), boost::make_function_output_iterator(
			scr_to_cell_idx(m_ptrScene->get_camera(), pzone)));
	pzone->update_contour();
	pzone->set_state(true);
}

void BasePlanePage::OnDeltaposSpinScalePow(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	if (!UpdateData())
		return;

	scalePow_ += pNMUpDown->iDelta/10.f;

	UpdateData(FALSE);
	SetModified();
	*pResult = 0;
}

void BasePlanePage::OnDeltaposSpinZOffset(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	if (!UpdateData())
		return;
	zOffset_ += pNMUpDown->iDelta/10.f;

	UpdateData(FALSE);
	SetModified();
	*pResult = 0;
}
