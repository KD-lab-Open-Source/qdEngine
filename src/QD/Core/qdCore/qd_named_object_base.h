#ifndef __QD_NAMED_OBJECT_BASE_H__
#define __QD_NAMED_OBJECT_BASE_H__

//! ������� ������������� ������.
class qdNamedObjectBase
{
public:
	
	qdNamedObjectBase();
	qdNamedObjectBase(const qdNamedObjectBase& obj);
	virtual ~qdNamedObjectBase() = 0;

	qdNamedObjectBase& operator = (const qdNamedObjectBase& obj);

	//! ���������� ��� �������.
	const char* name() const { if(!name_.empty()) return name_.c_str(); return NULL; }
	//! ������������� ��� �������.
	void set_name(const char* p){ if(p) name_ = p; else name_.clear(); }

private:

	//! ��� �������.
	std::string name_;
};

#endif /* __QD_NAMED_OBJECT_BASE_H__ */

