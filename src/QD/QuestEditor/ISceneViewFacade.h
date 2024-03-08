
#pragma once

/*!
��������� ��� ��������� ��������� �����. �.�. ��� ������ � ������� ���������
*/
enum STATE_SUBTOOLS_ENUM
{
	SSTE_NONE, ///< ������
	SSTE_CREATE_ZONE,///< ������� ����
	SSTE_EDIT_ZONE,///< ����������� ����
	SSTE_EDIT_ZONE_AND_SHOW_ALL,///< ����������� � ������������
	SSTE_CREATE_MASK,///< ������� �����
	SSTE_EDIT_MASK,///< ������������� �����
	SSTE_EDIT_DIMS,///< �������������� ������������
	SSTE_EDIT_DIR,///< �������������� �����������
	SSTE_EDIT_STATE_POS,///< ����������� ������� ���������
	SSTE_EDIT_STATE_CENTER_OFFSET,///< ����������� �������� ��������
};

enum eRedrawMode {RM_REDRAW_CONTENT, RM_NO_CONTENT_REDRAW};

/*!
����� ������ ��������� �����
*/
enum VIEW_STATE
{
	VS_NONE,///< ������ �� ������ � �� ������
	VS_EDIT_SCENE,///< ����������� �����. ������ ��������� ��������, ������� �� � �.�.
	VS_EDIT_NET,///< ����������� �����(������� � ����������� ����)
	VS_EDIT_COORD_ANIM,///< ����������� ������������ ��������
	VS_EDIT_GAME_AREA,///< ����������� ������� �������
	VS_EDIT_SCREEN_LAYOUT, ///< ����������� ����� �����
	VS_EDIT_MASK,///< ����������� �����
	VS_EDIT_ANIMATED,///< ����������� ����� � ����������
	VS_EDIT_STACK_STATE,///< ������� ���������
	VS_EDIT_STATE,///< ����������� �������
	VS_EDIT_INVENTORY,///< ����������� ���������
	VS_EDIT_INTERFACE,//! ����������� ���������
};

interface IProjectFacade;
class qdCoordsAnimation;
class StateCoordsStacker;
class qdGameObjectState;
class CoordAnimDlg;
interface IWorkOwner;
//! ��������� ������� � ��������� �����
interface ISceneViewFacade
{
	virtual void setProject(IProjectFacade* project) = 0;
	virtual IProjectFacade* getProject() const = 0;
	//! ������������� �������� CChildView::m_bDrawNetProjs
	virtual void SetDrawNetProjs(bool bval) = 0;
	//! ���������� �������� CChildView::m_bDrawNetProjs
	virtual bool GetDrawNetProjs() const = 0;
	//! ������������� �������� CChildView::m_bDrawBounds
	virtual void SetDrawBounds(bool bval) = 0;
	//! ���������� �������� CChildView::m_bDrawBounds
	virtual bool GetDrawBounds() const = 0;

	//! �������� ��������� ����
	virtual void OnCreateGridZone() = 0;
	//! �������� ������������� ����
	virtual void OnEditGridZone(class qdGridZone* ptrZone, IWorkOwner* workOwner) = 0;
	//! �� ����������� �������������� �������� ����
	virtual void OnAbortGridZoneEdit(bool bSave) = 0;

	//! �� ������ �������������� �������� ���������
	virtual void OnEditStateDims(class StateBoundDlg* pdlg, 
								class qdGameObjectState* pstate) = 0;
	//! �� ����������� ������������� ��������
	virtual void OnEndEditStateDims(bool bSave) = 0;

	//! ������������� ���������(������� �����) ��������� �����
	virtual void SetViewState(VIEW_STATE eVal) = 0;
	//! ���������� ������� �����
	virtual VIEW_STATE GetViewState() = 0;
	//! ������������� ��������� ��� �������� ���������(������ ������)
	virtual void SetSubTools(STATE_SUBTOOLS_ENUM ssteVal) = 0;
	//! ���������� ��������� ��� ������ ������
	virtual STATE_SUBTOOLS_ENUM GetSubTools() = 0;
	//! ����������� ������ ��� ���, ����� � ���� ������ ������� ����� � ��������
	virtual bool AdjustBitmapSize() = 0;

	virtual void reset() = 0;

	virtual void Redraw(eRedrawMode emode) = 0;
	virtual void SelectObj( class qdGameObject* obj) = 0;

	//! �� ������ �������� �����
	virtual bool OnCreateStateMask(qdGameObject* ptrGO) = 0;
	//! �� ������ �������������� �����
	virtual bool OnEditStateMask(qdGameObjectStateMask* ptrMask) = 0;
	//! �� ��������� �������������� �����
	virtual void OnAbortStateMaskEdit(bool bSave) = 0;
	//! ������������� �������� ���������
	virtual void SetActiveState(qdGameObjectState* pstate) = 0;
	//! ������������� �������� ���������
	virtual void SelectInventory(qdInventory* ptrInv) = 0;

	//! ������ ������������ �����
	virtual void PlayStopVideo(qdVideo* video) = 0;
	//! ��������� ������������ �����
	virtual void StopVideo(qdVideo* video) = 0;
	//! ������������� �� ����� � ������� ������
	virtual bool isVideoPlaying() const = 0;

	//! ������ ������������ ��������
	virtual void StartAnimation(qdGameObjectState* state) = 0;
	//! ��������� ������������ ��������
	virtual void StopAnimation(qdGameObjectState* state) = 0;
	virtual void PauseAnimation() = 0;

	virtual void activate() = 0;
	virtual void deactivate() = 0;

	virtual void BeginEditCoordAnim(qdCoordsAnimation* pca) = 0;
	virtual qdCoordsAnimation& GetCoordsAnimation() = 0;
	virtual void UpdateCoordsAnimation() = 0;
	virtual void EndEditCoordAnim() = 0;

	virtual void SetStateStackerDlg(StateCoordsStacker*) = 0;
	virtual void SetStackedState(qdGameObjectState* ) = 0;

	virtual void SetCoordsAnimationDlg(CoordAnimDlg* ) = 0;

	virtual bool OnEditAnimated(qdGameObjectAnimated* ) = 0;
};