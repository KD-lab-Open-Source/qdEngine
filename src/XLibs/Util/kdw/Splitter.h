#ifndef __KDW_SPLITTER_H_INCLUDED__
#define __KDW_SPLITTER_H_INCLUDED__

#include "kdw/Container.h"

namespace Win32{
    class Window;
};

namespace kdw{

class SplitterImpl;

class KDW_API Splitter: public Container{
public:
    Splitter(int splitterSize, int border, SplitterImpl* impl);
	~Splitter();
	static const int SPLITTER_WIDTH = 6;

    /// удалить все дочерние котролы
    void clear();
    /// изменить количество секций (мест для контролов)
    void resize(int newSize);
    /// добавить перед элементом beforeIndex (-1 = добавить в конец) 
    void add(Widget* widget, float position = 0.5f, bool fold = true, int beforeIndex = -1);
    /// удалить контрол по индексу
    void remove(int index, bool inFavourOfPrevious);
	/// заменить контрол
	void replace(Widget* oldWidget, Widget* newWidget);
	/// установить положение сплиттера
	void setSplitterPosition(float position, int splitterIndex = 0);

	int splitterSpacing() const{ return splitterSpacing_; }
	float widgetPosition(Widget* widget) const;
	Widget* widgetByPosition(float position);
	Widget* widgetByScreenPoint(Vect2i point);
	Widget* widgetByIndex(int index);
	virtual bool vertical() = 0;

    // virtuals:
	void visitChildren(WidgetVisitor& visitor) const;
	void serialize(Archive& ar);
    // ^^

	void _updateVisibility();
	void _setParent(Container* container);
	void _setPosition(const Recti& position);
	void _arrangeChildren();
	void _relayoutParents();

	int splittersCount();
	Recti getSplitterRect(int splitterIndex);
protected:
	friend SplitterImpl;
	void _setFocus();

	Win32::Window32* _window() const{ return window_; }

	void setPanePosition(int index, int poisitionInPixels);
	virtual int boxLength() const = 0;
	virtual Vect2i elementSize(Widget* widget) const = 0;
	virtual void setSplitterMinimalSize(const Vect2i& size) = 0;
	virtual Recti rectByPosition(int start, int end) = 0;
	virtual int positionFromPoint(const Vect2i point) const = 0;

	int splitterWidth() const;


	struct Element{
		Element()
		: position(0.0f)
		, snappedPosition(0.0f)
		, fold(false)
		{
		}
		void serialize(Archive& ar);
		ShareHandle<kdw::Widget> widget;
		Recti splitterRect;
		float position;
		float snappedPosition;
		bool fold;
	};

	typedef std::list<Element> Elements;
	Elements elements_;
    Win32::Window32* window_;
	int splitterSpacing_;
};

}

#endif
