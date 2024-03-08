#ifndef __KDW_CHECK_COMBO_BOX_H_INCLUDED__
#define __KDW_CHECK_COMBO_BOX_H_INCLUDED__

#include <vector>
#include <string>
#include "kdw/_WidgetWithWindow.h"

class ComboListString;
namespace kdw{

class CheckComboBoxImpl;
class CheckComboBox : public _WidgetWithWindow{
public:
	CheckComboBox(bool expandByContent = true, int border = 0);

	typedef std::vector<std::string> Items;
	typedef Items::iterator iterator;

	void setExpandByContent(bool expand);
	bool expandByContent() const{ return expandByContent_; }

	void showDropDown();
	void setDropDownHeight(int lines);
	int dropDownHeight() const;
	void selectAll(bool select = true);

	void serialize(Archive& ar);

	const char* value(){ return value_.c_str(); }
	void setValue(const char* value);

	void set(const char* comboList, const char* value);
	//void set(const char* comboList, int value);
	//void set(const ComboListString& comboListString);
	//void get(ComboListString& comboListString);

	void clear();	
	iterator begin() { return items_.begin(); }
	iterator end() { return items_.end(); }
	void add(const char* text);
	void insert(iterator before, const char* text);

	virtual void onEdited() { signalEdited_.emit(); }
	sigslot::signal0& signalEdited() { return signalEdited_; }
	
	virtual void onSelectionChanged(){ signalSelectionChanged_.emit(); }
	sigslot::signal0& signalSelectionChanged() { return signalSelectionChanged_; }

protected:
	CheckComboBoxImpl* window() const{ return reinterpret_cast<CheckComboBoxImpl*>(_window()); }
	friend class CheckComboBoxImpl;
	friend class CheckComboListBoxImpl;
	void _setPosition(const Recti& position);

	void updateMinimalSize();

	sigslot::signal0 signalSelectionChanged_;
	sigslot::signal0 signalEdited_;

	bool expandByContent_;
	Items items_;
	std::string value_;
	int dropDownHeight_;
};

}
#endif
