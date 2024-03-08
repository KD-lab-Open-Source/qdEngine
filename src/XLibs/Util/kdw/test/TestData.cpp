#include "StdAfx.h"

#include "TestData.h"
#include "Serialization\Serialization.h"
#include "Serialization\LibraryWrapper.h"
#include "Serialization\EnumDescriptor.h"
#include "Serialization\SerializationFactory.h"

BEGIN_ENUM_DESCRIPTOR(SwitcherType, "SwitcherType")
    REGISTER_ENUM(SWITCHER_AND, "�")
    REGISTER_ENUM(SWITCHER_OR, "���")
END_ENUM_DESCRIPTOR(SwitcherType)

BEGIN_ENUM_DESCRIPTOR(USELESS_ENUM, "Enumeration")
    REGISTER_ENUM(FIRST_VALUE, "First Value")
    REGISTER_ENUM(SECOND_VALUE, 0)
    REGISTER_ENUM(THIRD_VALUE, "Third Value")
    REGISTER_ENUM(LAST_VALUE, "Last Value")
END_ENUM_DESCRIPTOR(USELESS_ENUM)

BEGIN_ENUM_DESCRIPTOR(ZoneType, "��� ����")
	REGISTER_ENUM(ZONE_GENERATOR, "���������")
	REGISTER_ENUM(ZONE_SIMPLE, "������� ����")
	REGISTER_ENUM(ZONE_WALKING_EFFECT, "���������� ������")
END_ENUM_DESCRIPTOR(ZoneType)

BEGIN_ENUM_DESCRIPTOR(USELESS_FLAGS, "Flags")
    REGISTER_ENUM(FIRST_FLAG, "First Flags")
    REGISTER_ENUM(SECOND_FLAG, "Second Flag")
    REGISTER_ENUM(THIRD_FLAG, "Third Flag")
END_ENUM_DESCRIPTOR(USELESS_FLAGS)

REGISTER_CLASS(TestBase, TestSwitcher, "�/���");
REGISTER_CLASS(TestBase, TestBase, "������� �����");
REGISTER_CLASS(TestBase, TestDerivedA, "����������� ����� A");
REGISTER_CLASS(TestBase, TestDerivedB, "����������� ����� B");

/*
KeyFloat::value KeyFloat::none=0;
KeyPos::value KeyPos::none=Vect3f::ZERO;
KeyRotate::value KeyRotate::none=QuatF::ID;
KeyColor::value KeyColor::none=sColor4f(0,0,0,0);
float KeyGeneral::time_delta=0.05f;//� ��������


void KeyGeneral::serialize(Archive& ar)
{
	ar.serialize(time, "time", "time");
}

void KeyColor::serialize(Archive& ar)
{
	KeyGeneral::serialize(ar);
	sColor4f color (r, g, b, a);
	ar.serialize(color, "color", "����");
	r = color.r;
	g = color.g;
	b = color.b;
	a = color.a;
}

void CKeyColor::serialize(Archive& ar)
{
	ar.serialize((vector<KeyColor>&)*this, "color", "color");
}

*/
static const float INV_2_PI=1/(2*M_PI);

