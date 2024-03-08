#ifndef __XML_TAG_H__
#define __XML_TAG_H__

//! ������ XML �� ���� expat.
namespace xml {

//! XML ���.
class tag
{
public:
	typedef std::list<tag> subtags_t;
	typedef subtags_t::const_iterator subtag_iterator;
	
	//! ������ ������ ����.
	enum tag_data_format {
		//! ������ �����������
		TAG_DATA_VOID,
		//! ������ ���� short int
		TAG_DATA_SHORT,
		//! ������ ���� unsigned short int
		TAG_DATA_UNSIGNED_SHORT,
		//! ������ ���� int
		TAG_DATA_INT,
		//! ������ ���� unsigned int
		TAG_DATA_UNSIGNED_INT,
		//! ������ ���� float
		TAG_DATA_FLOAT,
		//! ��������� ������
		TAG_DATA_STRING
	};

	tag(int id = 0,tag_data_format data_fmt = TAG_DATA_VOID,int data_sz = 0,int data_offs = 0) : ID_(id), data_format_(data_fmt), data_size_(data_sz), data_offset_(data_offs), data_(NULL) { }
	tag(const tag& tg) : ID_(tg.ID_), data_format_(tg.data_format_), data_size_(tg.data_size_), data_offset_(tg.data_offset_), data_(tg.data_), subtags_(tg.subtags_) { }
	~tag(){ }

	tag& operator = (const tag& tg){
		if(this == &tg) return *this;

		ID_ = tg.ID_;
		data_format_ = tg.data_format_;
		data_size_ = tg.data_size_;
		data_offset_ = tg.data_offset_;

		subtags_ = tg.subtags_;

		return *this;
	}

	//! ���������� ������������� ����.
	int ID() const { return ID_; }

	//! ���������� ������ ������ ����.
	tag_data_format data_format() const { return data_format_; }

	//! ���������� ���������� �������� ������ ����.
	/**
	����� �������� ������ ������ � ������, ���� ��� �����
	�������� �� ������ �������� ������ � ������ - data_elemet_size().
	*/
	int data_size() const { return data_size_; }
	//! ���������� ������ �������� ������ ���� � ������.
	int data_element_size() const {
		switch(data_format_){
		case TAG_DATA_VOID:
			return 0;
		case TAG_DATA_SHORT:
			return sizeof(short);
		case TAG_DATA_INT:
			return sizeof(int);
		case TAG_DATA_UNSIGNED_INT:
			return sizeof(unsigned int);
		case TAG_DATA_FLOAT:
			return sizeof(float);
		case TAG_DATA_STRING:
			return sizeof(char);
		}
		return 0;
	}

	//! ������������� ���������� ��������� ������ ����.
	void set_data_size(int sz){ data_size_ = sz; }

	//! ���������� �������� �� ������ ���� � ������ �������.
	int data_offset() const { return data_offset_; }
	//! ������������� �������� �� ������ ���� � ������ �������.
	void set_data_offset(int off){ data_offset_ = off; }

	//! ���������� ��������� �� ������ ����.
	const char* data() const { return &*(data_ -> begin() + data_offset_); }

	//! ������������� ��������� �� ����� ������.
	void set_data(const std::vector<char>* p){ 
		data_ = p; 

		for(subtags_t::iterator it = subtags_.begin(); it != subtags_.end(); ++it)
			it -> set_data(p);
	}

	//! ������� ��������� �����.
	void clear(){ subtags_.clear(); }
	//! ��������� ��������� ���.
	/**
	���������� ������ �� ��������� ��������� ���.
	*/
	tag& add_subtag(const tag& tg){ subtags_.push_back(tg); return subtags_.back(); }
	//! ���������� true, ���� ������ ��������� ����� �� ������ .
	bool has_subtags() const { return !subtags_.empty(); }
	//! ���������� ���������� ��������� �����.
	int num_subtags() const { return subtags_.size(); }
	//! ���������� �������� ������ ������ ��������� �����.
	subtag_iterator subtags_begin() const { return subtags_.begin(); }
	//! ���������� �������� ����� ������ ��������� �����.
	subtag_iterator subtags_end() const { return subtags_.end(); }
	//! ����� ���������� ���� �� ��� ��������������.
	const tag* search_subtag(int subtag_id) const {
		for(subtag_iterator it = subtags_begin(); it != subtags_end(); ++it)
			if(it -> ID() == subtag_id) return &*it;
			
		return NULL;
	}

private:

	//! ������������� (���) ����.
	int ID_;
	//! ������ ������ ����.
	tag_data_format data_format_;
	//! ���������� ��������� ������ ����.
	int data_size_;
	//! �������� �� ������ ���� � ����� ������.
	int data_offset_;
	//! ��������� �� ������.
	const std::vector<char>* data_;
	
	//! ������ ��������� �����.
	subtags_t subtags_;
};

}; /* namespace xml */

#endif /* __XML_TAG_H__ */
