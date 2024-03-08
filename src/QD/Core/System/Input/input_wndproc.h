#ifndef __INPUT_WNDPROC_H__
#define __INPUT_WNDPROC_H__

class mouseDispatcher;
class keyboardDispatcher;

namespace input {

//! ��������� ��������� ����� � ����������.
/**
���������� true, ���� ��������� ����������.

�������������� ���������:
WM_KEYDOWN
WM_KEYUP
WM_SYSKEYDOWN
WM_SYSKEYUP
*/
bool keyboard_wndproc(const MSG& msg,keyboardDispatcher* dsp);

//! ��������� ��������� ����.
/**
���������� true, ���� ��������� ����������.

�������������� ���������:
WM_MOUSEMOVE
WM_LBUTTONDOWN
WM_RBUTTONDOWN
WM_LBUTTONUP
WM_RBUTTONUP
*/
bool mouse_wndproc(const MSG& msg,mouseDispatcher* dsp);

};

#endif /* __INPUT_WNDPROC_H__ */