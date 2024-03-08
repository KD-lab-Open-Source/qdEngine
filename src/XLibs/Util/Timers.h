#ifndef	__DURATION_TIMER_H__
#define __DURATION_TIMER_H__

#include "SynchroTimer.h"

class Archive;

extern SyncroTimer global_time;	// ����������������� ��������������� ������
extern SyncroTimer frame_time; // ������������������� ����������������� ������
extern SyncroTimer scale_time; // ������������������� ���������� ������

//////////////////////////////////////////////////////////////////
//			������� ��� ����������������� ������
//////////////////////////////////////////////////////////////////
class LogicTimer {
public:
	LogicTimer() { startTime_ = 0; }

	void start(time_type delay = 0) { startTime_ = timer() + delay; } // � ������������
	void stop() { startTime_ = 0; }
	void pause() { if(startTime_) startTime_ += timer().delta(); } // �������� ������ ����� ��� ����������� �����

	// ����� ����� startTime_, �� startTime_ - 0
	time_type time() const { return startTime_ && timer() >= startTime_ ? timer() - startTime_ + 1 : 0; }

	// ����� �� startTime_ (�����������), ����� startTime_ 0
	time_type timeRest() const { return startTime_ > timer() ? startTime_ - timer() : 0; }

	bool started() const { return startTime_ > 0; }
	bool busy() const { return timer() <= startTime_; }
	bool finished() const { return startTime_ && timer() >= startTime_; }

	void serialize(Archive& ar);

protected:
	time_type startTime_;

	static const SyncroTimer& timer() { return global_time; }
};

class InterpolationLogicTimer : public LogicTimer {
public:
	InterpolationLogicTimer() { durationInv_ = 0; }
	void start(time_type duration);
	float factor() const; // [0..1]
	void serialize(Archive& ar);
private: 
	float durationInv_;
};

//////////////////////////////////////////////////////////////////
//			������� ��� �������
//////////////////////////////////////////////////////////////////
class GraphicsTimer {
public:
	GraphicsTimer() { startTime_ = 0; }

	void start(time_type delay = 0) { startTime_ = timer() + delay; } // � ������������
	void stop() { startTime_ = 0; }
	void pause() { if(startTime_) startTime_ += timer().delta(); } // �������� ������ ����� ��� ����������� �����

	// ����� ����� startTime_, �� startTime_ - 0
	time_type time() const { return startTime_ && timer() >= startTime_ ? timer() - startTime_ + 1 : 0; }

	// ����� �� startTime_ (�����������), ����� startTime_ 0
	time_type timeRest() const { return startTime_ > timer() ? startTime_ - timer() : 0; }

	bool started() const { return startTime_ > 0; }
	bool busy() const { return timer() <= startTime_; }
	bool finished() const { return startTime_ && timer() >= startTime_; }

	void serialize(Archive& ar);

protected:
	time_type startTime_;

	static const SyncroTimer& timer() { return frame_time; }
};

class InterpolationGraphicsTimer : public GraphicsTimer {
public:
	InterpolationGraphicsTimer() { durationInv_ = 0; }
	void start(time_type duration);
	float factor() const; // [0..1]
	void serialize(Archive& ar);
private: 
	float durationInv_;
};

#endif // __DURATION_TIMER_H__