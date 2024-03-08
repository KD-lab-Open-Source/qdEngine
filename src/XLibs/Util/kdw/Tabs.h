#ifndef __KDW_TAB_H_INCLUDED__
#define __KDW_TAB_H_INCLUDED__

#include "kdw/_WidgetWithWindow.h"
#include "kdw/VBox.h"

namespace kdw{

class TabsImpl;

class TabChanger{};

class Tabs : public _WidgetWithWindow, public TabChanger{
public:
	Tabs(int border = 0);

	int add(const char* tabTitle, int before = -1);
	void remove(int index);
	void clear();
	
	int selectedTab();
	void setSelectedTab(int index, const TabChanger* changer = 0);

	typedef sigslot::signal1<const TabChanger*> SignalChanged;
	SignalChanged& signalChanged(){ return signalChanged_; }
	
	typedef sigslot::signal2<MouseButton, int> SignalMouseButtonDown;
	SignalMouseButtonDown& signalMouseButtonDown(){ return signalMouseButtonDown_; }
protected:
	SignalChanged signalChanged_;
	SignalMouseButtonDown signalMouseButtonDown_;

	TabsImpl& impl();
	friend class TabsImpl;
};

class TabPages : public VBox, public TabChanger{
public:
	TabPages(int border = 0);

	int add(const char* title, Widget* widget, int before = -1);
	void remove(int index);
	Widget* widgetByIndex(int index);
	int size() const;

	void setSelectedTab(int index);
protected:
	void onTabChange(const TabChanger* changer);
	typedef std::vector<ShareHandle<Widget> > Widgets;
	ShareHandle<Tabs> tabs_;
	Widgets widgets_;
};

}

#endif
