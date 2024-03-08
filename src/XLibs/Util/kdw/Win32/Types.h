#ifndef __KDW_WIN32_TYPES_H_INCLUDED__
#define __KDW_WIN32_TYPES_H_INCLUDED__

// основные типы <windows.h>:
typedef unsigned long       DWORD;
typedef int                 BOOL;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef float               FLOAT;
typedef long				LONG;

typedef int                 INT;
typedef unsigned int        UINT;

typedef __w64 unsigned int       UINT_PTR;
typedef __w64 long				LONG_PTR;

typedef UINT_PTR            WPARAM;
typedef LONG_PTR            LPARAM;
typedef LONG_PTR            LRESULT;

typedef DWORD   COLORREF;

struct HWND__;
typedef HWND__* HWND;
struct HDC__;
typedef HDC__* HDC;
struct HMENU__;
typedef HMENU__* HMENU;
struct HBITMAP__;
typedef HBITMAP__* HBITMAP;
struct HRGN__;
typedef HRGN__* HRGN;
struct HFONT__;
typedef HFONT__* HFONT;
struct HINSTANCE__;
typedef HINSTANCE__* HINSTANCE;
struct HACCEL__;
typedef HACCEL__* HACCEL;
struct _IMAGELIST;
typedef struct _IMAGELIST* HIMAGELIST;
struct tagRECT;
typedef tagRECT RECT;
// ^^^ это все можно заменить на #include <windows.h>



namespace Win32{

}

#endif
