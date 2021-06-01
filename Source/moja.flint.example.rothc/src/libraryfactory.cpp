#include "moja/flint/example/rothc/libraryfactory.h"

// Modules
#include "moja/flint/example/rothc/plantresiduemodule.h"
#include "moja/flint/example/rothc/rothcmodule.h"
#include "moja/flint/example/rothc/soilcovermodule.h"

namespace moja::flint::example::rothc {

// Instance of common data structure

extern "C" {

MOJA_LIB_API int getModuleRegistrations(ModuleRegistration* registrations) {
   int index = 0;
   registrations[index++] =
       ModuleRegistration{"PlantResidueModule", []() -> IModule* { return new PlantResidueModule(); }};
   registrations[index++] = ModuleRegistration{"SoilCoverModule", []() -> IModule* { return new SoilCoverModule(); }};
   registrations[index++] = ModuleRegistration{"RothCModule", []() -> IModule* { return new RothCModule(); }};
   return index;
}

MOJA_LIB_API int getTransformRegistrations(TransformRegistration* registrations) { return 0; }

MOJA_LIB_API int getFlintDataRegistrations(FlintDataRegistration* registrations) { return 0; }

MOJA_LIB_API int getFlintDataFactoryRegistrations(FlintDataFactoryRegistration* registrations) { return 0; }

MOJA_LIB_API int getDataRepositoryProviderRegistrations(DataRepositoryProviderRegistration* registrations) { return 0; }
}

}  // namespace moja::flint::example::rothc
