#ifndef __REFERENCE_TREE_BUILDER_H_INCLUDED__
#define __REFERENCE_TREE_BUILDER_H_INCLUDED__

#include "Serialization\StringTable.h"
#include "kdw/TreeSelector.h"

namespace kdw{

template<class ReferenceType>
class ReferenceTreeBuilder : public TreeBuilder {
public:
	ReferenceTreeBuilder(ReferenceType& reference)
	: reference_(reference)
	{}
protected:
	ReferenceType& reference_;

	typedef typename ReferenceType::Table LibraryType;

    Object* buildTree(Object* root){
		typedef StaticMap<std::string, Object*> Groups;
        Groups groups;
        groups[""] = root;
		Object* result = 0;

        LibraryType& library = LibraryType::instance();
		
        LibraryType::Strings& strings = const_cast<LibraryType::Strings&>(library.strings());
        LibraryType::Strings::iterator it;

		if(!ReferenceType::canAlwaysBeDerefenced()){
			static int temp;
			Object* o = root->add(new Object(TRANSLATE("[ Пустая ссылка ]"), ""));
			if(strcmp(reference_.c_str(), "") == 0)
				result = o; 
		}

		if(const char* groupsComboList = library.editorGroupsComboList()){
			ComboStrings groupsCombo;
			splitComboList(groupsCombo, groupsComboList);
			ComboStrings::iterator cit;
			FOR_EACH(groupsCombo, cit){
				groups[*cit] = root->add(new Object(cit->c_str(), 0, true));
			}
		}

        int index = 0;
        FOR_EACH(strings, it) {
            std::string name = it->c_str();
            if(reference_.validForComboList(*ReferenceType(name.c_str()))){
                Object* parent = root;

				std::string group = library.editorGroupName(index);
                Groups::iterator git = groups.find(group);
                if(git != groups.end())
                    parent = git->second;
				else
					parent = groups[group] = parent->add(new Object(group.c_str(), 0, true));

                Object* o = parent->add(new Object(name.c_str(), name.c_str()));
				if(strcmp(reference_.c_str(), name.c_str()) == 0)
					result = o;
            }
            ++index;
        }
		return result;
    }
	bool select(Object* object){
		typedef typename LibraryType::StringType String;
		if(object->selectable()){
			reference_ = ReferenceType(object->key());
			return true;
		}
		else
			return false;
	}
};

}

#endif