/*----------------------------------------------------------------------
       John Robbins - Microsoft Systems Journal Bugslayer Column
------------------------------------------------------------------------

The BugslayerUtil.DLL file contains various routines that are useful
across various projects, not just those for a single column.  I will
be adding to the DLL in future columns.

HISTORY     :

FEB '98 - Initial version.  This also incorporated the October '97
          memory dumper and validator library.

APR '98 - Added the CSymbolEngine class in SymbolEngine.h

AUG '98 - Added the Crash Handler functions to assist in crash
          information.
        - Set all exported functions to __stdcall.

OCT '98 - Fixed HookImportedFunctionsByName to no patch addresses
          about 2GB on Win95.  Also changed the function so that
          the user can pass NULL for the returned orignal function
          array and NULL for the number of functions hooked.
        - Added the IsNT function to the export list.

FEB '99 - Updated the CrashHandler code to work on Win9x.
        - Updated CSymbolEngine to support getting the versions of
          IMAGEHLP.DLL as well as the PDB reader DLL, MSDBI.DLL or
          MSPDB50.DLL.
        - Moved to VC6.
        - Fixed bug reported by Frank McGrath in
          HookImportedFunctionsByName.
        - Added the ASSERT and SUPERASSERT cool macros all in
          DiagAssert.h/.cpp.
        - Removed the old ASSERT/TRACE stuff in MSJDBG.H.

----------------------------------------------------------------------*/

#ifndef _BUGSLAYERUTIL_H
#define _BUGSLAYERUTIL_H

/*//////////////////////////////////////////////////////////////////////
                                Includes
//////////////////////////////////////////////////////////////////////*/
// If windows.h has not yet been included, include it now.
#ifndef _INC_WINDOWS
#include <windows.h>
#endif

// Force imagehlp in.
#include <imagehlp.h>


/*//////////////////////////////////////////////////////////////////////
                            Special Defines
//////////////////////////////////////////////////////////////////////*/
// The defines that set up how the functions or classes are exported or
//  imported.
#ifndef BUGSUTIL_DLLINTERFACE
#ifdef BUILDING_BUGSUTILITY_DLL
#define BUGSUTIL_DLLINTERFACE __declspec ( dllexport )
#else
#define BUGSUTIL_DLLINTERFACE __declspec ( dllimport )
#endif  // BUILDING_BUGSTILITY_DLL
#endif  // BUGSUTIL_DLLINTERFACE

#undef BUGSUTIL_DLLINTERFACE
#define BUGSUTIL_DLLINTERFACE

/*//////////////////////////////////////////////////////////////////////
                            Special Includes
//////////////////////////////////////////////////////////////////////*/
// Include the headers that do the memory dumping and validation
//  routines.
#include "MemDumperValidator.h"

// The CrashHandler routines.
#include "CrashHandler.h"

// The ASSERT routines.
#include "DiagAssert.h"


/*//////////////////////////////////////////////////////////////////////
                      C Function Declaration Area
                                 START
//////////////////////////////////////////////////////////////////////*/
#ifdef __cplusplus
extern "C" {
#endif  // _cplusplus

/*----------------------------------------------------------------------
FUNCTION        :   GetLoadedModules
DISCUSSION      :
    For the specified process id, this function returns the HMODULES for
all modules loaded into that process address space.  This function works
for both NT and 95.
PARAMETERS      :
    dwPID        - The process ID to look into.
    uiCount      - The number of slots in the paModArray buffer.  If
                   this value is 0, then the return value will be TRUE
                   and puiRealCount will hold the number of items
                   needed.
    paModArray   - The array to place the HMODULES into.  If this buffer
                   is too small to hold the result and uiCount is not
                   zero, then FALSE is returned, but puiRealCount will
                   be the real number of items needed.
    puiRealCount - The count of items needed in paModArray, if uiCount
                   is zero, or the real number of items in paModArray.
RETURNS         :
    FALSE - There was a problem, check GetLastError.
    TRUE  - The function succeeded.  See the parameter discussion for
            the output parameters.
----------------------------------------------------------------------*/
BOOL BUGSUTIL_DLLINTERFACE __stdcall
                           GetLoadedModules ( DWORD     dwPID        ,
                                              UINT      uiCount      ,
                                              HMODULE * paModArray   ,
                                              LPUINT    puiRealCount  );

/*----------------------------------------------------------------------
FUNCTION        :   AllocAndFillProcessModuleList
DISCUSSION      :
    Hides the work to get the loaded module list for the current
process.  This function returns a pointer to memory allocated from the
specified heap and that memory must be deallocated with a call to
HeapFree from the same heap.
PARAMETERS      :
    hHeap    - The heap to allocate from.
    puiCount - The total elements in the returned array.
RETURNS         :
    NULL  - There was a problem.
    !NULL - The block of memory holding the HMODULEs.
----------------------------------------------------------------------*/
HMODULE * /*BUGSUTIL_DLLINTERFACE*/ __stdcall
                     AllocAndFillProcessModuleList ( HANDLE hHeap    ,
                                                     LPUINT puiCount  );

typedef struct tag_HOOKFUNCDESCA
{
    // The name of the function to hook.
    LPCSTR szFunc   ;
    // The procedure to blast in.
    PROC   pProc    ;
} HOOKFUNCDESCA , * LPHOOKFUNCDESCA ;

typedef struct tag_HOOKFUNCDESCW
{
    // The name of the function to hook.
    LPCWSTR szFunc   ;
    // The procedure to blast in.
    PROC    pProc    ;
} HOOKFUNCDESCW , * LPHOOKFUNCDESCW ;

#ifdef UNICODE
#define HOOKFUNCDESC   HOOKFUNCDESCW
#define LPHOOKFUNCDESC LPHOOKFUNCDESCW
#else
#define HOOKFUNCDESC   HOOKFUNCDESCA
#define LPHOOKFUNCDESC LPHOOKFUNCDESCA
#endif  // UNICODE

/*----------------------------------------------------------------------
FUNCTION        :   HookImportedFunctionsByName
DISCUSSION      :
    Hooks the specified functions imported into hModule by the module
indicated by szImportMod.  This function can be used to hook from one
to 'n' of the functions imported.
    The techniques used in the function are slightly different than
that shown by Matt Pietrek in his book, "Windows 95 System Programming
Secrets."  He uses the address of the function to hook as returned by
GetProcAddress.  Unfortunately, while this works in almost all cases, it
does not work when the program being hooked is running under a debugger
on Windows95 (and presumably, Windows98).  The problem is that
GetProcAddress under a debugger returns a "debug thunk," not the address
that is stored in the Import Address Table (IAT).
    This function gets around that by using the real thunk list in the
PE file, the one not bashed by the loader when the module is loaded and
fixed up, to find where the named import is located.  Once the named
import is found, then the original table is blasted to make the hook.
As the name implies, this function will only hook functions imported by
name.
PARAMETERS      :
    hModule     - The module where the imports will be hooked.
    szImportMod - The name of the module whose functions will be
                  imported.
    uiCount     - The number of functions to hook.  This is the size of
                  the paHookArray and paOrigFuncs arrays.
    paHookArray - The array of function descriptors that list which
                  functions to hook.  At this point, the array does not
                  have to be in szFunc name order.  Also, if a
                  particular pProc is NULL, then that item will just be
                  skipped.  This makes it much easier for debugging.
    paOrigFuncs - The array of original addresses that were hooked.  If
                  a function was not hooked, then that item will be
                  NULL.  This parameter can be NULL if the returned
                  information is not needed.
    puiHooked   - Returns the number of functions hooked out of
                  paHookArray.  This parameter can be NULL if the
                  returned information is not needed.
RETURNS         :
    FALSE - There was a problem, check GetLastError.
    TRUE  - The function succeeded.  See the parameter discussion for
            the output parameters.
----------------------------------------------------------------------*/
BOOL BUGSUTIL_DLLINTERFACE __stdcall
        HookImportedFunctionsByName ( HMODULE         hModule     ,
                                      LPCSTR          szImportMod ,
                                      UINT            uiCount     ,
                                      LPHOOKFUNCDESCA paHookArray ,
                                      PROC *          paOrigFuncs ,
                                      LPUINT          puiHooked    ) ;

/*----------------------------------------------------------------------
FUNCTION        :   BSUGetModuleBaseName
DISCUSSION      :
    Returns the base name of the specified module in a manner that is
portable between NT and Win95/98.
PARAMETERS      :
    hProcess   - The handle to the process.  In Win95/98 this is
                 ignored.
    hModule    - The module to look up.  If this is NULL, then the
                 module returned is the executable.
    lpBaseName - The buffer that recieves the base name.
    nSize      - The size of the buffer.
RETURNS         :
    !0 - The length of the string copied to the buffer.
    0  - The function failed.  To get extended error information,
         call GetLastError
----------------------------------------------------------------------*/
DWORD BUGSUTIL_DLLINTERFACE __stdcall
      BSUGetModuleBaseName ( HANDLE  hProcess   ,
                             HMODULE hModule    ,
                             LPTSTR  lpBaseName ,
                             DWORD   nSize       ) ;

/*----------------------------------------------------------------------
FUNCTION        :   IsNT
DISCUSSION      :
    Returns TRUE if the operating system is NT.  I simply got tired of
always having to call GetVersionEx each time I needed to check.
Additionally, I also need to check the OS inside loops so this function
caches the results so it is faster.
PARAMETERS      :
    None.
RETURNS         :

----------------------------------------------------------------------*/
BOOL BUGSUTIL_DLLINTERFACE __stdcall IsNT ( void ) ;


#ifdef __cplusplus
}
#endif  // _cplusplus
/*//////////////////////////////////////////////////////////////////////
                                  END
                      C Function Declaration Area
//////////////////////////////////////////////////////////////////////*/

/*//////////////////////////////////////////////////////////////////////
                         C++ Only Declarations
                                 START
//////////////////////////////////////////////////////////////////////*/
#ifdef __cplusplus

#include "SymbolEngine.h"

#endif  // __cplusplus
/*//////////////////////////////////////////////////////////////////////
                                  END
                         C++ Only Declarations
//////////////////////////////////////////////////////////////////////*/

#endif  // _BUGSLAYERUTIL_H


