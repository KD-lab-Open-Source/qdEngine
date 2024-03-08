////////////////////////////////////////////////////////////////////////////////
//
// Additional include for making STL more convenient
//
// Author: Alexandre Kotliar, K-D Lab
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __STL_ADDITION_H__
#define __STL_ADDITION_H__

// For disabling some warning 
#pragma warning( disable : 4786 ) // 'identifier' : identifier was truncated to 'number' characters in the debug information
#pragma warning( disable : 4284 ) // return type for 'identifier::operator –>' is not a UDT or reference to a UDT
#pragma warning( disable : 4800 ) // 'type' : forcing value to bool 'true' or 'false' (performance warning)
// #pragma warning( disable : 4267 )
#pragma warning( disable : 4244 ) // 'variable' : conversion from 'type' to 'type', possible loss of data
#pragma warning( disable : 4018 ) // 'expression' : signed/unsigned mismatch
#pragma warning( disable : 4554 ) // 'operator' : check operator precedence for possible error; use parentheses to clarify precedence
#pragma warning( disable : 4996 ) // 'function': was declared deprecated

#ifdef __ICL
#pragma warning( disable : 880 )
#pragma warning( disable : 1125 )
#endif

#ifndef FOR_EACH
#define FOR_EACH(list, iterator) \
  for(iterator = (list).begin(); iterator != (list).end(); ++iterator)
#endif

#endif // __STL_ADDITION_H__
