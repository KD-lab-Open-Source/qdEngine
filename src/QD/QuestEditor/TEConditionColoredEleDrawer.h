/********************************************************************
	created:	2003/05/23
	created:	23:5:2003   17:04
	filename: 	d:\Projects\Quest\QuestEditor\TEConditionColoredEleDrawer.h
	file path:	d:\Projects\Quest\QuestEditor
	file base:	TEConditionColoredEleDrawer
	file ext:	h
	Powerd by:	Илюха при идейном руководстве и моральной поддержке of Scmorr
	
	purpose:	Выбираем цвета элементов в соответствии с условиями
				цвета связей в соответствии с типом
*********************************************************************/
#pragma once
#include "basetedrawer.h"
#include <vector>
#include "te_consts.h"

/**
	Перекрывает функции выбора кистей и карандашей.  
	Выбирает цвета элементов в соответствии с условиями
	цвета связей в соответствии с типом
 */
class TEConditionColoredEleDrawer :	public BaseTEDrawer
{
private:
	/**
		Пара id-условия - кисть
	 */
	typedef struct tagBRUSH_INDX_ENTRY : public std::pair<int, HBRUSH> {
		typedef std::pair<int, HBRUSH> _Base;
		tagBRUSH_INDX_ENTRY():_Base(0, NULL){}
		tagBRUSH_INDX_ENTRY(int key, COLORREF clr):_Base(key, CreateSolidBrush(clr)){}
		bool operator == (int type) const {
			return first == type;
		}
	} BRUSH_INDEX_ENTRY;

	/**
		Обертка над HPEN, чтобы он удалился при разрушении контейнера
	 */
	struct LinkPen 
	{
		HPEN pen_;
		~LinkPen();
		operator HPEN();
	};
	/**
		Обертка над HBRUSH, чтобы он удалился при разрушении контейнера
	 */
	struct Brush
	{
		HBRUSH brush_;
		~Brush();
		operator HBRUSH();
	};
//	typedef CBrush BrushVector_t[LINK_TYPES_COUNT];
//	typedef LinkPen PenVector_t[LINK_TYPES_COUNT];
	typedef Brush BrushVector_t[LINK_TYPES_COUNT];
	typedef LinkPen PenVector_t[LINK_TYPES_COUNT];
	typedef std::vector<BRUSH_INDEX_ENTRY> BrushIdx_t;
public:
	TEConditionColoredEleDrawer(void);
	~TEConditionColoredEleDrawer(void);
protected:
	virtual HBRUSH	SelectEleBackBrush(qdTriggerElementConstPtr const& pele) const;
	virtual HBRUSH	SelectLinkBrColor(qdTriggerLink const* pl) const;
	virtual HPEN	SelectLinkPenColor(qdTriggerLink const* pl) const;
protected:
	BOOL setHideObjBrush(COLORREF color);
	BOOL setHideObjBrush(HBRUSH brush);
	HBRUSH getHideObjBrush() const{
		return m_brHideObj;
	}
	BOOL setInversedCondBrush(COLORREF color);
	BOOL setInversedCondBrush(HBRUSH brush);
	HBRUSH getInversedCondBrush() const{
		return m_brInversedCond;
	}
	BOOL setDonedLinkBrush(COLORREF color);
	BOOL setDonedLinkBrush(HBRUSH brush);
	HBRUSH getDonedLinkBrush() const{
		return m_brDonedLinkBrush;
	}

	BOOL setDonedLinkPen(HPEN hpen);
	BOOL setDonedLinkPen(int nPenStyle, int nWidth, COLORREF crColor);
	HPEN getDonedLinkPen() const{
		return m_penDonedLink;
	}

	HBRUSH getLinkTypeBrush(int type) const{
		ASSERT(0<=type&&type<LINK_TYPES_COUNT);
		return m_vecLinkBrushs[type];
	}
	HPEN getLinkTypePen(int type) const{
		ASSERT(0<=type&&type<LINK_TYPES_COUNT);
		return m_vecLinkPens[type];
	}
private:
	/// создает кисти для условий
	static BrushIdx_t const initBrushIdx();
private:
	/// Список кистей для линков
	static BrushVector_t m_vecLinkBrushs;
	/// Список карандашей для линков
	static PenVector_t m_vecLinkPens;
	/// Индекс кистей для условий
	static BrushIdx_t m_BrushIdx;
	/// Кисть отрицающего условия
	static CBrush m_brInversedCond;
	/// Кисть скрытого объекта
	static CBrush m_brHideObj;
	/// Кисть для отработавшего линка
	static CBrush m_brDonedLinkBrush;
	/// Карандаш для отработавшего линка
	static CPen m_penDonedLink;
};
