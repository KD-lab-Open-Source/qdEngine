//////////////////////////////////////////////////////////////////
//  ������� ��� ������� �������������
//
//  1. ����� ���������� � ������������.
//
//  2. �������: 
//	��������� �������
//	�������� �������
//	�������� true-������� � "�����������"
//	���������� � ������� ���������� �������
//
//  3. ����� �������� - stop().
//
//  4. ���� ������������� (����� SyncroTimer)
//	�� clocki()
//	�� frames - � ��������� ���������������� FPS
//
//////////////////////////////////////////////////////////////////
#ifndef	__DURATION_TIMER_H__
#define __DURATION_TIMER_H__

#include "SynchroTimer.h"

extern SyncroTimer global_time;

class BaseTimer {
protected:
	time_type start_time;
public:
	BaseTimer() { start_time = 0; }
	void stop() { start_time = 0; }

	time_type get_start_time() const { return start_time; }
};

// ��������� �������
class MeasurementTimer : public BaseTimer {
public:
	void start();
	time_type operator () () const; // ����� � ������� ������
};

// ������ - ���������� � ������� ���������� �������
class DurationTimer : public BaseTimer {
public:
	void start(time_type duration);
	time_type operator () () const;  // true:  ��� start � �� ������ ����� duration, ���������� ������� �������
	int operator ! () const { return (*this)() ? 0 : 1; } 
};

// ������ - �������� �������
class DelayTimer : public BaseTimer {
public:
	void start(time_type delay);
	time_type operator () () const;  // true:  ��� start � ������ ����� delay, ���������� ����� �� ����� ��������
	int operator ! () const { return (*this)() ? 0 : 1; } 
};

// ������ - �������� true-������� 
class DelayConditionTimer : public BaseTimer {
public:
	int operator () (int condition, time_type delay);  // true: condition == true ����������� ����� delay �����. 
};

// ������ - ���������� true-������� 
class AverageConditionTimer : public BaseTimer {
public:
	int operator () (int condition, time_type delay);  // true: condition == true ����������� ����� delay. 
};

// ������ - ���������� true-������� 
class HysteresisConditionTimer : public BaseTimer {
	int turned_on;
public:
	HysteresisConditionTimer() { turned_on = 0; }
	// true: condition == true ����������� ����� on_delay, �����������, ���� condition == false ����� off_delay
	int operator () (int condition, time_type on_delay, time_type off_delay);  
	int operator () (int condition, time_type delay) { return (*this)(condition, delay, delay); }
	int operator () () { return turned_on; }
};

class InterpolationTimer : public MeasurementTimer {
public:
	void start(time_type duration);
	float operator () () const; // [0..1]
private: 
	float durationInv_;
};

///////////////////////////////////////////////////////////////////////////////////////////////
//	Inline definitions
///////////////////////////////////////////////////////////////////////////////////////////////

// ��������� �������
inline void MeasurementTimer::start()
{
	start_time = global_time();
}	
inline time_type MeasurementTimer::operator()() const
{
	return start_time ? global_time() - start_time : 0;
}

// ������ - �������� �������
inline void DelayTimer::start(time_type delay)
{
	start_time = global_time() + delay;
}	
inline time_type DelayTimer::operator () () const
{
	return start_time && global_time() > start_time ? global_time() - start_time : 0;
}
	
// ������ - �������� true-������� 
inline int DelayConditionTimer::operator () (int condition, time_type delay)
{
	if(condition){
		if(start_time){
			if(global_time() - start_time >= delay)
				return 1;
			}
		else
			start_time = global_time();
		}
	return 0;
}

// ������ - ���������� true-������� 
inline int AverageConditionTimer::operator () (int condition, time_type delay)
{
	if(condition){
		if(start_time){
			if(global_time() - start_time >= delay)
				return 1;
			}
		else
			start_time = global_time();
		}
	else 
		start_time = 0;
	return 0;
}

// ������ - ���������� � ������� ���������� �������
inline void DurationTimer::start(time_type duration)
{
	start_time = global_time() + duration;
}	
inline time_type DurationTimer::operator () () const
{
	return start_time > global_time() ? start_time - global_time() : 0;
}

// ������ - ���������� true-������� 
inline int HysteresisConditionTimer::operator () (int condition, time_type on_delay, time_type off_delay)
{
	if(turned_on && condition || !turned_on && !condition){
		start_time = 0;
		return turned_on;
		}

	if(!turned_on && condition){
		if(start_time){
			if(global_time() - start_time >= on_delay){
				turned_on = 1;
				start_time = 0;
				}
			}
		else
			start_time = global_time();
		}

	if(turned_on && !condition){
		if(start_time){
			if(global_time() - start_time >= off_delay){
				turned_on = 0;
				start_time = 0;
				}
			}
		else
			start_time = global_time();
		}

	return turned_on;
}

inline void InterpolationTimer::start(time_type duration)
{
	durationInv_ = 1/(float)duration;
	MeasurementTimer::start();
}

inline float InterpolationTimer::operator()() const 
{
	float t = MeasurementTimer::operator()()*durationInv_;
	if(t < 1.f)
		return t;
	else
		return 1.f;
}



#endif // __DURATION_TIMER_H__
