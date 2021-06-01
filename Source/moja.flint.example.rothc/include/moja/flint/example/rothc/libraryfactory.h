#pragma once

#include "moja/flint/example/rothc/_modules.rothc_exports.h"

#include <moja/flint/librarymanager.h>

namespace moja::flint::example::rothc {

// Instance of common data structure

extern "C" MOJA_LIB_API int getModuleRegistrations(ModuleRegistration* registrations);
extern "C" MOJA_LIB_API int getTransformRegistrations(TransformRegistration* registrations);
extern "C" MOJA_LIB_API int getFlintDataRegistrations(FlintDataRegistration* registrations);
extern "C" MOJA_LIB_API int getFlintDataFactoryRegistrations(FlintDataFactoryRegistration* registrations);
extern "C" MOJA_LIB_API int getDataRepositoryProviderRegistrations(DataRepositoryProviderRegistration* registrations);

}  // namespace moja::flint::example::rothc
