#ifndef __ZIP_CONFIG_H__
#define __ZIP_CONFIG_H__

#include "Serialization\StringTableBase.h"
#include "Serialization\StringTable.h"

class ZipConfig : public StringTableBase
{
public:
	typedef void (*ProgressCallback)(int, int, const char*);

	ZipConfig(const char* name = "");
	~ZipConfig();

	void serialize(Archive& ar);

	bool isEmpty() const;

	const char* zipName() const { return zipName_.c_str(); }
	const char* path() const { return path_.c_str(); }
	const char* filesMask() const { return filesMask_.c_str(); }
	const char* excludeFilesMask() const { return excludeFilesMask_.c_str(); }

	int compressionLevel() const { return compressionLevel_; }

	/// ������������� zip��, ���� �������� ����� ������� ������� �� ���
	static bool initArchives();
	/// ������ �������� � zip�
	static bool makeArchives(ProgressCallback progressCallback = 0);

private:

	/// ��� zip �����
	std::string zipName_;
	/// ���� � ��������, ������� ���� ��������� � zip
	/// ����������� ��������� ����� ";"
	std::string path_;
	/// ����� ����� ������, ������� ���� ��������� � zip
	/// ����������� ��������� ����� ";"
	std::string filesMask_;
	/// ����� ����� ������, ������� �� ���� ������ � zip
	/// ����������� ��������� ����� ";"
	std::string excludeFilesMask_;

	/// ������� ������, 0-9
	int compressionLevel_;
};

typedef StringTable<ZipConfig> ZipConfigTable;

#endif // __ZIP_CONFIG_H__

