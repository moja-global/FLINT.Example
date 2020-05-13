#ifndef MOJA_${LIBNAME_EXPORT_SUBMODULE}_LIB_CONFIGURATION_INCLUDED
#define MOJA_${LIBNAME_EXPORT_SUBMODULE}_LIB_CONFIGURATION_INCLUDED

#include "${CMAKE_CURRENT_SOURCE_DIR}/include/moja/modules/mulliongroup/${PACKAGE}/_modules.${PACKAGE}_exports.h"

#endif // MOJA_${LIBNAME_EXPORT_SUBMODULE}_LIB_CONFIGURATION_INCLUDED


#ifndef ${LIBNAME_EXPORT_SUBMODULE}_EXPORTS_INCLUDED
#define ${LIBNAME_EXPORT_SUBMODULE}_EXPORTS_INCLUDED

//
// The following block is the standard way of creating macros which make exporting
// from a DLL simpler. All files within this DLL are compiled with the ${LIBNAME_EXPORT_SUBMODULE}_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see
// ${LIBNAME_EXPORT_SUBMODULE}_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
//

#if defined(_WIN32) && defined(MOJA_DLL)
#if defined(${LIBNAME_EXPORT}_EXPORTS)
#define ${LIBNAME_EXPORT_SUBMODULE}_API __declspec(dllexport)
#else
#define ${LIBNAME_EXPORT_SUBMODULE}_API __declspec(dllimport)
#endif
#endif

#if !defined(${LIBNAME_EXPORT_SUBMODULE}_API)
#if !defined(${LIBNAME_EXPORT_SUBMODULE}_NO_GCC_API_ATTRIBUTE) && defined (__GNUC__) && (__GNUC__ >= 4)
#define ${LIBNAME_EXPORT_SUBMODULE}_API __attribute__ ((visibility ("default")))
#else
#define ${LIBNAME_EXPORT_SUBMODULE}_API
#endif
#endif

#endif // ${LIBNAME_EXPORT_SUBMODULE}_EXPORTS_INCLUDED


#ifndef MOJA_GLOBAL_INCLUDES
#define MOJA_GLOBAL_INCLUDES

//
// Pull in basic definitions
//
#include <Poco/Bugcheck.h>
#include "moja/types.h"
#include <string>
#include <memory>

#endif // MOJA_GLOBAL_INCLUDES
