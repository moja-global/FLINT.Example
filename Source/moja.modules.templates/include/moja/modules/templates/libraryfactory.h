

#pragma once

#include <moja/flint/librarymanager.h>

namespace moja {
namespace modules {
namespace templates {

extern "C" MOJA_LIB_API int getModuleRegistrations(moja::flint::ModuleRegistration* outModuleRegistrations);
extern "C" MOJA_LIB_API int getTransformRegistrations(moja::flint::TransformRegistration* outTransformRegistrations);
extern "C" MOJA_LIB_API int getFlintDataRegistrations(moja::flint::FlintDataRegistration* outFlintDataRegistrations);
extern "C" MOJA_LIB_API int getFlintDataFactoryRegistrations(
    moja::flint::FlintDataFactoryRegistration* outFlintDataFactoryRegistrations);

}  // namespace templates
}  // namespace modules
}  // namespace moja

