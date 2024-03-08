#ifndef __QD_RESOURCE_H__
#define __QD_RESOURCE_H__

//! ������� ����� ��� ������� ��������.
/**
��������, ����� � �.�.
*/
class qdResource
{
public:
	//! ������� ������.
	enum file_format_t {
		//! �������� - .qda
		RES_ANIMATION,
		//! ������ - .tga
		RES_SPRITE,
		//! ���� - .wav
		RES_SOUND,
		//! ������������ ������
		RES_UNKNOWN
	};

	qdResource();
	qdResource(const qdResource& res);
	virtual ~qdResource();

	qdResource& operator = (const qdResource& res);
	
	//! ��������� � ������ ������ �������.
	virtual bool load_resource() = 0;
	//! ��������� �� ������ ������ �������.
	virtual bool free_resource() = 0;

	//! ������������� ��� �����, � ������� �������� ������ �������.
	virtual void set_resource_file(const char* file_name) = 0;
	//! ���������� ��� �����, � ������� �������� ������ �������.
	/**
	���� ��� �� ������, ������ ����������� NULL.
	*/
	virtual const char* resource_file() const = 0;

	//! ���������� true, ���� ������ ������� ��������� � ������.
	bool is_resource_loaded() const { return is_loaded_; }

	static file_format_t file_format(const char* file_name);

#ifdef __QD_DEBUG_ENABLE__
	virtual unsigned resource_data_size() const = 0;
#endif

protected:

	//! ������������� ��� ��������� ����, ������������ ��� ������ ������� ��������� � ������.
	void toggle_resource_status(bool st = true){ is_loaded_ = st; }

private:

	//! ����� true, ���� ������ ������� ��������� � ������.
	bool is_loaded_;
};

#ifdef __QD_DEBUG_ENABLE__
class qdResourceInfo
{
public:
	qdResourceInfo(const qdResource* res = NULL,const qdNamedObject* owner = NULL);
	qdResourceInfo(const qdResourceInfo& inf) : resource_(inf.resource_), data_size_(inf.data_size_), resource_owner_(inf.resource_owner_) { }
	~qdResourceInfo();

	qdResourceInfo& operator = (const qdResourceInfo& inf){
		if(this == &inf) return *this;

		resource_ = inf.resource_;
		resource_owner_ = inf.resource_owner_;
		data_size_ = inf.data_size_;

		return *this;
	}

	bool operator < (const qdResourceInfo& inf) const { return data_size_ < inf.data_size_; }

	unsigned data_size() const { return data_size_; }

	qdResource::file_format_t file_format() const;

	bool write(XStream& fh,int line_class_id = -1) const;

private:
	unsigned data_size_;
	const qdResource* resource_;
	const qdNamedObject* resource_owner_;
};

typedef std::vector<qdResourceInfo> qdResourceInfoContainer;

#endif

#endif /* __QD_RESOURCE_H__ */

