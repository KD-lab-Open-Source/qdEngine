#pragma once

class qdGameObjectAnimated;
class qdGameObjectState;
class qdCamera;

///  \brief ������� ����� �������, � ���������� ����������� �������� ���, 
/// ����� �������� ��������� ��������� ���������� �� ��� �� �����
class MoveObjectCenter
{
public:
	MoveObjectCenter(void);
	~MoveObjectCenter(void);

	void operator()(qdGameObjectAnimated* pobj, 
		qdGameObjectState const* pLeadingState,
		const Vect2i& off, qdCamera const* pcam) const;
};
