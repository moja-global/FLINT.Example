#include "moja/modules/chapman_richards/libraryfactory.h"

#include "moja/modules/chapman_richards/commondata.h"

// Modules
#include "moja/modules/chapman_richards/aggregatorerror.h"
#include "moja/modules/chapman_richards/aggregatorlandunit.h"
#include "moja/modules/chapman_richards/buildlandunitmodule.h"
#include "moja/modules/chapman_richards/disturbanceeventmodule.h"
#include "moja/modules/chapman_richards/forestgrowthmodule.h"
#include "moja/modules/chapman_richards/landunitsqlitewriter.h"

// Transforms
#include "moja/modules/chapman_richards/hansenforestcovertransform.h"

// Flint Data
#include "moja/modules/chapman_richards/forestevents.h"
#include "moja/modules/chapman_richards/foresttype.h"
#include "moja/modules/chapman_richards/runstatistics.h"

#include <moja/flint/imodule.h>
#include <moja/flint/mojalibapi.h>

namespace moja::modules::chapman_richards {

extern "C" {

MOJA_LIB_API int getModuleRegistrations(flint::ModuleRegistration* registrations) {
   auto index = 0;
   registrations[index++] = flint::ModuleRegistration{
       "AggregatorError",
       []() -> flint::IModule* { return new AggregatorError(ObjectHolder::Instance().systemSettings); }};
   registrations[index++] = flint::ModuleRegistration{
       "AggregatorLandUnit", []() -> flint::IModule* { return new AggregatorLandUnit(ObjectHolder::Instance()); }};
   registrations[index++] = flint::ModuleRegistration{
       "LandUnitSQLiteWriter", []() -> flint::IModule* { return new LandUnitSQLiteWriter(ObjectHolder::Instance()); }};
   registrations[index++] =
       flint::ModuleRegistration{"ForestGrowthModule", []() -> flint::IModule* { return new ForestGrowthModule(); }};
   registrations[index++] = flint::ModuleRegistration{"DisturbanceEventModule",
                                                      []() -> flint::IModule* { return new DisturbanceEventModule(); }};
   registrations[index++] = flint::ModuleRegistration{
       "BuildLandUnitModule",
       []() -> flint::IModule* { return new BuildLandUnitModule(ObjectHolder::Instance().systemSettings); }};
   return index;
}

MOJA_LIB_API int getTransformRegistrations(flint::TransformRegistration* registrations) {
   auto index = 0;
   registrations[index++] = flint::TransformRegistration{
       "HansenForestCoverTransform", []() -> flint::ITransform* { return new HansenForestCoverTransform(); }};
   return index;
}

MOJA_LIB_API int getFlintDataRegistrations(flint::FlintDataRegistration* registrations) {
   auto index = 0;
   registrations[index++] =
       flint::FlintDataRegistration{"ForestType", []() -> flint::IFlintData* { return new ForestType(); }};
   registrations[index++] =
       flint::FlintDataRegistration{"ForestTypeList", []() -> flint::IFlintData* { return new ForestTypeList(); }};
   registrations[index++] = registrations[index++] = flint::FlintDataRegistration{
       "SimulationUnitData", []() -> flint::IFlintData* { return new SimulationUnitData(); }};
   registrations[index++] =
       flint::FlintDataRegistration{"RunStatistics", []() -> flint::IFlintData* { return new RunStatistics(); }};
   return index;
}

MOJA_LIB_API int getFlintDataFactoryRegistrations(flint::FlintDataFactoryRegistration* registrations) {
   auto index = 0;
   registrations[index++] =
       flint::FlintDataFactoryRegistration{"internal.flint", "EventQueue", &createForestEventsFactory};
   return index;
}
}

}  // namespace moja::modules::chapman_richards
