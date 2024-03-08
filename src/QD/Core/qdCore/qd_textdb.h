#ifndef __QD_TEXTDB_H__
#define __QD_TEXTDB_H__

/// ���� ������ � ��������.
class qdTextDB
{
public:
	qdTextDB();
	~qdTextDB();

	/// ������� ����.
	void clear(){ texts_.clear(); }

	/// ���������� ����� � ��������������� text_id.
	/**
	���� ����� �� ������ - ������ ������ ������.
	*/
	const char* getText(const char* text_id) const;

	/// ���������� ���� � ������ � ��������������� text_id.
	const char* getSound(const char* text_id) const;

	/// ���������� ����������� ������ � ��������������� text_id.
	const char* getComment(const char* text_id) const;

	/// �������� ����.
	/**
	���� clear_old_texts == true, �� ����������� � ������ ������ ���� ���������.
	� ��������� ������ ���� ������������ ������������.
	*/
	bool load(const char* file_name,const char* comments_file_name = NULL,bool clear_old_texts = true);
	bool load(XStream& fh,const char* comments_file_name = NULL,bool clear_old_texts = true);
	bool load(XZipStream& fh,const char* comments_file_name = NULL,bool clear_old_texts = true);

	typedef std::list<std::string> IdList;
	void getIdList(const char* mask, IdList& idList) const;
	bool getIdList(IdList& idList) const;
	bool getRootIdList(IdList& idList) const;

	static qdTextDB& instance();

private:

	struct qdText
	{
		qdText(const char* text,const char* snd) : text_(text), sound_(snd) { }

		std::string text_;
		std::string sound_;
#ifndef _FINAL_VERSION_
		std::string comment_;
#endif
	};

	typedef std::hash_map<std::string,qdText> qdTextMap;
	qdTextMap texts_;
};

#endif // __QD_TEXTDB_H__
