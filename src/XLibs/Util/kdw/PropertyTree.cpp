#include "StdAfx.h"
#include "kdw/Tree.h"
#include "kdw/TreeView.h"
#include "kdw/PropertyTree.h"
#include "kdw/PropertyTreeModel.h"

#include "Serialization/SerializationFactory.h"
#include "kdw/Serialization.h"

#include "kdw/_PropertyRowBuiltin.h"
#include "kdw/Clipboard.h"

#include "PropertyOArchive.h"
#include "PropertyIArchive.h"
#include "kdw/_PropertyTreeColumnDrawer.h"
#include "kdw/PopupMenu.h"

#include "kdw/Win32/Handle.h"
#include "kdw/Win32/Window.h" // только дл€ defaultFont
#include "kdw/Win32/Rectangle.h"
#include <crtdbg.h>

#include "kdw/PropertyEditor.h"



namespace kdw{

REGISTER_CLASS(Widget, PropertyTree, "ƒерево свойств")

// --------------------------------------------------------------------------------

REGISTER_CLASS(TreeColumnDrawer, PropertyTreeColumnDrawer, "ќтрисовщик колонки дерева свойств");


// --------------------------------------------------------------------------------


PropertyTreeColumnDrawer::PropertyTreeColumnDrawer(Tree* tree)
: TreeColumnDrawer(tree)
{

}
int PropertyTreeColumnDrawer::rowHeight(TreeRow* treeRow, int column) const
{
	PropertyRow* row = static_cast<PropertyRow*>(treeRow);
	return PropertyRow::ROW_DEFAULT_HEIGHT + row->floorHeight();
}

int PropertyTreeColumnDrawer::rowIndent(TreeRow* treeRow) const
{
	PropertyRow* row = static_cast<PropertyRow*>(treeRow);
	return ((row->isContainer() && row->pulledUp()) ? 0 : 12);
}

bool PropertyTreeColumnDrawer::rowHasIcon(PropertyRow* row) const
{
	return row->hasIcon() || (row->pulledRow() && row->pulledRow()->hasIcon());
}

HFONT PropertyTreeColumnDrawer::rowFont(PropertyRow* row) const
{
	HFONT normalFont = Win32::defaultFont();
	HFONT boldFont = Win32::defaultBoldFont();
	return (tree()->rowHasVisibleChildren(row) || row->isContainer()) ? (tree()->emphasizeFolders() ? boldFont : normalFont) : normalFont ;
}

std::string PropertyTreeColumnDrawer::rowText(PropertyRow* row) const
{
	PropertyRow* parent = row->parent();
	if(parent && parent->isContainer()){
		std::string text;
		int index = 0;
		PropertyRow::iterator it = parent->begin();
		while(it != parent->end() && *it != row){
			++index;
			++it;
		}
		char buffer[15];
		sprintf(buffer, " %i.", index);
		text = buffer;
		return text;
	}
	else{
		return row->nameAlt()[0] ? row->nameAltUndecorated() : row->name();
	}
}

TreeHitTest PropertyTreeColumnDrawer::hitTest(TreeRow* treeRow, Vect2i point, const RECT& rowRect)
{
	PropertyRow* row = safe_cast<PropertyRow*>(treeRow);
	RECT rect = rowRect;
	rect.left += row->indent();
	rect.left += 1;
	rect.right -= 1;
	rect.top += 1;
	::InflateRect(&rect, -2, -2);
	int plusSize = rect.bottom - rect.top;
	RECT plusRect;
	plusRect.left = rect.left;
	plusRect.top = rect.top;
	plusRect.right = plusRect.left + plusSize;
	plusRect.bottom = plusRect.top + plusSize;
	rect.left += plusSize;

	// text
	HFONT normalFont = Win32::defaultFont();
	HFONT boldFont = Win32::defaultBoldFont();

	bool empty = !tree()->rowHasVisibleChildren(row);
	HFONT font_ = empty ? normalFont : (tree()->emphasizeFolders() ? boldFont : normalFont);
	RECT textRect = rect;
	
	std::string text = rowText(row);

	Vect2i size = calculateTextSize(*Win32::_globalDummyWindow, font_, text.c_str());
	textRect.right = textRect.left + size.x;
	textRect.bottom = textRect.top + size.y;
	//text

	POINT pt = {point.x, point.y};
	if(!empty && PtInRect(&plusRect, pt))
		return TREE_HIT_PLUS;
	else if(PtInRect(&textRect, pt))
		return TREE_HIT_TEXT;
	else if(PtInRect(&rect, pt))
		return TREE_HIT_ROW;
	else
		return TREE_HIT_NONE;
}

PropertyRow::WidgetPosition PropertyTreeColumnDrawer::overridenWidgetPosition(PropertyRow* row) const
{
	if(row->fullRow()){
		if(strlen(row->digest()) == 0)
			return PropertyRow::WIDGET_POSITION_ROW;
		else
			return PropertyRow::WIDGET_POSITION_COLUMN;
	}
	if(row->pulledRow() && row->pulledRow()->hasIcon())
		return PropertyRow::WIDGET_POSITION_ROW_AUTOHIDE;
	return row->widgetPosition();
}

void PropertyTreeColumnDrawer::getSubRectPlus(TreeRow* row, const Recti& rowRect, Recti& outResult) const
{
	if(row->parent() == tree_->model()->root() && tree_->compact())
		outResult.set(rowRect.left() + row->indent(), rowRect.top(), 0, PropertyRow::ROW_DEFAULT_HEIGHT);
	else
		outResult.set(rowRect.left() + row->indent(), rowRect.top(), PropertyRow::ROW_DEFAULT_HEIGHT, PropertyRow::ROW_DEFAULT_HEIGHT);
}

void PropertyTreeColumnDrawer::getSubRectIcon(PropertyRow* row, const Recti& rowRect, Recti& outRect) const
{
	if(rowHasIcon(row)){
		Recti textRect;
		getSubRectText(row, rowRect, textRect);
		outRect = rowRect;
		outRect.left(textRect.left() - PropertyRow::ROW_DEFAULT_HEIGHT);
		outRect.width(PropertyRow::ROW_DEFAULT_HEIGHT);
		outRect.height(PropertyRow::ROW_DEFAULT_HEIGHT);
	}
	else{
		outRect.set(0, 0, 0, 0);
	}
}

void PropertyTreeColumnDrawer::getSubRectFloor(PropertyRow* row, const Recti& rowRect, Recti& outRect) const
{
	int floorHeight = row->floorHeight();
	int indent = row->indent() + PropertyRow::ROW_DEFAULT_HEIGHT;
	outRect.set(rowRect.left() + indent, rowRect.top() + PropertyRow::ROW_DEFAULT_HEIGHT,
				rowRect.width() - indent, floorHeight);
}


void PropertyTreeColumnDrawer::getSubRectWidget(PropertyRow* row, const Recti& rowRect, Recti& outRect) const
{
	Recti textRect;
	getSubRectText(row, rowRect, textRect);
	Recti rect = rowRect;

	int requestedWidth = row->minimalWidth();

	PropertyRow::WidgetPosition widgetPos = overridenWidgetPosition(row);

    int editorWidth = 0;	

	if(widgetPos == PropertyRow::WIDGET_POSITION_COLUMN)
		requestedWidth = rowRect.width() / 2;
	else if(widgetPos == PropertyRow::WIDGET_POSITION_ROW)
		requestedWidth = rowRect.width() - (rowText(row).empty() ? 0 : textRect.right());
	if(widgetPos == PropertyRow::WIDGET_POSITION_ROW_AUTOHIDE){
		requestedWidth = rowRect.width() - textRect.right();
		if(requestedWidth < row->minimalWidth())
			editorWidth = 0;
		else
			editorWidth =  min(rect.right() - textRect.right() - 4,
			                   row->minimalWidth());
	}
	else{
		editorWidth = clamp(requestedWidth,
			                row->minimalWidth(),
							rect.right() - textRect.right() - (rowText(row).empty() ? 0 : 4));
	}


	if(row->isStatic()){
		outRect.set(rect.right(), rect.top(), 0, PropertyRow::ROW_DEFAULT_HEIGHT);
	}
	else{
		if(editorWidth < row->minimalWidth() && overridenWidgetPosition(row) != PropertyRow::WIDGET_POSITION_ROW_AUTOHIDE){
			textRect.right(textRect.right() + editorWidth - row->minimalWidth());
			editorWidth = row->minimalWidth();
		}
		outRect.set(rect.right() - editorWidth, rect.top() + (PropertyRow::ROW_DEFAULT_HEIGHT -  PropertyRow::ROW_DEFAULT_HEIGHT) / 2, editorWidth, PropertyRow::ROW_DEFAULT_HEIGHT);
	}

}

void PropertyTreeColumnDrawer::getSubRectText(TreeRow* treeRow, const Recti& rowRect, Recti& outResult) const
{
	PropertyRow* row = static_cast<PropertyRow*>(treeRow);
	std::string text = rowText(row);
	HFONT font = rowFont(row);

	// считаем пр€моугольник текста
	Vect2i textSize = calculateTextSize(*Win32::_globalDummyWindow, font, text.c_str());
	int indent = 2 + row->indent();
	if(rowHasIcon(row))
		indent += PropertyRow::ROW_DEFAULT_HEIGHT;
		
	int rowMinimalWidth = (overridenWidgetPosition(row) != PropertyRow::WIDGET_POSITION_ROW_AUTOHIDE) ? row->minimalWidth() : 0;
	if(tree_->compact() && treeRow->parent() == model()->root())
		outResult.set(indent, rowRect.top(), min(textSize.x, rowRect.width() - rowMinimalWidth - indent), PropertyRow::ROW_DEFAULT_HEIGHT);
	else
		outResult.set(PropertyRow::ROW_DEFAULT_HEIGHT + indent, rowRect.top(), min(textSize.x, rowRect.width() - rowMinimalWidth - indent - PropertyRow::ROW_DEFAULT_HEIGHT), PropertyRow::ROW_DEFAULT_HEIGHT);
}

void PropertyTreeColumnDrawer::drawRow(TreeRow* treeRow, int column, HDC dc, const RECT& rowRectr, int indent, bool focused, bool hovered, bool activated) const
{
	//bool lastRow = false;
	Recti rowRect(rowRectr.left, rowRectr.top, rowRectr.right - rowRectr.left, rowRectr.bottom - rowRectr.top);
	PropertyRow* row = safe_cast<PropertyRow*>(treeRow);
	if(column != 0 || rowRectr.bottom == rowRectr.top ||
		(row->isContainer() && row->pulledUp()))
		return;

	std::string text = rowText(row);
	RECT rect = rowRectr;
	RECT selectionRect = rowRectr;
	
	selectionRect.left += indent;
	bool inRoot = treeRow->parent() == tree_->model()->root();
	if(!(tree_->compact() && inRoot))
		selectionRect.left += PropertyRow::ROW_DEFAULT_HEIGHT - 1;

	COLORREF textColor = ::GetSysColor(COLOR_BTNTEXT);
	if(row->selected()){
		// заливаем фон цветом выделени€
		if(tree()->isFocused())
			::FillRect(dc, &selectionRect, ::GetSysColorBrush(COLOR_HIGHLIGHT));
		else
			::FillRect(dc, &selectionRect, ::GetSysColorBrush(COLOR_3DSHADOW));
		textColor = ::GetSysColor(COLOR_HIGHLIGHTTEXT);
	}

	if(!tree_->compact() || row->parent() != model()->root()){
		Recti plusRect(0, 0, 0, 0);
		getSubRectPlus(row, rowRect, plusRect);
		/*
		// вертикальные линии
		if(!tree_->compact()){
			Vect2i center(plusRect.center());

			Win32::AutoSelector pen(dc, CreatePen(PS_SOLID, 0, GetSysColor(COLOR_3DSHADOW)));

			if(row->level() > 0){
				PropertyRow* parent = row;
				for(int i = row->level(); i > 0; --i){
                    indent = plusRect.right() - 4 - (row->indent() - parent->indent());
					::MoveToEx(dc, indent, rowRect.top(), 0);
					::LineTo(dc, indent, rowRect.bottom());
					parent = parent->parent();
				}
			}
			
			if(false){
				PropertyRow* parent = row;
				while(parent && parent->parent() && parent->parent()->parent()){
					Recti plusRect(0, 0, 0, 0);
					// вертикальные линии
					getSubRectPlus(parent, rowRect, plusRect);
					Vect2i center(plusRect.center());

					::MoveToEx(dc, center.x, rowRect.top(), 0);
					::LineTo(dc, center.x, lastRow ? center.y : (rowRect.bottom() + 1));
					::MoveToEx(dc, center.x, center.y, 0);
					::LineTo(dc, plusRect.right(), center.y);
					parent = parent->parent();
				}
			}
		}
		*/
		// рисуем плюсик
		if(tree()->rowHasVisibleChildren(row)){
			getSubRectPlus(row, rowRect, plusRect);
			drawPlus(dc, plusRect, row->expanded(), row->selected(), row->expanded());
		}
	}


	// рисуем custom-ную часть
	Recti iconRect;
	getSubRectIcon(row, rowRect, iconRect);
	Recti editorRect;
	getSubRectWidget(row, rowRect, editorRect);
	if(!row->isStatic()){

		Recti floorRect;
		getSubRectFloor(row, rowRect, floorRect);
		row->redraw(dc, Win32::Rect(iconRect), Win32::Rect(editorRect), Win32::Rect(floorRect), row);
	}

	// рисуем текст
	Recti textRect;
	getSubRectText(row, rowRect, textRect);
	textRect.left(textRect.left() + 2);

	HFONT font = rowFont(row);

	HBRUSH oldBrush = HBRUSH(::SelectObject(dc, (HGDIOBJ)(::GetStockObject(HOLLOW_BRUSH))));
	int oldBackMode = ::SetBkMode(dc, TRANSPARENT);
	COLORREF oldBkColor = ::SetBkColor(dc, ::GetSysColor(COLOR_BTNFACE));
	COLORREF oldTextColor = ::SetTextColor(dc, textColor);
	HFONT oldFont = (HFONT)::SelectObject(dc, font);
	::DrawText(dc, text.c_str(), int(strlen(text.c_str())), &Win32::Rect(textRect), DT_LEFT | DT_END_ELLIPSIS | DT_SINGLELINE | DT_VCENTER);
	
	int textRight = textRect.right();
	// краткое содержание
	if(row->digest()[0]){
		const char* text = row->digest();

		int right = editorRect.left() > textRect.right() ? editorRect.left() : rowRect.right();
		if(right + 16 > textRect.right()){
			::SelectObject(dc, Win32::defaultFont());
			::SetTextColor(dc, GetSysColor(COLOR_3DSHADOW));
			Win32::Rect digestRect(textRect.right() + 6, textRect.top(), right, textRect.bottom());
			::DrawText(dc, text, int(strlen(text)), &Win32::Rect(digestRect), DT_LEFT | DT_END_ELLIPSIS | DT_SINGLELINE | DT_VCENTER);
			textRight = right;
		}
	}

	// рисуем пунктирную линию
	if(!row->isStatic() && overridenWidgetPosition(row) != PropertyRow::WIDGET_POSITION_ROW_AUTOHIDE){
		Vect2i textSize(textRect.size());
		HPEN dotPen = CreatePen(PS_DOT, 1, GetSysColor(COLOR_3DSHADOW));
		int y = rowRect.top() + round(PropertyRow::ROW_DEFAULT_HEIGHT * 0.77f);
		HPEN oldPen = (HPEN)SelectObject(dc, dotPen);
		int len = editorRect.left() - textRight - 8;
		if(len > 0){
			MoveToEx(dc, textRight + len + 4, y, 0);
			LineTo(dc, textRight, y);
			SelectObject(dc, oldPen);
			DeleteObject(dotPen);
		}
		SelectObject(dc, oldPen);
		DeleteObject(dotPen);
	}

	if(focused){
		selectionRect.left -= 1;
		selectionRect.top -= 1;
		selectionRect.right += 1;
		selectionRect.bottom += 1;
		DrawFocusRect(dc, &selectionRect);
	}

	::SetTextColor(dc, oldTextColor);
	::SelectObject(dc, oldFont);
	::SelectObject(dc, oldBrush);
	::SetBkMode(dc, oldBackMode);
	::SetBkColor(dc, oldBkColor);
}


// --------------------------------------------------------------------------------

PropertyTree::PropertyTree(int border)
: Tree(0, border)
, immediateUpdate_(true)
, justChanged_(false)
, librarySupport_(false)
, hideUntranslated_(true)
{
	setEmphasizeFolders(true);
	setSelectFocused(true);
	setFlat(true);
	model_ = new PropertyTreeModel();
	setModel(model_);
	PropertyTreeColumnDrawer* drawer = new PropertyTreeColumnDrawer(this);
	setColumn(0, drawer, 0.0f);
	setColumn(1, drawer, 0.4f);
}

PropertyTree::~PropertyTree()
{
}

void PropertyTree::attach(Serializer serializer)
{
	attached_.clear();
	attached_.push_back(serializer);
	revert();
}
void PropertyTree::attach(Serializers& serializers)
{
	attached_ = serializers;
	revert();
}

void PropertyTree::detach()
{
	attached_.clear();
	model()->root()->clear();
	update();
}

void PropertyTree::revert()
{
	signalBeforeApply_.emit();
	if(!attached_.empty()){
		interruptDrag();
		widget_ = 0;
		PropertyOArchive oa(model_);

		Serializers::iterator it = attached_.begin();
		it->serialize(oa);
		while(++it != attached_.end()){
			PropertyTreeModel model2;
			PropertyOArchive oa2(&model2);
			it->serialize(oa2);
			model_->root()->intersect(model2.root());
		}
		update();
	}
}

void PropertyTree::apply()
{
	if(!attached_.empty()){
		Serializers::iterator it;
		FOR_EACH(attached_, it){
			PropertyIArchive ia(model_);
			it->serialize(ia);
		}
	}
}

void PropertyTree::setImmediateUpdate(bool immediateUpdate)
{
	immediateUpdate_ = immediateUpdate;
}

void PropertyTree::setHideUntranslated(bool hideUntranslated)
{
	hideUntranslated_ = hideUntranslated;
}

const PropertyTreeColumnDrawer* PropertyTree::drawer() const
{
	return safe_cast<const PropertyTreeColumnDrawer*>(columns()[0].drawer());
}

void PropertyTree::update()
{
	__super::update();
}

bool PropertyTree::rowVisible(TreeRow* _row) const
{
	PropertyRow* row = safe_cast<PropertyRow*>(_row);
	return !(row->hidden() && hideUntranslated_);
}

bool PropertyTree::rowHasVisibleChildren(const PropertyRow* row) const
{
	if(row->empty())
		return false;
	if(!hideUntranslated_)
		return true;
	PropertyRow::const_iterator it;
	FOR_EACH(*row, it){
		const PropertyRow* child = safe_cast<const PropertyRow*>(&**it);
		if(!child->hidden())
			return true;
	}
	return false;
}

bool PropertyTree::rowHasIcon(PropertyRow* row) const
{
	return drawer()->rowHasIcon(row);
}

bool PropertyTree::hitActivationRect(TreeRow* _row, Vect2i point, const Recti& rowRect, int column)
{
	PropertyRow* row = safe_cast<PropertyRow*>(_row);
	Recti rect;
	drawer()->getSubRectWidget(row, rowRect, rect);
	if(rect.point_inside(point))
		return true;
	drawer()->getSubRectIcon(row, rowRect, rect);
	if(rect.point_inside(point))
		return true;
	return false;
}

bool PropertyTree::onRowKeyDown(TreeRow* _row, sKey key)
{
	PropertyRow* row = safe_cast<PropertyRow*>(_row);
	if(row->onKeyDown(this, key))
		return true;

	switch(key){
	case VK_SPACE:
		if(activateRow(row))
			return true;
		break;
	case 'C' | sKey::CONTROL:
		onRowMenuCopy(row);
		return true;
	case 'V' | sKey::CONTROL:
		onRowMenuPaste(row);
		return true;
	case VK_APPS: {
		PopupMenu menu(300);
		
		if(onContextMenu(row, menu.root())){
			Recti rowRect;
			getRowRect(row, rowRect);
			Win32::Rect rect(rowRect);
			_window()->clientToScreen(rect);
			Vect2i pt(rect.left + (rect.bottom - rect.top), rect.bottom);
			menu.spawn(pt, this);
		}
		return true;
		}
	}
	
	return Base::onRowKeyDown(row, key);
}

bool PropertyTree::spawnWidget(PropertyRow* row, PropertyRow* hostRow)
{
	if(!widget_ || widget_->locationRow() != row){
		interruptDrag();
		setWidget(0);
		PropertyRowWidget* newWidget = row->createWidget(model());
		if(newWidget && hostRow != row)
			newWidget->setLocationRow(hostRow);
		setWidget(newWidget);
		return newWidget != 0;
	}
	return false;
}

void PropertyTree::onFollowReference(LibraryBookmark bookmark)
{
	signalFollowReference_.emit(bookmark);
}

void PropertyTree::onSearch(PropertyRow* row)
{
	signalSearch_.emit(row);
}

bool PropertyTree::canSearch() const
{
	return librarySupport_;
}

bool PropertyTree::activateRowWidget(PropertyRow* row)
{
	interruptDrag();
	return row->onActivateWidget(this, row);
}

bool PropertyTree::activateRow(PropertyRow* row)
{
	interruptDrag();
	return model()->activateRow(this, row, 0);
}

bool PropertyTree::onRowLMBDown(TreeRow* treeRow, const Recti& rowRect, Vect2i point)
{
	PropertyRow* row = safe_cast<PropertyRow*>(treeRow);
	const PropertyTreeColumnDrawer* drawer = this->drawer();
	
	PropertyTreeModel::UpdateLock lock = model()->lockUpdate();

	if(treeRow != model()->root()){
		bool changed = false;
		bool capture = row->onMouseDown(this, point, changed);
		
		if(!changed && !widget_){ // FIXME: осмысленный метод дл€ проверки
			Base::onRowLMBDown(treeRow, rowRect, point);
			if(capture){
				return true;
			}

			Recti rect;
			drawer->getSubRectIcon(row, rowRect, rect);
			if(rect.point_inside(point)){
				model()->activateRowIcon(this, row);
				return false;
			}
			drawer->getSubRectWidget(row, rowRect, rect);
			if(rect.point_inside(point)){
				if(activateRowWidget(row))
					return false;
			}
			return capture;
		}
	}
	return Base::onRowLMBDown(treeRow, rowRect, point);	
}

void PropertyTree::onRowLMBUp(TreeRow* treeRow, const Recti& rowRect, Vect2i point)
{
	PropertyRow* row = safe_cast<PropertyRow*>(treeRow);
	row->onMouseUp(this, point);
	if(GetCapture() == *_window())
		ReleaseCapture();
}

bool PropertyTree::onContextMenu(PropertyRow* row, PopupMenuItem& menu)
{
	PropertyRow::iterator it;
	for(it = row->begin(); it != row->end(); ++it){
		PropertyRow* child = safe_cast<PropertyRow*>(&**it);
		if(child->isContainer() && child->pulledUp())
			child->onContextMenu(menu, this);
	}
	row->onContextMenu(menu, this);
	if(!menu.empty())
		menu.addSeparator();
	menu.add(TRANSLATE(" опировать"), row)
		.connect(this, &Self::onRowMenuCopy)
		.setHotkey(sKey('C' | sKey::CONTROL));
	menu.add(TRANSLATE("¬ставить"), row)
		.connect(this, &Self::onRowMenuPaste)
		.enable(!row->readOnly())
		.setHotkey(sKey('V' | sKey::CONTROL))
		.enable(canBePasted(row));
#ifdef NDEBUG
	menu.addSeparator();
	menu.add(TRANSLATE("–азобрать"), row).connect(this, &Self::onRowMenuDecompose);
#endif
	return true;
}


void PropertyTree::onRowRMBDown(TreeRow* treeRow, const Recti& rowRect, Vect2i point)
{
	PropertyRow* row = safe_cast<PropertyRow*>(treeRow);
	PopupMenu menu(300);

	if(onContextMenu(row, menu.root())){
		menu.spawn(this);
	}
}

void PropertyTree::onRowRMBUp(TreeRow* row, const Recti& rowRect, Vect2i point)
{
}
void PropertyTree::onRowMouseMove(TreeRow* treeRow, const Recti& rowRect, Vect2i point)
{
	PropertyRow* row = safe_cast<PropertyRow*>(treeRow);
	row->onMouseMove(this, point);
}


void PropertyTree::onRowSelected(TreeRow* treeRow)
{
	PropertyRow* row = safe_cast<PropertyRow*>(treeRow);
	if(row->onSelect(model())){
		Tree::onRowSelected(treeRow);
	}
}

void PropertyTree::onRowMenuCopy(PropertyRow* row)
{
	Clipboard clipboard(this, &constStrings_, model());
    clipboard.copy(row);

}


void PropertyTree::onRowMenuPaste(PropertyRow* row)
{
	if(!canBePasted(row))
		return;
	PropertyRow* parent = row->parent();

	Clipboard clipboard(this, &constStrings_, model());
	if(clipboard.paste(row))
		model()->rowChanged(parent ? parent : model()->root());
	else
		ASSERT(0 && "Unable to paste element!"); // TODO: осмысленное сообщение
}

bool PropertyTree::canBePasted(PropertyRow* destination)
{
	if(destination->readOnly())
		return false;
	Clipboard clipboard(this, &constStrings_, model());
	return clipboard.paste(destination, true);
}

bool PropertyTree::canBePasted(const char* destinationType)
{
	Clipboard clipboard(this, &constStrings_, model());
	return clipboard.canBePastedOn(destinationType);
}

void PropertyTree::onRowMenuDecompose(PropertyRow* row)
{
	ShareHandle<PropertyRow> clonedRow = row->clone();
	Serializer se(clonedRow, "row", "row");
	edit(se, 0, IMMEDIATE_UPDATE, this);
}

void PropertyTree::onModelUpdated()
{
	signalBeforeChange_.emit();

	if(widget_){
		widget_ = 0;
	}
	if(immediateUpdate_){
		apply();
		revert();
	}
	Tree::onModelUpdated();
	justChanged_ = true;
	signalChanged_.emit();
	justChanged_ = false;
}


void PropertyTree::setWidget(PropertyRowWidget* widget)
{
	if(widget_){
		widget_->commit();
		xassert(widget_->actualWidget());
		if(widget_->actualWidget())
			widget_->actualWidget()->hide();
		widget_->addRef();
		widget_ = 0;
	}
	widget_ = widget;
	model()->dismissUpdate();
	if(widget){
		xassert(widget_->actualWidget());
        if(widget_->actualWidget())
			widget_->actualWidget()->_setParent(this);
		_arrangeChildren();
	}
}

void PropertyTree::visitChildren(WidgetVisitor& visitor) const
{
	if(widget_){
		visitor(*widget_->actualWidget());
	}
}



void PropertyTree::_arrangeChildren()
{
	if(widget_){
		PropertyRow* row = widget_->locationRow();
		Recti rowRect;
		if(getRowRect(row, rowRect)){
			Recti rect;
			drawer()->getSubRectWidget(row, rowRect, rect);
			Widget* w = widget_->actualWidget();
			xassert(w);
			if(w){
				w->_setPosition(rect);
				if(!w->isVisible()){
					w->show();
					if(w->_focusable())
						w->setFocus();
					else{
						//xassert(w->_focusable());
					}
				}
			}
			else{
				//xassert(w);
			}
		}
		else{
			widget_ = 0;
		}
	}
}

ComboStrings PropertyTree::focusedPath()
{
    return ComboStrings();
}

void PropertyTree::selectItemByPath(const ComboStrings& path)
{

}

bool PropertyTree::isFocused() const
{
	return Tree::isFocused() || (widget_ && GetFocus() == *widget_->actualWidget()->_window());
}

PropertyTreeModel* PropertyTree::model()
{
	return static_cast<PropertyTreeModel*>(Tree::model());
}

// --------------------------------------------------------------------------------

PropertyTreeModel::PropertyTreeModel()
{
	setRoot(new PropertyRow("", "root", ""));
}

bool PropertyTreeModel::activateRow(Tree* tree, TreeRow* treeRow, int column)
{
	PropertyRow* row = safe_cast<PropertyRow*>(treeRow);
	return row->onActivate(safe_cast<PropertyTree*>(tree));

}

bool PropertyTreeModel::activateRowIcon(Tree* _tree, PropertyRow* row)
{
	PropertyTree* tree = safe_cast<PropertyTree*>(_tree);
	if(row->pulledRow() && row->pulledRow()->hasIcon())
		return row->pulledRow()->onActivateIcon(tree);
	return row->onActivateIcon(tree);
}

void PropertyTreeModel::rowChanged(PropertyRow* row)
{
	row->setMultiValue(false);
	while(row->parent()){
		row->childrenUpdated();
		row = row->parent();
	}
	requestUpdate();
}

PropertyRow* PropertyTreeModel::root()
{
	return static_cast<PropertyRow*>(TreeModel::root());
}

void PropertyTreeModel::onUpdated()
{
	TreeModel::onUpdated();
}

void PropertyTreeModel::clear()
{
	root_ = 0;
}

bool PropertyTreeModel::defaultTypeRegistered(const char* typeName) const
{
	return defaultTypes_.find(typeName) != defaultTypes_.end();
}

void PropertyTreeModel::addDefaultType(PropertyRow* row, const char* typeName)
{
	defaultTypes_[typeName] = row;
}

PropertyRow* PropertyTreeModel::defaultType(const char* typeName) const
{
	DefaultTypes::const_iterator it = defaultTypes_.find(typeName);
	ASSERT(it != defaultTypes_.end());
	return it->second;
}

void PropertyTreeModel::addDefaultType(PropertyRow* row, const char* baseName, const char* derivedTypeName, const char* derivedTypeNameAlt)
{
	ASSERT(baseName);
	ASSERT(derivedTypeName);
	ASSERT(derivedTypeNameAlt);
	DefaultTypesPoly::iterator it = defaultTypesPoly_.find(baseName);
	BaseClass& base = (it == defaultTypesPoly_.end()) ? defaultTypesPoly_[baseName] : it->second;
	DerivedTypes::iterator dit = base.types.begin();
	while(dit != base.types.end()){
		if(dit->name == derivedTypeName){
			DerivedClass& derived = *dit;
			derived.name = derivedTypeName;
			ASSERT(derived.row == 0);
			derived.row = row;
			return;
		}
		++dit;
	}
	base.types.push_back(DerivedClass());
	DerivedClass& derived = base.types.back();
	derived.name = derivedTypeName;
	derived.row = row;
	base.strings.push_back(derivedTypeNameAlt);
}

PropertyRow* PropertyTreeModel::defaultType(const char* baseName, int derivedIndex) const
{
	DefaultTypesPoly::const_iterator it = defaultTypesPoly_.find(baseName);
	ASSERT(it != defaultTypesPoly_.end());
	const BaseClass& base = it->second;
	ASSERT(derivedIndex >= 0);
	ASSERT(derivedIndex < int(base.types.size()));
	DerivedTypes::const_iterator tit = base.types.begin();
	std::advance(tit, derivedIndex);
	return tit->row;
}


/*
PropertyRow* PropertyTreeModel::defaultType(const char* baseName, const char* derivedTypeName) const
{
	DefaultTypesPoly::const_iterator it = defaultTypesPoly_.find(baseName);
	ASSERT(it != defaultTypesPoly_.end());
	const BaseClass& base = it->second;
	DerivedTypes::const_iterator dit;
	dit = base.types.find(derivedTypeName);
	ASSERT(dit != base.types.end());
	return dit->second();
}
*/

bool PropertyTreeModel::defaultTypeRegistered(const char* baseName, const char* derivedName) const
{
	DefaultTypesPoly::const_iterator it = defaultTypesPoly_.find(baseName);

	if(it != defaultTypesPoly_.end()){
		if(!derivedName)
			return true;
		const BaseClass& base = it->second;

		DerivedTypes::const_iterator dit;
		for(dit = base.types.begin(); dit != base.types.end(); ++dit){
			if(dit->name == derivedName)
				return true;
		}
		return false;
	}
	else
		return false;
}

const ComboStrings& PropertyTreeModel::typeComboStrings(const char* baseTypeName) const
{
	DefaultTypesPoly::const_iterator it = defaultTypesPoly_.find(baseTypeName);

	ASSERT(it != defaultTypesPoly_.end());
	const BaseClass& base = it->second;
	return base.strings;
}

std::string generateDigest(Serializer& ser)
{
	PropertyTreeModel model;
	PropertyOArchive oa(&model);
	ser.serialize(oa);
	return model.root()->digest();
}

}
