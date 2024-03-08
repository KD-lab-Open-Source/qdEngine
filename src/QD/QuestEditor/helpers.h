
#ifndef __HELPER_HEADER_
#pragma once

typedef std::vector<Vect3f> Vect3fVect_t;

//!!!!!!!!!!!!!!!
//������� �������� �� ��, ��� ������ ����������
//� ��� �������������� �����. ������� �������� ���������
//� ����� �����, � ��� ���������� �� �������� �����.
//������������� �� ����� ���� ���������� � ��������, � ��� ��
//��������� ������ ������� �� ������(� ������, ���� ��� �����������)
namespace geom_helper{
//��������� ������� ������� � ��������� �����,
//���������� ����� ����� ����������� ��������������� ����.
//��� ���� � ������� ����������� �
bool DetectInts(const Vect3fVect_t& poly, const Vect3f& st_point, Vect3fVect_t& out);

/// ����������� �������� ������ ���� �� ��� �������������� ��� ������� len
bool cross_segms(const Vect3f& b, const Vect3f& e, const Vect3f& st_p, Vect3f& res, float len);

/// ����������� �������� � ����� ����
template<class __Vect>
bool cross_segms(const __Vect& a, const __Vect &b, 
		const __Vect& c, const __Vect& d, float* pkx = NULL, float *pky = NULL)
{
	float x0 = a.x;
	float x1 = b.x;
	float x2 = c.x;
	float x3 = d.x;
	
	float y0 = a.y;
	float y1 = b.y;
	float y2 = c.y;
	float y3 = d.y;
	
	if(abs(x1 - x0) < 1e-8)//������� ������������
	{
		if(abs(x3 - x2) < 1e-8)//������� //
			return false;
		
		//���� �����
		float del = (x0 - x2)/(x3-x2);
		if(del < 0.f || del > 1.f)
			return false;
		if (pky) 
			*pky = del;
		//���� �����
		del = (y2 - y0 + (y3 - y2)*del)/(y1 - y0);
		if(del < 0.f || del > 1.f)
			return false;
		
		if (pkx) 
			*pkx = del;
		return true;
	}
	if(abs(y3 - y2) < 1e-8)//������� ��������������
	{
		if(abs(y1 - y0) < 1e-8)//�������� //
			return false;
		//���� �����
		float del = (y2 - y0)/(y1 - y0);		
		if(del < 0.f || del > 1.f)
			return false;

		if (pkx) 
			*pkx = del;

		//���� �����
		del = (x0 - x2 + (x1 - x0)*del)/ (x3 - x2);
		if(del < 0.f || del > 1.f)
			return false;
		if (pky) 
			*pky = del;
		return true;
	}
	//��������
	float del = y3-y2 - (y1-y0)*(x3-x2)/(x1-x0);
	if(abs(del) < 1e-8)//������� �����������
		return false;
	
	//���� �����
	del = (y0 - y2 + (y1 - y0)* (x2 - x0)/(x1 - x0))/del;
	if(del < 0.f || del > 1.f)
		return false;

	if (pky) 
		*pky = del;
	
	//���� �����
	del = (x2 - x0 + (x3 - x2)*del)/(x1 - x0);
	if(del < 0.f || del > 1.f)
		return false;
	
	if (pkx) 
		*pkx = del;
	return true;
}
}
#endif