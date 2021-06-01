#include "moja/modules/chapman_richards/disturbanceeventmodule.h"

#include "moja/modules/chapman_richards/disturbanceevents.h"
#include "moja/modules/chapman_richards/forestevents.h"
#include "moja/modules/chapman_richards/foresttype.h"

#include <moja/flint/eventqueue.h>
#include <moja/flint/flintexceptions.h>
#include <moja/flint/iflintdata.h>
#include <moja/flint/ioperation.h>
#include <moja/flint/ipool.h>
#include <moja/flint/ivariable.h>

#include <moja/notificationcenter.h>
#include <moja/signals.h>

#include <fmt/format.h>

namespace moja {
namespace modules {
namespace chapman_richards {

void DisturbanceEventModule::configure(const DynamicObject& config) {}

void DisturbanceEventModule::subscribe(NotificationCenter& notificationCenter) {
   notificationCenter.subscribe(signals::TimingInit, &DisturbanceEventModule::onTimingInit, *this);
   notificationCenter.subscribe(signals::DisturbanceEvent, &DisturbanceEventModule::disturbanceEventHandler, *this);
}

void DisturbanceEventModule::onTimingInit() {
   atmosphere_ = _landUnitData->getPool("atmosphereCM");

   above_ground_cm_ = _landUnitData->getPool("aboveGroundCM");
   below_ground_cm_ = _landUnitData->getPool("belowGroundCM");
   dead_organic_cm_ = _landUnitData->getPool("deadOrganicCM");

   forest_exists_ = _landUnitData->getVariable("forest_exists");
   forest_age_ = _landUnitData->getVariable("forest_age");
   forest_type_ = _landUnitData->getVariable("forest_type");
}

void DisturbanceEventModule::simulate(const ForestPlantEvent& plant) {
   const bool forest_exists = forest_exists_->value();

   if (forest_exists) {
      BOOST_THROW_EXCEPTION(flint::SimulationError()
                            << flint::Details("Planting occured when forest present.")
                            << flint::LibraryName("moja.modules.chapman_richards")
                            << flint::ModuleName(BOOST_CURRENT_FUNCTION) << flint::ErrorCode(1));
   }

   forest_exists_->set_value(true);
   forest_age_->set_value(0.0);

   const auto forest_types_var = _landUnitData->getVariable("forest_types");
   const auto forest_types = std::static_pointer_cast<const ForestTypeList>(
       forest_types_var->value().extract<std::shared_ptr<flint::IFlintData>>());

   auto forest_type = forest_types->find(plant.forest_type_id);

   _landUnitData->addPool(fmt::format("{}.{}", above_ground_cm_->name(), forest_type->name), "", "", 1.0, 1, 0.0,
                          above_ground_cm_);
   _landUnitData->addPool(fmt::format("{}.{}", below_ground_cm_->name(), forest_type->name), "", "", 1.0, 1, 0.0,
                          below_ground_cm_);
   _landUnitData->addPool(fmt::format("{}.{}", dead_organic_cm_->name(), forest_type->name), "", "", 1.0, 1, 0.0,
                          dead_organic_cm_);

   forest_type_->set_value(std::static_pointer_cast<flint::IFlintData>(forest_type));
}

flint::IPool* DisturbanceEventModule::get_cohort_pool(const std::string& parent, const std::string& forest_type) {
   return _landUnitData->getPool(fmt::format("{}.{}", parent, forest_type));
}

void DisturbanceEventModule::simulate(const ForestClearEvent& thin) {
   const bool forest_exists = forest_exists_->value();

   if (!forest_exists) {
      BOOST_THROW_EXCEPTION(flint::SimulationError()
                            << flint::Details("Clearing occured when no forest present.")
                            << flint::LibraryName("moja.modules.chapman_richards")
                            << flint::ModuleName(BOOST_CURRENT_FUNCTION) << flint::ErrorCode(1));
   }

   if (thin.is_clearing()) {
      forest_exists_->set_value(false);  //	Clearing Thin
      forest_age_->set_value(0.0);
   }

   auto forest_type = std::static_pointer_cast<const ForestType>(
       forest_type_->value().extract<const std::shared_ptr<flint::IFlintData>>());

   auto* above_ground_cm = get_cohort_pool(above_ground_cm_->name(), forest_type->name);
   auto* below_ground_cm = get_cohort_pool(below_ground_cm_->name(), forest_type->name);

   auto operation = _landUnitData->createProportionalOperation();
   operation->addTransfer(above_ground_cm, atmosphere_, 1.0)
            ->addTransfer(below_ground_cm, atmosphere_, 1.0);

   _landUnitData->submitOperation(operation);
}

void DisturbanceEventModule::disturbanceEventHandler(const flint::EventQueueItem* event) {
   const auto disturbance_event = std::static_pointer_cast<const DisturbanceEventBase>(event->_event);
   disturbance_event->simulate(*this);
}

}  // namespace chapman_richards
}  // namespace modules
}  // namespace moja
