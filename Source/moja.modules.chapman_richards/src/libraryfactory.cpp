#include "moja/modules/chapman_richards/libraryfactory.h"

#include "moja/modules/chapman_richards/aggregatorerror.h"
#include "moja/modules/chapman_richards/aggregatorlandunit.h"
#include "moja/modules/chapman_richards/buildlandunitmodule.h"
#include "moja/modules/chapman_richards/commondata.h"
#include "moja/modules/chapman_richards/disturbanceeventmodule.h"
#include "moja/modules/chapman_richards/forestevents.h"
#include "moja/modules/chapman_richards/forestgrowthmodule.h"
#include "moja/modules/chapman_richards/foresttype.h"
#include "moja/modules/chapman_richards/hansenforestcovertransform.h"
#include "moja/modules/chapman_richards/landunitsqlitewriter.h"
#include "moja/modules/chapman_richards/runstatistics.h"

#include <moja/flint/imodule.h>
#include <moja/flint/mojalibapi.h>

// Modules
// Transforms
// Flint Data

namespace moja {
namespace modules {
namespace chapman_richards {

extern "C" {
// Flint Data

MOJA_LIB_API int getModuleRegistrations(flint::ModuleRegistration* outModuleRegistrations) {
   auto index = 0;
   outModuleRegistrations[index++] = flint::ModuleRegistration{
       "AggregatorError",
       []() -> flint::IModule* { return new AggregatorError(ObjectHolder::Instance().systemSettings); }};
   outModuleRegistrations[index++] = flint::ModuleRegistration{
       "AggregatorLandUnit", []() -> flint::IModule* { return new AggregatorLandUnit(ObjectHolder::Instance()); }};
   outModuleRegistrations[index++] = flint::ModuleRegistration{
       "LandUnitSQLiteWriter", []() -> flint::IModule* { return new LandUnitSQLiteWriter(ObjectHolder::Instance()); }};
   outModuleRegistrations[index++] =
       flint::ModuleRegistration{"ForestGrowthModule", []() -> flint::IModule* { return new ForestGrowthModule(); }};
   outModuleRegistrations[index++] = flint::ModuleRegistration{
       "DisturbanceEventModule", []() -> flint::IModule* { return new DisturbanceEventModule(); }};
   outModuleRegistrations[index++] = flint::ModuleRegistration{
       "BuildLandUnitModule",
       []() -> flint::IModule* { return new BuildLandUnitModule(ObjectHolder::Instance().systemSettings); }};
   return index;
}

MOJA_LIB_API int getTransformRegistrations(flint::TransformRegistration* outTransformRegistrations) {
   auto index = 0;
   outTransformRegistrations[index++] = flint::TransformRegistration{
       "HansenForestCoverTransform", []() -> flint::ITransform* { return new HansenForestCoverTransform(); }};
   return index;
}

MOJA_LIB_API int getFlintDataRegistrations(flint::FlintDataRegistration* outFlintDataRegistrations) {
   auto index = 0;
   outFlintDataRegistrations[index++] =
       flint::FlintDataRegistration{"ForestType", []() -> flint::IFlintData* { return new ForestType(); }};
   outFlintDataRegistrations[index++] =
       flint::FlintDataRegistration{"ForestTypeList", []() -> flint::IFlintData* { return new ForestTypeList(); }};
   outFlintDataRegistrations[index++] = outFlintDataRegistrations[index++] = flint::FlintDataRegistration{
       "SimulationUnitData", []() -> flint::IFlintData* { return new SimulationUnitData(); }};
   outFlintDataRegistrations[index++] =
       flint::FlintDataRegistration{"RunStatistics", []() -> flint::IFlintData* { return new RunStatistics(); }};
   return index;
}

MOJA_LIB_API int getFlintDataFactoryRegistrations(
    flint::FlintDataFactoryRegistration* outFlintDataFactoryRegistrations) {
   auto index = 0;
   outFlintDataFactoryRegistrations[index++] =
       flint::FlintDataFactoryRegistration{"internal.flint", "EventQueue", &createForestEventsFactory};
   return index;
}
}

}  // namespace chapman_richards
}  // namespace modules
}  // namespace moja