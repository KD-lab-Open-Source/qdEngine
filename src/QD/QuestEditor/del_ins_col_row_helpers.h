/********************************************************************
	created:	2003/07/30
	created:	30:7:2003   13:39
	filename: 	d:\Projects\Quest\QuestEditor\del_ins_col_row_helpers.h
	file path:	d:\Projects\Quest\QuestEditor
	file base:	del_ins_col_row_helpers
	file ext:	h
	powerd by:	�����
	
	purpose:	�������� ��� ��������� ���������� 
*********************************************************************/
namespace te_h{
/**
	���������, ��� ������ ������ �������� �� ��������� ��������� 
	� ���������� (test_for_vert_offset::idx_, test_for_vert_offset::idx_max_].
	�.�. ����������� ��������� �� ������� � ������ ������ ��������
 */
class test_for_vert_offset
{
	/// ������ ������� ����������
	int idx_;
	/// ������� ����� ����������
	int idx_max_;
public:
	test_for_vert_offset(int idx, int idx_max = std::numeric_limits<int>::max())
		:idx_(idx), idx_max_(idx_max){}
	void setIdx(int idx){
		idx_ = idx;
	}
	void setIdxMax(int idx){
		idx_max_ = idx;
	}
	/// ������ ��������
	bool operator()(qdTriggerElementConstPtr pele) const{
		int const y = pele->cell_number().y;
		return (idx_ < y && y <= idx_max_);
	}
};

/**
	���������, ��� ������ ������ �������� �� ����������� ��������� 
	� ���������� (test_for_vert_offset::idx_, test_for_vert_offset::idx_max_].
	�.�. ����������� ��������� �� ������� � ������ ������ �����
 */
class test_for_horz_offset
{
	/// ������ ������� ����������
	int idx_;
	/// ������� ����� ����������
	int idx_max_;
public:
	test_for_horz_offset(int idx, int idx_max = std::numeric_limits<int>::max()):
	  idx_(idx), idx_max_(idx_max){}
	void setIdx(int idx){
		idx_ = idx;
	}
	void setIdxMax(int idx){
		idx_max_ = idx;
	}
	bool operator()(qdTriggerElementConstPtr pele) const{
		int const x = pele->cell_number().x;
		return (idx_ < x&& x <= idx_max_);
	}
};

/**
	������� ���������� ������� �� �������� ��������
 */
class offset_ele{
	//! ��������������� ����������
	mutable CRect r_;
	//! �����
	CSize offset_;
	//! ������ ������ � ������ ������
	int fullCellSX_;
	int fullCellSY_;
public:
	offset_ele(CSize offset, int fSX, int fSY):
	  offset_(offset),fullCellSX_(fSX),fullCellSY_(fSY){}

	  void operator()(qdTriggerElementPtr ptrEle) const
	  {
		  operator()(ptrEle, offset_);
	  }

	  /**
	  	����� ���� ��������. �������� ���. ��������� ������� � ������������
		������� �����
	   */
	  VOID operator()(qdTriggerElementPtr ptrEle, const CSize &offset) const
	  {
		  r_ = ptrEle->bounding_rect();
		  r_.OffsetRect(offset);
		  ptrEle->set_bounding_rect(r_);
		  ptrEle->set_cell_number(te_h::get_cell_idx(r_.left, fullCellSX_),
			  te_h::get_cell_idx(r_.top, fullCellSY_));
	  }
};

/**
	��������� ����� �� �������� �������.
 */
class test_can_shift_element 
{
	mutable CRect rcTemp_;
	CSize offset_;
	class TEWnd * pwnd_;
public:
	//��� ����������� ����������
	typedef qdTriggerElementConstPtr argument_type;
	typedef bool result_type;

	test_can_shift_element(CSize const& offset, 
							TEWnd *pwnd):
	offset_(offset), pwnd_(pwnd){}
	/**
		����� ���� ��������. ��������. ���� ������� �� �������� ������ ����.
		���� ������, �� �������� ������� ������
	 */
	bool operator()(argument_type pele) const
	{
		rcTemp_ = pele->bounding_rect();
		rcTemp_.OffsetRect(offset_.cx, offset_.cy);
		return (!pwnd_->find_te(rcTemp_.TopLeft()));
	}
};
}