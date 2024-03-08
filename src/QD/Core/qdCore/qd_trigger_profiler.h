
#ifndef __QD_TRIGGER_PROFILER_H__
#define __QD_TRIGGER_PROFILER_H__

#ifndef _FINAL_VERSION
#define __QD_TRIGGER_PROFILER__
#endif

#ifdef __QD_TRIGGER_PROFILER__

#include <vector>

class XStream;

class qdTriggerElement;
class qdTriggerLink;
class qdTriggerChain;

#ifdef _QUEST_EDITOR
#include <boost/intrusive_ptr.hpp>
typedef boost::intrusive_ptr<qdTriggerElement> qdTriggerElementPtr;
typedef boost::intrusive_ptr<qdTriggerElement const> qdTriggerElementConstPtr;
#else
typedef qdTriggerElement* qdTriggerElementPtr;
typedef qdTriggerElement const* qdTriggerElementConstPtr;
#endif // _QUEST_EDITOR

//! ������ � ���� ���������� ���������.
class qdTriggerProfilerRecord
{
public:
	enum event_t {
		//! ��������� ��������� �������� ��������
		ELEMENT_STATUS_UPDATE,
		//! ��������� ��������� ����� ��������
		PARENT_LINK_STATUS_UPDATE,
		//! ��������� ��������� ����� ��������
		CHILD_LINK_STATUS_UPDATE,
	};

	qdTriggerProfilerRecord();
	qdTriggerProfilerRecord(unsigned int tm,event_t ev,const qdTriggerChain* trigger,int el_id,int lnk_id,int st);
	qdTriggerProfilerRecord(const qdTriggerProfilerRecord& rec);
	~qdTriggerProfilerRecord();

	qdTriggerProfilerRecord& operator = (const qdTriggerProfilerRecord& rec);

	bool save(XStream& fh) const;
	bool load(XStream& fh);

	//! ����� ������� (� ������������� �� ������ ����������).
	unsigned int time() const { return time_; }
	//! ������������� �������.
	event_t event() const { return event_; }
	//! ������������� �������� (����� �������� � ������ ����-����������).
	int trigger_id() const { return trigger_id_; }
	//! ������������� �������� ��������.
	int element_id() const {  return element_id_; }
	//! ������������� ����� ��������.
	int link_id() const { return link_id_; }
	//! ����� ��������� ��������/�����.
	int status() const { return status_; }

private:

	//! ����� ������� (� ������������� �� ������ ����������).
	unsigned int time_;

	//! ������������� �������.
	event_t event_;
	
	//! ������������� �������� (����� �������� � ������ ����-����������).
	int trigger_id_;
	//! ������������� �������� ��������.
	int element_id_;
	//! ������������� ����� ��������.
	int link_id_;
	//! ����� ��������� ��������/�����.
	int status_;
};

//! ��������� ���������.
class qdTriggerProfiler
{
public:
	//! ������ �������� ������ ����������.
	enum {
		//! ���������� ����� �������
		PROFILER_TEXT_TIME		= 0x01,
		//! ���������� ��� ��������
		PROFILER_TEXT_TRIGGER_NAME	= 0x02,
		//! ���������� ��� �����
		PROFILER_TEXT_SCENE_NAME	= 0x04
	};

	~qdTriggerProfiler();

	bool save_to_work_file() const;
	bool load_from_work_file();

	void add_record(const qdTriggerProfilerRecord& rec){ if(is_logging_enabled_ && !is_read_only_) records_.push_back(rec); }

	int num_records() const { return records_.size(); }

	bool is_logging_enabled() const { return is_logging_enabled_; }
	void enable(){ is_logging_enabled_ = true; }
	void disable(){ is_logging_enabled_ = false; }

	void set_read_only(bool v){ is_read_only_ = v; }
	bool is_read_only() const { return is_read_only_; }

	typedef std::vector<qdTriggerProfilerRecord> record_container_t;
	typedef record_container_t::const_iterator record_iterator_t;

	record_iterator_t records_begin() const { return records_.begin(); }
	record_iterator_t records_end() const { return records_.end(); }
	record_iterator_t get_record(int record_num = 0) const { return records_.begin() + record_num; }

	bool evolve(int record_num) const;

	static qdTriggerProfiler& instance();

	static const char* const activation_comline(){ return activation_comline_; }

	static const char* record_text(const qdTriggerProfilerRecord& rec,char separator = ' ');

	static int record_text_format(){ return record_text_format_; }
	static void set_record_text_format(int fmt){ record_text_format_ = fmt; }

	const char* work_file() const { return work_file_.c_str(); }
	void set_work_file(const char* fname);

private:

	qdTriggerProfiler();
	
	record_container_t records_;

	bool is_logging_enabled_;
	bool is_read_only_;

	std::string work_file_;

	static int record_text_format_;

	static const char* const activation_comline_;

	static const char* element_text(qdTriggerElementPtr el);

	static qdTriggerElementPtr get_record_element(const qdTriggerProfilerRecord& rec);
	static qdTriggerLink* get_record_link(const qdTriggerProfilerRecord& rec);
	static qdTriggerChain*	get_record_trigger(const qdTriggerProfilerRecord& rec);
};

#endif /* __QD_TRIGGER_PROFILER__ */

#endif /* __QD_TRIGGER_PROFILER_H__ */

