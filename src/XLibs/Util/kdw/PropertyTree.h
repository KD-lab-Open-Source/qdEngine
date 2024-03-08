#ifndef __KDW_PROPERTY_TREE_H_INCLUDED__
#define __KDW_PROPERTY_TREE_H_INCLUDED__

#include "kdw/Tree.h"
#include "kdw/ConstStringList.h"
#include "Serialization/Serializer.h"
#include "Serialization/LibraryBookmark.h"

class LibraryBookmark;

namespace kdw{

class PopupMenuItem;
class PropertyTreeModel;
class PropertyRow;
class PropertyRowWidget;
class PropertyTreeColumnDrawer;
class KDW_API PropertyTree : public Tree{
public:
	typedef PropertyTree Self;
	typedef Tree Base;
	PropertyTree(int border = 0);
	~PropertyTree();

	void attach(Serializer serializer);
	void attach(Serializers& serializers);
	void detach();

	void revert();
	void apply();

	void setHideUntranslated(bool hideUntranslated);
	bool hideUntranslated() const{ return hideUntranslated_; }
	void setImmediateUpdate(bool immediateUpdate);
	bool immediateUpdate() const{ return immediateUpdate_; }
	void setHasLibrarySupport(bool librarySupport){ librarySupport_ = librarySupport; }

	PropertyTreeModel* model();

	sigslot::signal1<LibraryBookmark>& signalFollowReference(){ return signalFollowReference_; }
	sigslot::signal0& signalBeforeApply(){ return signalBeforeApply_; }
	sigslot::signal0& signalChanged(){ return signalChanged_; }
	sigslot::signal0& signalSelected(){ return signalChanged_; } // FIXME
	sigslot::signal1<const PropertyRow*>& signalSearch(){ return signalSearch_; }
	sigslot::signal0& signalBeforeChange(){ return signalBeforeChange_; }
	bool justChanged() const{ return justChanged_; }
	bool isFocused() const;
	const PropertyTreeColumnDrawer* drawer() const;
	ComboStrings focusedPath();
	void selectItemByPath(const ComboStrings& path);

	void update();
	bool spawnWidget(PropertyRow* row, PropertyRow* hostRow);
	void onFollowReference(LibraryBookmark bookmark);
	void onSearch(PropertyRow* row);
	bool canSearch() const;
	bool rowVisible(TreeRow* row) const;
	bool rowHasVisibleChildren(const PropertyRow* row) const;
	bool rowHasIcon(PropertyRow* row) const;
protected:
	bool onContextMenu(PropertyRow* row, PopupMenuItem& menu);
	bool hitActivationRect(TreeRow* row, Vect2i point, const Recti& rowRect, int column);
	bool onRowKeyDown(TreeRow* row, sKey key);
	bool onRowLMBDown(TreeRow* row, const Recti& rowRect, Vect2i point);
	void onRowLMBUp(TreeRow* row, const Recti& rowRect, Vect2i point);
	void onRowRMBDown(TreeRow* row, const Recti& rowRect, Vect2i point);
	void onRowRMBUp(TreeRow* row, const Recti& rowRect, Vect2i point);
	void onRowMouseMove(TreeRow* row, const Recti& rowRect, Vect2i point);

	void onRowSelected(TreeRow* row);

	void onRowMenuCopy(PropertyRow* row);
	void onRowMenuPaste(PropertyRow* row);
	void onRowMenuDecompose(PropertyRow* row);

	void onModelUpdated();
	bool activateRow(PropertyRow* row);
	bool activateRowWidget(PropertyRow* row);
	bool canBePasted(PropertyRow* destination);
	bool canBePasted(const char* destinationType);

	void setWidget(PropertyRowWidget* widget);
	//void getWidgetRect(PropertyRow* row, Recti& rect);
	void _arrangeChildren();
	void visitChildren(WidgetVisitor& visitor) const;

	sigslot::signal1<LibraryBookmark> signalFollowReference_;
	sigslot::signal1<const PropertyRow*> signalSearch_;
	sigslot::signal0 signalBeforeApply_;
	sigslot::signal0 signalChanged_;
	sigslot::signal0 signalBeforeChange_;
	// in-place widget
	PolymorphicHandle<PropertyRowWidget> widget_;

	ConstStringList constStrings_;
	PropertyTreeModel* model_;
	Serializers attached_;

	bool justChanged_;
	bool immediateUpdate_;
	bool hideUntranslated_;
	bool librarySupport_;
};


std::string KDW_API generateDigest(Serializer& ser);

}

#endif
