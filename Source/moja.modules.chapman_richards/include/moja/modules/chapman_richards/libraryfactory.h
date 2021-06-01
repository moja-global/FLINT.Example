

#pragma once

#include <moja/flint/librarymanager.h>

namespace moja::modules::chapman_richards {

extern "C" MOJA_LIB_API int getModuleRegistrations(flint::ModuleRegistration* registrations);
extern "C" MOJA_LIB_API int getTransformRegistrations(flint::TransformRegistration* registrations);
extern "C" MOJA_LIB_API int getFlintDataRegistrations(flint::FlintDataRegistration* registrations);
extern "C" MOJA_LIB_API int getFlintDataFactoryRegistrations(flint::FlintDataFactoryRegistration* registrations);

}  // namespace moja::modules::chapman_richards
