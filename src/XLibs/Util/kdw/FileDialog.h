#ifndef __KDW_FILE_DIALOG_H_INCLUDED__
#define __KDW_FILE_DIALOG_H_INCLUDED__

#include "kdw/API.h"
#include <string>
class Archive;

namespace kdw{

class Widget;
class KDW_API FileDialog{
public:
	FileDialog(kdw::Widget* owner, bool save, const char** masks, const char* startDirectory = 0, const char* startFileName = 0, bool allow_multiselect = false);
	bool showModal();
	const char* fileName() const{ return fileName_.c_str(); }
	const std::vector<std::string>& fileNames() const { return fileNames_; }

	void serialize(Archive& ar);
protected:
	HWND ownerWnd_;
	std::string startDirectory_;
	std::string startFileName_;
	std::string fileName_;
	std::vector<std::string> fileNames_;
	std::vector<std::string> masks_;
	bool save_;
	bool multiselect_;
};

}

#endif
