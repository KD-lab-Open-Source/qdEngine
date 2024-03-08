#ifndef __TEST_DATA_H_INCLUDED__
#define __TEST_DATA_H_INCLUDED__

#include <string>
#include <vector>

#include "xMath\xMath.h"
#include "Handle.h"

#include "XMath\ComboListColor.h"
#include "kdw/PropertyEditor.h"
#include "Serialization\RangedWrapper.h"
#include "Serialization\Serialization.h"
#include "Serialization\SerializationFactory.h"
#include "Serialization\StringTableBase.h"
#include "Serialization\Decorators.h"
#include "sKey.h"

struct sColor4c{
	unsigned char r, g, b, a;
	void serialize(Archive& ar){
		ar.serialize(r, "r", "&r");
		ar.serialize(g, "g", "&g");
		ar.serialize(b, "b", "&b");
		ar.serialize(a, "a", "&a");
	}
};

class TestReference 
{
public:
	const char* c_str() const { return "Test"; }
	void serialize(Archive& ar) {
		ar.serialize(key, "key", "Ключ");
	}
	int key;
};

enum USELESS_ENUM {
    FIRST_VALUE,
    SECOND_VALUE,
    THIRD_VALUE,
    LAST_VALUE
};

enum USELESS_FLAGS {
	FIRST_FLAG  = 1 << 1,
	SECOND_FLAG = 1 << 2,
	THIRD_FLAG  = 1 << 3
};

class TestBase : public PolymorphicBase{
public:
	std::string base_member;
    float       base_float;
	bool		base_flag;

	TestBase () {
        base_member = "New string";
        base_float = 3.1415926f;
		base_flag = false;
    }
	virtual ~TestBase () {}

	virtual void serialize (Archive& ar) {
		ar.serialize(NotDecorator(base_flag), "base_flag", "^");
		/*
		if(base_flag){
			ar.serialize(base_member, "base_member", "Название");
			ar.serialize(base_float,  "base_float", "Флоат в базовом класе");
		}
		*/
	}
};

class TestDerivedA : public TestBase
{
public:
	TestDerivedA (){
		derived_a_member = "";
	}
	virtual ~TestDerivedA () {}

	std::string derived_a_member;

	virtual void serialize (Archive& ar) {
		ar.serialize(derived_a_member, "derived_a_member", "&Строка в производном классе");
		__super::serialize (ar);
	}
};

class TestDerivedB : public TestBase
{
public:
	Vect2f derived_b_member;
	std::vector<ShareHandle <TestBase> > test_poly;

	std::string one_string;
	std::string second_string;

	TestDerivedB ()
	{
		derived_b_member = Vect2f::ZERO;
	}
	virtual ~TestDerivedB () {}

	virtual void serialize (Archive& ar) {
		__super::serialize (ar);
		
		ar.serialize(derived_b_member, "derived_b_member", "Derived B Member");
		//ar.serialize(test_poly, "test_poly", "Vector of TestBase*");
		ar.serialize(one_string, "one_string", "&One String");
		ar.serialize(second_string, "second_string", "Second String");
	}
};

enum SwitcherType{
	SWITCHER_AND,
	SWITCHER_OR
};

class TestSwitcher : public TestBase{
public:
	TestSwitcher()
	: type(SWITCHER_AND)
	{
	}
	void serialize(Archive& ar){
		__super::serialize(ar);
		ar.serialize(type, "type", "<");
		ar.serialize(children, "children", "^Children");
	}
	SwitcherType type;
	std::vector<ShareHandle <TestBase> > children;
	
};


struct TestChild
{
	/*
private:
	TestChild (const TestChild& _orig) {
	}
	*/
public:
	TestChild () 
	{
	}

	float a, b;
	std::vector< ShareHandle<TestBase> > poly_vector;
    void serialize (Archive& ar) {
        ar.serialize(a, "a", "&A");
        ar.serialize(b, "b", "&B");
		ar.serialize(poly_vector, "poly_vector", "Poly Vector");
	}
	
};

struct SomeSortOfReference {
	ComboListString combo_string;

	SomeSortOfReference ()
		: combo_string ("First|Second|Third", "Second")
	{}

	bool serialize (Archive& ar, const char* name, const char* nameAlt) {
		return ar.serialize(combo_string, name, nameAlt);
	}
};

enum ZoneType {
	ZONE_GENERATOR,
	ZONE_SIMPLE,
	ZONE_WALKING_EFFECT
};

struct TestData
{
	TestData () {
		enableSerialization_ = true;
		type = ZONE_WALKING_EFFECT;
		enum_value = FIRST_VALUE;
		labels.push_back ("First String");
		labels.push_back ("Second String");
		labels.push_back ("Last One");
		single_child = 0;
		comboList_ = ComboListString("First|Second|Third|ComboListString!", "Second");
		hotkey = sKey(VK_RETURN | sKey::SHIFT);

		ColorContainer colors;
		colors.push_back(Color4f(1.0f, 0.0f, 0.0f, 0.8f));
		colors.push_back(Color4f(0.0f, 1.0f, 0.0f, 1.0f));
		colors.push_back(Color4f(0.0f, 0.0f, 0.5f, 0.6f));
		colors.push_back(Color4f(0.0f, 0.0f, 0.0f, 0.2f));
		comboListColor.setComboList(colors);

		/*
		poly_vector.push_back(new TestBase);
		poly_vector.push_back(0);
		poly_vector.push_back(new TestDerivedB);
		poly_vector.push_back(new TestDerivedA);
		*/

		//gradient.GetOrCreateKey(0.0f)->set(1.0f, 1.0f, 1.0f, 1.0f);
		//gradient.GetOrCreateKey(1.0f)->set(0.0f, 1.0f, 0.0f, 1.0f);
		//child.clear ();
		//child.push_back (new TestBase ());
	}
	virtual ~TestData () {
		//delete child;
	}

	ComboListString comboList_;
	//CKeyColor gradient;
	bool mega_boolean;
	//std::vector< ShareHandle<TestBase> > child;
	TestReference test_reference;
	ShareHandle<TestBase> single_child;
    ShareHandle<TestBase> smart_child;
	typedef std::vector< ShareHandle<TestBase> > TestBases;
	TestBases poly_vector;
    std::string name;
	Vect2f position;
	Vect2f size;
	SomeSortOfReference reference;
	USELESS_ENUM enum_value;
	BitVector<USELESS_FLAGS> flags;
    float fvalue;
	sColor4c fore_color;
	sColor4c back_color;
	ComboListColor color_;
	ComboListColor comboListColor;

	std::list<TestData> childs;
	std::list<TestBase> childsBase;
	std::list<std::string> labels;

	int int_member;

	bool enableSerialization_;
	char   char_value;
    short  short_value;
    int    int_value;
    long   long_value;
    float  float_value;
    double double_value;

	ZoneType type;
	sKey hotkey;

	wstring uniName; 

	TestBase base;
	void serialize (Archive& ar) {
		bool order = mega_boolean;
		ar.serialize(HLineDecorator(), "hline1", "<");
		ar.serialize(single_child, "single_child", "<Условие");
		ar.serialize(HLineDecorator(), "hline2", "<");
		ar.serialize(ButtonDecorator("Срочно нажми меня!"), "button", "1: ");
		ButtonDecorator button("Вторая кнопочка!");
		ar.serialize(button, "button", "2: ");
		if(button){
			kdw::edit(Serializer(*this), "testStateNested");
		}
		if(order)
			ar.serialize(ButtonDecorator("Третья кнопочка!"), "button", "3: ");

		ar.serialize(hotkey, "hotkey", "<");
		ar.serialize(base, "base", "Base");
		ar.serialize(enableSerialization_, "enableSerialization", "Включить");
		if(enableSerialization_){
			ar.serialize(comboList_, "comboList", "^КомбоЛистСтринг");
			ar.serialize(childs, "childs", "Дочерние элементы");

			TestBases::iterator it;
			for(it = poly_vector.begin(); it != poly_vector.end(); ++it){
				if(TestBase* base = *it){
					ASSERT(base->numRef() == 1);
				}
			}

			ar.serialize(uniName, "uniName", "uniName");
			ar.serialize(poly_vector, "poly_vector", "Вектор полиморфных элементов");
			ar.serialize(childsBase, "childsBase", "Вектор простых элементов");
			ar.serialize(RangedWrapperf (fvalue, -15.0f, 15.0f, 0.5f), "fvalue", "Значение");
			ar.serialize(mega_boolean, "mega_boolean", 0);
			ar.serialize(name, "name", "Имя");
			ar.serialize(position, "position", "Положение");
			ar.serialize(size, "size", "Размер");

			ar.serialize(flags, "flags", "!Флаги");

			ar.serialize(type, "type", "Тип");

			vector<Color4f> comboList;
			comboList.push_back(Color4f::BLACK);
			comboList.push_back(Color4f::RED);
			comboList.push_back(Color4f::GREEN);
			color_ = ComboListColor(comboList, Color4f::GREEN);
			ar.serialize(color_, "color_", "color_");

			//ar.serialize(gradient, "gradient", "Градиент");

			ar.serialize(back_color, "backColor", 0);
			ar.serialize(fore_color, "foreColor", 0);
			ar.serialize(comboListColor, "comboListColor", 0);

			if(ar.isInput() || !ar.isEdit() || type == ZONE_GENERATOR) {
				ar.serialize(char_value, "char_value", "Символ");
				ar.serialize(short_value, "short_value", "Короткое целое");
				ar.serialize(int_value, "int_value", "Целое");
				ar.serialize(long_value, "long_value", "!Длинное целое");
				ar.serialize(float_value, "float_value", "С плавающей запятой");
				ar.serialize(double_value, "double_value", "С плавающей запятой, двойной точности");
			}

			if(!order)
				ar.serialize(ButtonDecorator("Третья кнопочка!"), "button", "3: ");

			if(ar.isInput() || !ar.isEdit() || type == ZONE_WALKING_EFFECT) {
				ar.serialize(labels, "labels", "Метки");
			}

			if(ar.isInput() || !ar.isEdit() || type == ZONE_SIMPLE) {
				ar.serialize(enum_value, "enum_value", "Значение перечисления");
				//ar.serialize(test_reference, "test_reference", "Reference");
			}
		}
	}
};

typedef StringTableBasePolymorphic<TestBase> TestElement ;
typedef StringTable<TestElement> TestLibrary;

#endif
