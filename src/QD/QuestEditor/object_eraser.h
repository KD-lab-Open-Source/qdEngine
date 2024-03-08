
#ifndef __OBJECT_ERASER_H_INCLUDED__
#define __OBJECT_ERASER_H_INCLUDED__

namespace axl
{
	template <class _Handle> struct object_eraser 
	{
		object_eraser(){}
		object_eraser (_Handle* _ptr) 
		{
			delete _ptr;			
		}
		void operator()(_Handle* _ptr){
			delete _ptr;
		}
	};


	// handle
	template <> struct object_eraser<HANDLE> {
		object_eraser(){}
		object_eraser(HANDLE handle) {
			CloseHandle(handle);
		}
		void operator()(HANDLE handle) {
			CloseHandle(handle);
		}
	};

	#ifdef STRICT

	// window
	template <> struct object_eraser<HWND> {
		object_eraser(){}
		object_eraser(HWND hWnd) {
			DestroyWindow(hWnd);
		}
		void operator() (HWND hWnd) {
			DestroyWindow(hWnd);
		}
	};

	// device context
	template <> struct object_eraser<HDC> {
		object_eraser(){}
		object_eraser(HDC hDC) {
			DeleteDC(hDC);
		}
		void operator()(HDC hDC) {
			DeleteDC(hDC);
		}
	};
	
	// GDI objects
	
	// can't define eraser for HGDIOBJECT
	// because it is declared like HANDLE (as void *)
	
	// pen
	template <> struct object_eraser<HPEN> {
		object_eraser(){}
		object_eraser(HPEN hPen) {
			DeleteObject(hPen);
		}
		void operator()(HPEN hPen) {
			DeleteObject(hPen);
		}
	};
	// brush
	template <> struct object_eraser<HBRUSH> {
		object_eraser(){}
		object_eraser(HBRUSH hBrush) {
			DeleteObject(hBrush);
		}
		void operator()(HBRUSH hBrush) {
			DeleteObject(hBrush);
		}
	};
	// font
	template <> struct object_eraser<HFONT> {
		object_eraser(){}
		object_eraser(HFONT hFont) {
			DeleteObject(hFont);
		}
		void operator()(HFONT hFont) {
			DeleteObject(hFont);
		}
	};
	// bitmap
	template <> struct object_eraser<HBITMAP> {
		object_eraser(){}
		object_eraser(HBITMAP hBitmap) {
			DeleteObject(hBitmap);
		}
		void operator()(HBITMAP hBitmap) {
			DeleteObject(hBitmap);
		}
	};
	// region
	template <> struct object_eraser<HRGN> {
		object_eraser(){}
		object_eraser(HRGN hRgn){
			DeleteObject(hRgn);
		}
		void operator()(HRGN hRgn){
			DeleteObject(hRgn);
		}
	};
	// palette
	template <> struct object_eraser<HPALETTE> {
		object_eraser(){}
		object_eraser(HPALETTE hPalette) {
			DeleteObject(hPalette);
		}
		void operator()(HPALETTE hPalette) {
			DeleteObject(hPalette);
		}
	};

	// menu
	template <> struct object_eraser<HMENU> {
		object_eraser(){}
		object_eraser(HMENU hMenu) {
			DestroyMenu(hMenu);
		}
		void operator()(HMENU hMenu) {
			DestroyMenu(hMenu);
		}
	};

	// icon
	template <> struct object_eraser<HICON> {
		object_eraser(){}
		object_eraser(HICON hIcon){
			DestroyIcon(hIcon);
		}
		void operator()(HICON hIcon){
			DestroyIcon(hIcon);
		}
	};

	// accelerator table
	template <> struct object_eraser<HACCEL> {
		object_eraser(){}
		object_eraser(HACCEL hAccel) {
			DestroyAcceleratorTable(hAccel);
		}
		void operator()(HACCEL hAccel) {
			DestroyAcceleratorTable(hAccel);
		}
	};

	// registry key
	template <> struct object_eraser<HKEY> {
		object_eraser(){}
		object_eraser(HKEY hKey) {
			RegCloseKey(hKey);
		}
		void operator()(HKEY hKey) {
			RegCloseKey(hKey);
		}
	};

	#endif // STRICT
}

#endif // __OBJECT_ERASER_H_INCLUDED__
