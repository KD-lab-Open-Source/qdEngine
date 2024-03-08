/********************************************************************
	created:	2003/07/29
	created:	29:7:2003   12:20
	filename: 	d:\Projects\Quest\QuestEditor\MakeLinkActive.h
	file path:	d:\Projects\Quest\QuestEditor
	file base:	MakeLinkActive
	file ext:	h
	author:		Илюха
	
	purpose:	делаем указанный линк активным
*********************************************************************/
#pragma once

class qdTriggerLink;
class qdTriggerElement;
class TEBaseWorkMode;

/**
	Делает указанный линк активным. Активный линк имеет якоря на концах
 */
class MakeLinkActive
{
public:
	MakeLinkActive(TEBaseWorkMode* pmode, 
					qdTriggerElementPtr const& pLinkChild,
					qdTriggerElementPtr const& pLinkOwner);
	~MakeLinkActive();
	bool operator()();
	static bool run(TEBaseWorkMode* pmode, 
					qdTriggerElementPtr const& pLinkChild,
					qdTriggerElementPtr const& pLinkOwner);
private:
	/// Владелец
	qdTriggerElementPtr m_pLinkOwner;
	/// Подчиненный
	qdTriggerElementPtr m_pLinkChild;
	/// РР, в котором эта операция выполняется
	TEBaseWorkMode* m_pMode;
};
