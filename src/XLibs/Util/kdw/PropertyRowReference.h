#ifndef __KDW_PROPERTY_ROW_FILE_SELECTOR__
#define __KDW_PROPERTY_ROW_FILE_SELECTOR__

#include "kdw/Win32/Drawing.h"
#include "kdw/Win32/Window.h"

#include "kdw/PropertyTree.h"
#include "kdw/PropertyTreeModel.h"
#include "kdw/PropertyRow.h"
#include "kdw/TreeSelector.h"
#include "kdw/ReferenceTreeBuilder.h"
#include "kdw/PopupMenu.h"
#include "Serialization/LibrariesManager.h"
#include "Serialization/SerializationFactory.h"

namespace kdw{

template<class ReferenceType, class BuilderType = ReferenceTreeBuilder<ReferenceType> >
class PropertyRowReference : public PropertyRowImpl<ReferenceType, PropertyRowReference>, public LibraryLocation{
public:
	PropertyRowReference(void* object, int size, const char* name, const char* nameAlt, const char* typeName)
	: PropertyRowImpl<ReferenceType, PropertyRowReference>(object, size, name, nameAlt, typeName)
	{

	}
	PropertyRowReference()
	{
	}

	bool activateOnAdd() const{ return true; }
	bool onActivateWidget(PropertyTree* tree, PropertyRow* hostRow)
	{
		return onActivate(tree);
	}

	bool onActivate(PropertyTree* tree)
	{
		bool shift = bool(GetAsyncKeyState(VK_SHIFT) >> 15);
		if(shift){
			if(location()){
				LibraryBookmark bookmark;
				if(getBookmark(bookmark)){
					tree->onFollowReference(bookmark);
					return true;
				}
				return false;
			}
		}
		else{
			TreeSelectorDialog dialog(tree);
			BuilderType builder(value());
			dialog.setBuilder(&builder);
			if(dialog.showModal() == kdw::RESPONSE_OK){
				tree->model()->rowChanged(this);
				return true;
			}
		}
		return false;
	}

	const LibraryLocation* location() const{ return this; }
	

	bool getBookmark(LibraryBookmark& bookmark) const{
		const char* libraryName = ReferenceType::editorLibraryName();
		if(libraryName && libraryName[0]){
			EditorLibraryInterface* library = LibrariesManager::instance().find(libraryName);
			if(library && library->editorFindElement(value().c_str()) != -1){
				bookmark = LibraryBookmark(libraryName, value().c_str(), ComboStrings());
				return true;
			}
		}
		return false;
	}

	bool onContextMenu(PopupMenuItem& root, PropertyTree* tree)
	{
		LibraryBookmark bookmark;
		if(getBookmark(bookmark)){
			root.add(TRANSLATE("Следовать по ссылке..."), bookmark)
				.connect(tree, &PropertyTree::onFollowReference);
		}
		return __super::onContextMenu(root, tree);
	}

	std::string valueAsString() const{ return value().c_str(); }
};

}

#define REGISTER_REFERENCE(Type) \
	REGISTER_PROPERTY_ROW(Type, kdw::PropertyRowReference<Type>); \
	static bool Type##ReferenceRegistered = kdw::PropertyRowFactory::instance().addReference(typeid(Type).name(), (LibraryInstanceFunc)&Type::Table::instance); 

#endif
