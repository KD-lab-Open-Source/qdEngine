#include "StdAfx.h"
#include "kdw/ContentUtil.h"
#include "kdw/FileDialog.h"
#include "kdw/Serialization.h" // для TRANSLATE
#include "FileUtils/FileUtils.h"
#include "XTL/StaticMap.h"
#include <windows.h>

namespace kdw{

GetAllTextureNamesFunc getAllTextureNamesFunc;

std::string selectAndCopyResource(const char* resourceHomePath, const char** filter, const char* defaultName, const char* title, Widget* parent)
{
	static StaticMap<std::string, std::string> initialDirs_;

    std::string initial_dir = resourceHomePath;
	if(initialDirs_.find(resourceHomePath) != initialDirs_.end())
		initial_dir = initialDirs_[resourceHomePath];

    string result;
	kdw::FileDialog fileDialog(parent, false, filter, initial_dir.c_str(), defaultName);

	CurrentDirectorySaver currentDir;
    if(fileDialog.showModal() && isFileExists(fileDialog.fileName()) ){
		currentDir.restore();
        string tmp = fileDialog.fileName();
        string path2file = extractFilePath(fileDialog.fileName());
        string fileName = extractFileName(fileDialog.fileName());

        char fileFullPath[_MAX_PATH];
        char homeFullPath[_MAX_PATH];

        if(_fullpath(fileFullPath, path2file.c_str(), _MAX_PATH) == 0)
            return result; // ""
        if(_fullpath(homeFullPath, resourceHomePath, _MAX_PATH) == 0)
            return result; // ""

        result = resourceHomePath;
        if(!result.empty())
            result += "\\";
        result += fileName;
        
        int filePathLen = strlen(fileFullPath);
        int homePathLen = strlen(homeFullPath);

        if(homePathLen <= filePathLen && strnicmp(fileFullPath, homeFullPath, homePathLen) == 0){
			initialDirs_[resourceHomePath] = fileFullPath;
            result = resourceHomePath;
            result += std::string(fileFullPath + homePathLen, fileFullPath + filePathLen);
            result += "\\";
            result += fileName;
            return result; //ok(файл уже во внутренних ресурсах)
		}
        //копирование ресурса
        if(!CopyFile(fileDialog.fileName(), result.c_str(), FALSE)){
            result.clear();//err ошибка копирования во внутренние ресурсы
        }
    }
    return result;
}


std::string selectAndCopyModel(const char* resourceHomePath, const char** filter, const char* defaultName, const char* title, Widget* parent)
{
	static StaticMap<std::string, std::string> initialDirs_;

    std::string initial_dir = resourceHomePath;
	if(initialDirs_.find(resourceHomePath) != initialDirs_.end())
		initial_dir = initialDirs_[resourceHomePath];

    string result;
	kdw::FileDialog fileDialog(parent, false, filter, initial_dir.c_str(), defaultName);

	CurrentDirectorySaver currentDir;
    if(fileDialog.showModal() && isFileExists(fileDialog.fileName()) ){
		currentDir.restore();
        string tmp = fileDialog.fileName();
        string path2file = extractFilePath(fileDialog.fileName());
        string fileName = extractFileName(fileDialog.fileName());

        char fileFullPath[_MAX_PATH];
        char homeFullPath[_MAX_PATH];

        if(_fullpath(fileFullPath, path2file.c_str(), _MAX_PATH) == 0)
            return result;//error //"";
        if(_fullpath(homeFullPath, resourceHomePath, _MAX_PATH) == 0)
            return result;//error //"";

        result = resourceHomePath;
        if(!result.empty())
            result += "\\";
        result += fileName;
        
        int filePathLen = strlen(fileFullPath);
        int homePathLen = strlen(homeFullPath);

        if(homePathLen <= filePathLen && strnicmp(fileFullPath, homeFullPath, homePathLen) == 0){
            result = resourceHomePath;
            result += std::string(fileFullPath + homePathLen, fileFullPath + filePathLen);
            result += "\\";
            result += fileName;
            return result; //ok(файл уже во внутренних ресурсах)
        }
        //копирование моделей
        if(!CopyFile(fileDialog.fileName(), result.c_str(), FALSE)){
			XBuffer buf;
			buf < TRANSLATE("Ошибка копирования во внутренние ресурсы.\n")
				< TRANSLATE("из: ") < fileDialog.fileName() < "\n"
				< TRANSLATE("в: ") < result.c_str();
			//showMessage(buf);
            result.clear();
        }

		// копируем .furinfo, если такой имеется
		{
		std::string fileName = extractFileBase(fileDialog.fileName());
		std::string filePath = extractFilePath(fileDialog.fileName());
		std::string sourceFilename = filePath + fileName + ".furinfo";
		std::string destinationFilename = resourceHomePath;
		destinationFilename+= "\\";
		destinationFilename += fileName + ".furinfo";
		CopyFile(sourceFilename.c_str(), destinationFilename.c_str(), FALSE);
		}

        //извлечение названий текстур из модели и копирование
        vector<string> textureNames;
		xassert(getAllTextureNamesFunc);
        getAllTextureNamesFunc(fileDialog.fileName(), textureNames);
        vector<string>::iterator p;
        for(p=textureNames.begin(); p!=textureNames.end(); p++){
            tmp=resourceHomePath;
            if(!tmp.empty()) tmp+="\\";
            tmp+="Textures\\";

            string::size_type i=p->find_last_of("\\");
            if(i!=string::npos)
                tmp+=&(p->c_str()[i+1]);
            else
                tmp+=*p;

            if(!CopyFile(p->c_str(), tmp.c_str(), FALSE)){
				XBuffer buf;
				buf < TRANSLATE("Не могу скопировать текстуру\nиз: ")
								< p->c_str() < "\nв: " < tmp.c_str();
				//kdw::showMessage(buf);
				result.clear();
                break;
            }
        }
    }
    return result;
}

}
