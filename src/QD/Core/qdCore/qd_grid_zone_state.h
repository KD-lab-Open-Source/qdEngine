#ifndef __QD_GRID_ZONE_STATE_H__
#define __QD_GRID_ZONE_STATE_H__

#include "xml_fwd.h"

#include "qd_conditional_object.h"

class XStream;

//! ��������� ���� �� ����� �����.
class qdGridZoneState : public qdConditionalObject
{
public:
	explicit qdGridZoneState(bool st);
	qdGridZoneState(const qdGridZoneState& st);
	~qdGridZoneState();

	qdGridZoneState& operator = (const qdGridZoneState& st);

	int named_object_type() const { return QD_NAMED_OBJECT_GRID_ZONE_STATE; }

	//! �������� ������ �� �������.
	bool load_script(const xml::tag* p);
	//! ������ ������ � ������.
	bool save_script(class XStream& fh,int indent = 0) const;

	//! �������� ��� ��������� ����.
	bool state() const { return state_; }

	qdConditionalObject::trigger_start_mode trigger_start();

	static const char* const ZONE_STATE_ON_NAME;
	static const char* const ZONE_STATE_OFF_NAME;

private:

	//! �������� ��� ��������� ����.
	bool state_;
};

#endif /* __QD_GRID_ZONE_STATE_H__ */
