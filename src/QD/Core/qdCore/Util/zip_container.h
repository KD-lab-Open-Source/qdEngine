
#ifndef __ZIP_CONTAINER_H__
#define __ZIP_CONTAINER_H__

//#include <hash_map>

//#define _ZIP_CONTAINER_DEBUG_

//! ��������� �������� �� ������ zip-������ ��� �������.
class zipContainer
{
public:
	zipContainer();
	~zipContainer();

	//! ��������� �����.
	bool open_container(const char* file_name,bool load_idx = true);
	//! ��������� �����.
	bool close_container();

	//! ��������� ���� �� ������ � ������ file_name.
	bool open_file(const char* file_name,XStream& fh);

	bool is_open(){ return stream_.isOpen(); }

	//! ������ ������� ������.
	bool save_index(XStream& fh) const;
	//! �������� ������� ������.
	bool load_index(XStream& fh);

private:

	XStream stream_;
	std::string stream_file_name_;

	struct file_info {
		unsigned offset;
		unsigned size;

		file_info(unsigned offs,unsigned sz) : offset(offs), size(sz) { }
	};

	typedef std::hash_map<std::string,file_info> file_map_t;
	file_map_t file_map_;

	const file_info* find_file(const char* file_name) const;
	static const char* convert_file_name(const char* file_name);

	long index_offset_;
	long index_size_;

	bool find_index();

#ifdef _ZIP_CONTAINER_DEBUG_
	void dump_info();
#endif	
};

#endif // __ZIP_CONTAINER_H__
