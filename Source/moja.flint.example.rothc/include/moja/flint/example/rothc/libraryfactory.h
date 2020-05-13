#ifndef MOJA_FLINT_EXAMPLE_ROTHC_LIBRARYFACTORY_H_
#define MOJA_FLINT_EXAMPLE_ROTHC_LIBRARYFACTORY_H_
#pragma once

#include "moja/flint/example/rothc/_modules.rothc_exports.h"
#include <moja/flint/librarymanager.h>

namespace moja {
namespace flint {
namespace example {
namespace rothc {

// Instance of common data structure

extern "C" MOJA_LIB_API int getModuleRegistrations					(moja::flint::ModuleRegistration*					outModuleRegistrations);
extern "C" MOJA_LIB_API int getTransformRegistrations				(moja::flint::TransformRegistration*				outTransformRegistrations);
extern "C" MOJA_LIB_API int getFlintDataRegistrations				(moja::flint::FlintDataRegistration*				outFlintDataRegistrations);
extern "C" MOJA_LIB_API int getFlintDataFactoryRegistrations		(moja::flint::FlintDataFactoryRegistration*			outFlintDataFactoryRegistrations);
extern "C" MOJA_LIB_API int getDataRepositoryProviderRegistrations	(moja::flint::DataRepositoryProviderRegistration*	outDataRepositoryProviderRegistration);

}}}} // moja::flint::example::rothc

#endif	// MOJA_FLINT_EXAMPLE_ROTHC_LIBRARYFACTORY_H_