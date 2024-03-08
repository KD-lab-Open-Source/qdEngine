#pragma once

/********************************************************************
	created:	2002/04/17
	created:	17:4:2002   18:20
	filename: 	D:\Projects\Quest\QuestEditor\RelativePath.h
	file path:	D:\Projects\Quest\QuestEditor
	file base:	RelativePath
	file ext:	h
	author:		Дмитрий Поскряков
	
	purpose:	Вычисление относительного пути
*********************************************************************/

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//если текущий путь не содержит имени файла, то он 
//должен быть завершен слешем!!!!

namespace r_p{
inline void DecompositePath(const CString& full_path,CStringArray& ar);
/// Состаляет относительный путь
/**
	За более подробными разъяснениями обращаться к Диме Поскрякову, как к автору 
	данных строк
 */
inline
bool RelativePath(const CString &full_path,const CString& current_dir,CString& out)
{
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	_splitpath( full_path, drive, dir, fname, ext );
	CStringArray full,current;
	DecompositePath(full_path,full);
	DecompositePath(current_dir,current);
	
	out=_T("");
	if(full[0].CompareNoCase(current[0])!=0)
		return false;
	
	int maxi=std::min(full.GetSize(),current.GetSize());

	int ib = maxi;
	for(int i=1;i<maxi;i++)
	{
		if(full[i].CompareNoCase(current[i])!=0)
		{
			ib = i;
			break;
		}
	}
	
	
	for(int i=ib;i<current.GetSize();i++)
		out+= _T("..\\");
	for(int i=ib;i<full.GetSize();i++)
		out+=full[i]+_T("\\");
	out+=fname;
	out+=ext;
/*	
#ifdef _DEBUG
	{
		char path_buffer[_MAX_PATH];
		strcpy(path_buffer,current_dir);
		char* p=_fullpath(path_buffer,out,_MAX_PATH);
		ASSERT(p);
		ASSERT(full_path.CompareNoCase(p)==0);
	}
#endif _DEBUG
*/	
	return true;
}
/// Разбивает путь на список имен каталогов 
/**
За более подробными разъяснениями обращаться к Диме Поскрякову, как к автору 
данных строк
*/
inline
void DecompositePath(const CString &full_path,CStringArray& ar)
{
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	_splitpath( full_path, drive, dir, fname, ext );
	if(*drive)
		ar.Add(drive);
	
	char* p;
	for(p=dir;*p;++p)
		if(*p=='/')
			*p='\\';
			
	char* pbegin=dir;
	while(*pbegin)
	{
		while(*pbegin=='\\')
			++pbegin;
		p=pbegin;
		while(*p&&*p!='\\')
			++p;
		if(*p)
		{
			*p=0;
			++p;
		}
		if(*pbegin)
			ar.Add(pbegin);
		pbegin=p;
	}

}

}