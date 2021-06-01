#pragma once

#include "moja/flint/example/base/_modules.base_exports.h"

#include <moja/flint/librarymanager.h>

namespace moja::flint::example::base {

// Instance of common data structure

extern "C" MOJA_LIB_API int getModuleRegistrations(ModuleRegistration* outModuleRegistrations);
extern "C" MOJA_LIB_API int getTransformRegistrations(TransformRegistration* outTransformRegistrations);
extern "C" MOJA_LIB_API int getFlintDataRegistrations(FlintDataRegistration* outFlintDataRegistrations);
extern "C" MOJA_LIB_API int getFlintDataFactoryRegistrations(
    FlintDataFactoryRegistration* outFlintDataFactoryRegistrations);
extern "C" MOJA_LIB_API int getDataRepositoryProviderRegistrations(
    DataRepositoryProviderRegistration* outDataRepositoryProviderRegistration);

}  // namespace moja::flint::example::base
