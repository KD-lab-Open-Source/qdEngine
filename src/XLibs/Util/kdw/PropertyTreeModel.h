#ifndef __PROPERTY_TREE_MODEL_H_INCLUDED__
#define __PROPERTY_TREE_MODEL_H_INCLUDED__


#include "kdw/TreeModel.h"
#include "kdw/PropertyRow.h"

#include "XTL/StaticMap.h"

namespace kdw{
class TreeModel;
class KDW_API PropertyTreeModel : public TreeModel{
public:

	PropertyTreeModel();
	typedef TreeModel Base;
	void clear();
	int columnsCount() const { return 2; }

	bool activateRow(Tree* tree, TreeRow* row, int column);
	bool activateRowIcon(Tree* tree, PropertyRow* row);

	void rowChanged(PropertyRow* row);

	PropertyRow* root();
	void onUpdated();

	// для defaultArchive
	const ComboStrings& typeComboStrings(const char* baseTypeName) const;

	bool defaultTypeRegistered(const char* typeName) const;
	void addDefaultType(PropertyRow* propertyRow, const char* typeName);
	PropertyRow* defaultType(const char* baseName, int derivedIndex) const;

	bool defaultTypeRegistered(const char* typeName, const char* derivedName) const;
	void addDefaultType(PropertyRow* propertyRow, const char* typeName, const char* derivedTypeName, const char* derivedTypeNameAlt);
	PropertyRow* defaultType(const char* typeName) const;
protected:
	
	typedef StaticMap<std::string, ShareHandle<PropertyRow> > DefaultTypes;
	DefaultTypes defaultTypes_;

	struct DerivedClass{
		std::string name;
		ShareHandle<PropertyRow> row;
	};
	typedef std::vector<DerivedClass> DerivedTypes;

	struct BaseClass{
		std::string name;
		ComboStrings strings;
		ComboStrings stringsAlt;
		DerivedTypes types;
	};


	typedef StaticMap<std::string, BaseClass> DefaultTypesPoly;
	DefaultTypesPoly defaultTypesPoly_;
};

}

#endif
