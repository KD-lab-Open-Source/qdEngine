#ifndef __KDW_TREE_PATH_H_INCLUDED__
#define __KDW_TREE_PATH_H_INCLUDED__

namespace kdw{

struct TreePathLeaf{
	TreePathLeaf(int _index)
	: name("")
	, index(_index)
	{
	}
	TreePathLeaf(const char* _name = "", int _index = -1)
	: name(_name ? _name : "")
	, index(_index)
	{
	}
	bool operator==(const TreePathLeaf& rhs) const{
		return name == rhs.name && index == rhs.index;
	}
	bool operator!=(const TreePathLeaf& rhs) const{
		return name != rhs.name || index != rhs.index;
	}
	std::string name;
	int index;
	void serialize(Archive& ar);
};
typedef std::vector<TreePathLeaf> TreePath;
typedef std::vector<TreePath> TreePathes;

struct TreeSelection : std::vector<TreePath>{
	typedef std::vector<TreePath> Base;
	bool operator==(const TreeSelection& rhs){
		if(size() != rhs.size())
			return false;
		for(int i = 0; i < size(); ++i)
			if((*this)[i] != rhs[i])
				return false;
	}
	bool serialize(Archive& ar, const char* name, const char* nameAlt);
};

struct TreeState{
	TreeState()
	: scrollOffset(0)
	{}
	void serialize(Archive& ar);
	int scrollOffset;
	TreePath focusedRow;
	TreeSelection selection;
	std::vector<char> expanded;
};

}
#endif
