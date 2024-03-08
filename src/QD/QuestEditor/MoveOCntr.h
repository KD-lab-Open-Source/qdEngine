#pragma once

class MoveOCntr
{
public:
	MoveOCntr(Vect2s off, qdGameObjectState* pstate, bool transform_correction = true);
	~MoveOCntr(void);

	//! ��������� ������ �� ����������� ��������
	//! ���������� ���������� � �������� �������� ���������
	//! ������� ��������� ���������, ���� ����
	Vect2i operator()(const qdCamera* pcam);

private:
	Vect2i offset_;
	qdGameObjectState* pstate_;

	bool transformCorrection_;

	/// ������� ��� ���������, ����� ��������,
	/// ����� ��������������� �������� ������ �������
	void adjust_state_offsets(const Vect2i& object_offset);
};
