// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__C460B41B_FF7D_4EEA_B6AD_BB72CE058C83__INCLUDED_)
#define AFX_STDAFX_H__C460B41B_FF7D_4EEA_B6AD_BB72CE058C83__INCLUDED_

#if _MSC_VER > 1000
//#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#define _WIN32_WINNT	0x0500

#ifdef _DEBUG
#define DBGCHECK _ASSERTE(_CrtCheckMemory())
#else
#define DBGCHECK
#endif

//#define   _STLP_NO_OWN_NAMESPACE 1
//#define   _STLP_NO_NEW_IOSTREAMS 1
# define _STLP_USE_OWN_NAMESPACE 1
# define _STLP_REDEFINE_STD 1

#include <Additional\addition.h>
#pragma warning (disable : 4666)

#pragma warning(disable : 4786 4503)
#pragma warning(disable : 4018)
#pragma warning(disable : 4244)

// My warning disabling
#pragma warning(disable : 4204)	// is missing debugging information for refreencing module

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT
#include <afxtempl.h>

#include <ASSERT.h>
#include <afxdlgs.h>

#include <functional>
#include <algorithm>
#include <string>
#include <list>
#include <vector>
#include <hash_map>

#include <XUtil.h>
#include "XMath\XMath.h"
#include "XMath\XMathLib.h"

#include "qd_fwd.h"
#include "app_log.h"
#include "qd_save_stream.h"
#include "qd_game_scene.h"
#include "qd_game_dispatcher.h"

#include "Custom Controls/CJFlatComboBox.h"

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //!defined(AFX_STDAFX_H__C460B41B_FF7D_4EEA_B6AD_BB72CE058C83__INCLUDED_)
