#ifndef __KDW_API_H_INCLUDED__
#define __KDW_API_H_INCLUDED__

#pragma warning(disable : 4251) // class '' needs to have dll-interface to be used by clients of class ''
#pragma warning(disable : 4275) // non dll-interface class '' used as base for dll-interface class ''

#if !defined(KDW_DLL) && (defined(_AFXDLL) || defined(_DLL) || defined(_MTD))
#define KDW_DLL
#endif

#ifndef KDW_DLL
# define KDW_API 
#else
# ifdef KDW_EXPORTED
#  define KDW_API __declspec(dllexport)
# else
#  define KDW_API __declspec(dllimport)
# endif
#endif


#endif
