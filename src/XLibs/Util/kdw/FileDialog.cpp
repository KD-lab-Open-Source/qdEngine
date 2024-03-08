#include "StdAfx.h"
#include "kdw/FileDialog.h"
#include "kdw/Widget.h"
#include "kdw/Win32/Window.h"
#include <windows.h>

namespace kdw{

// COPY_PASTE
static HWND findOwner(HWND wnd){
	if(!wnd)
		return wnd;
	while(HWND parent = GetParent(wnd))
		wnd = parent;
	return wnd;
}

// COPY_PASTE
static HWND findOwner(Widget* widget){
	if(!widget)
		return 0;
	HWND parentWnd = 0;
	Win32::Window32* window = _findWindow(widget);
	ASSERT(window);
	if(window)
		return findOwner(*window);
	else
		return 0;
}

FileDialog::FileDialog(kdw::Widget* owner, bool save, const char** masks, const char* startDirectory, const char* startFileName, bool allow_multiselect)
: ownerWnd_(findOwner(owner))
, save_(save)
, startDirectory_(startDirectory ? startDirectory : "")
, startFileName_(startFileName ? startFileName : "")
, multiselect_(allow_multiselect)
{
	const char** p = masks;
	while(*p){
        masks_.push_back(*p);
		++p;
		xassert(*p);
        masks_.push_back(*p);
		++p;
	}
}

bool FileDialog::showModal()
{
	char fileName[_MAX_PATH + _MAX_FNAME * 32];
	ZeroMemory(fileName, sizeof(fileName));
	if(startFileName_ != "")
		strcpy(fileName, startFileName_.c_str());
	
	char filter[512];
	ZeroMemory(filter, sizeof(filter));
	char* p = filter;
	const char* filterEnd = filter + sizeof(filter) - 2;

	std::vector<std::string>::iterator it;
	for(it = masks_.begin(); it != masks_.end(); ++it){
		const char* mask = it->c_str();
		int len = strlen(mask);
		xassert(p + len + 1< filterEnd);
		strcpy(p, mask);
		p += len;
		*p = '\0';
		++p;
	}
	p[1] = '\0';

	OPENFILENAME openFileName;
	ZeroMemory(&openFileName, sizeof(openFileName));
	openFileName.lStructSize = sizeof(openFileName);
	openFileName.Flags = OFN_NOCHANGEDIR;
	if(!save_){
		openFileName.Flags |= OFN_PATHMUSTEXIST;
		if(multiselect_)
			openFileName.Flags |= OFN_EXPLORER | OFN_ALLOWMULTISELECT;
	}

	openFileName.hwndOwner = ownerWnd_;
	openFileName.hInstance = Win32::_globalInstance();
	openFileName.lpstrTitle = save_ ? "Сохранить файл" : "Открыть файл";
	openFileName.lpstrFilter = 0/*filter*/; 
	openFileName.nFilterIndex = 1;
	openFileName.lpstrCustomFilter = filter;
	openFileName.nMaxCustFilter = sizeof(filter) - 1;
	openFileName.lpstrFile = fileName;
	openFileName.nMaxFile = sizeof(fileName) - 1;
	openFileName.lpstrInitialDir = startDirectory_.c_str();
	if(save_){
		if(!GetSaveFileName(&openFileName))
			return false;
	}
	else{
		if(!GetOpenFileName(&openFileName))
			return false;
	}

	if(!save_ && multiselect_ && fileName[strlen(fileName) + 1]){
		XBuffer buf(_MAX_PATH);
		char* p = fileName + strlen(fileName) + 1;
		while(strlen(p)){
			buf.init();
			buf < fileName < "\\" < p;
			fileNames_.push_back(buf.c_str());
			p += strlen(p) + 1;
		}
	}
	else {
		fileNames_.push_back(fileName);
		fileName_ = fileName;
	}

	return true;
}

void FileDialog::serialize(Archive& ar)
{
	//__super::serialize(ar);
}

}

