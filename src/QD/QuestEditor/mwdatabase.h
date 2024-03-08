/********************************************************************
	created:	2002/11/06
	created:	6:11:2002   11:37
	filename: 	d:\projects\quest\questeditor\mwdatabase.h
	file path:	d:\projects\quest\questeditor
	file base:	mwdatabase
	file ext:	h
	Powerd by:	Илюха
	
	purpose:	базовый класс данных для визардов
*********************************************************************/

#ifndef __MW_DATA_BASE_H__

#define __MW_DATA_BASE_H__

class MWDataBase
{
public:
	bool changed() const{
		return m_bChanged;
	}
	void change(){
		m_bChanged = true;
	}
private:
	bool m_bChanged;//были ли изменены данные
};

typedef std::auto_ptr<MWDataBase> MWDataBasePtr;

#endif//__MW_DATA_BASE_H__