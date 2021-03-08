#include "moja/modules/templates/libraryfactory.h"

#include <moja/flint/imodule.h>
#include <moja/flint/mojalibapi.h>

// Modules
// Transforms
// Flint Data

namespace moja {
namespace modules {
namespace templates {

extern "C" {
// Flint Data

MOJA_LIB_API int getModuleRegistrations(flint::ModuleRegistration* outModuleRegistrations) {
   auto index = 0;
   return index;
}

MOJA_LIB_API int getTransformRegistrations(flint::TransformRegistration* outTransformRegistrations) {
   auto index = 0;
   return index;
}

MOJA_LIB_API int getFlintDataRegistrations(flint::FlintDataRegistration* outFlintDataRegistrations) {
   auto index = 0;
   return index;
}

MOJA_LIB_API int getFlintDataFactoryRegistrations(
    flint::FlintDataFactoryRegistration* outFlintDataFactoryRegistrations) {
   auto index = 0;
   return index;
}
}

}  // namespace templates
}  // namespace modules
}  // namespace moja