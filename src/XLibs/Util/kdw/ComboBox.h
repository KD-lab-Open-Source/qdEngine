#ifndef __KDW_COMBO_BOX_H_INCLUDED__
#define __KDW_COMBO_BOX_H_INCLUDED__

#include <vector>
#include <string>

#include "kdw/_WidgetWithWindow.h"

class ComboListString;

namespace kdw{
    
class ComboBoxImpl;
class ComboBox : public _WidgetWithWindow{
public:
	ComboBox(bool expandByContent = false, int border = 0);


	typedef std::vector<std::string> Items;
	typedef Items::iterator iterator;

	void setSelectedIndex(int index);
	int selectedIndex() const;
	std::string selectedString() const;

	void setExpandByContent(bool expand);
	bool expandByContent() const{ return expandByContent_; }

	void showDropDown();
	void setDropDownHeight(int lines);
	int dropDownHeight() const;

	void serialize(Archive& ar);

	void set(const char* comboList, const char* value, bool onlyVisible = false);
	//void set(const char* comboList, int value);
	void set(const ComboListString& comboListString, bool onlyVisible = false);
	void get(ComboListString& comboListString);


	void clear();
	iterator begin() { return items_.begin(); }
	iterator end() { return items_.end(); }
	void add(const char* text);
	void insert(iterator before, const char* text);

	virtual void onEdited();
	sigslot::signal0& signalEdited() { return signalEdited_; }
	
	virtual void onSelectionChanged(){ signalSelectionChanged_.emit(); }
	sigslot::signal0& signalSelectionChanged() { return signalSelectionChanged_; }

	void _setFocus();
protected:
	ComboBox(ComboBoxImpl* impl, bool expandByContent, int border);

	ComboBoxImpl* impl() const{ return reinterpret_cast<ComboBoxImpl*>(_window()); }
	friend class ComboBoxImpl;
	void _setPosition(const Recti& position);

	void updateMinimalSize();

	sigslot::signal0 signalSelectionChanged_;
	sigslot::signal0 signalEdited_;

	bool expandByContent_;
	Items items_;
	int selectedIndex_;
	int dropDownHeight_;
};

}

#endif
