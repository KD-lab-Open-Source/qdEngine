#include "stdafx.h"
#include "resource.h"
#include "screen_ele_name_test.h"
#include "qd_interface_screen.h"
#include "qd_interface_element.h"
#include "name_op.h"

screen_ele_name_test::screen_ele_name_test(qdInterfaceScreen* pScreen):
m_pScreen(pScreen)
{
}

screen_ele_name_test::~screen_ele_name_test(void)
{
}

const CString screen_ele_name_test::get_unique_name(const CString& str) const
{
	return ::get_unique_name(str, m_pScreen->element_list());
}
