#pragma once

class qdTriggerProfiler;
/**
	������ �� ���������� ���������� � ����
 */
class ProfilerDataToFile
{
	ProfilerDataToFile(void);
	~ProfilerDataToFile(void);
public:
	/**
		����������� ��� �����.
		���������� ���� ������
	 */
	static bool run(qdTriggerProfiler const& profiler);
};
