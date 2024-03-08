
#pragma	once


class qdGameObjectState;
/// \brief 	���������, ������� ��������� �����(������ ����� �������), �������� ���������
/// �������� ���������, �������������� �������� ��� �������������� �� ���������
struct IRequestingStateEdit
{
	/// ���������� ������������� ���������
	virtual qdGameObjectState* GetState() = 0;
	/// ����������, ����� �������� ���������
	virtual void EndStackState() = 0;
	/// ����������, ����� �������������� �������� ���������
	virtual void EndEditStateDims() = 0;
	/// ����������, ����� �������������� �� ���������
	virtual void EndEditCoordAnimation() = 0;
};