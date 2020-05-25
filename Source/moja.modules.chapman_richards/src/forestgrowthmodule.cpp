#include "moja/modules/chapman_richards/forestgrowthmodule.h"

#include "moja/modules/chapman_richards/foresttype.h"

#include <moja/flint/flintexceptions.h>
#include <moja/flint/iflintdata.h>
#include <moja/flint/ioperation.h>
#include <moja/flint/ivariable.h>
#include <moja/flint/operationdatapackage.h>
#include <moja/flint/timing.h>

#include <moja/notificationcenter.h>
#include <moja/signals.h>

#include <boost/format.hpp>

namespace moja {
namespace modules {
namespace chapman_richards {

double ForestGrowthModule::calculate_above_ground_biomass(double age, double max, double k, double m) {
   return max * pow(1 - exp(-k * age), 1 / (1 - m));
}

void ForestGrowthModule::configure(const DynamicObject& config) {}

void ForestGrowthModule::subscribe(NotificationCenter& notificationCenter) {
   notificationCenter.subscribe(signals::LocalDomainInit, &ForestGrowthModule::onLocalDomainInit, *this);
   notificationCenter.subscribe(signals::TimingInit, &ForestGrowthModule::onTimingInit, *this);
   notificationCenter.subscribe(signals::TimingStep, &ForestGrowthModule::onTimingStep, *this);
}

ForestGrowthModule::partitioned_biomass ForestGrowthModule::calculate_forest_biomass(const ForestType* forest_type,
                                                                                     double age) {
   partitioned_biomass biomass;
   if (age > 0.0) {
      const auto above_ground_mass =
          calculate_above_ground_biomass(age, forest_type->max, forest_type->k, forest_type->m);

      biomass.above_ground = above_ground_mass;
      const auto root_mass = biomass.above_ground * forest_type->root_to_shoot;
      biomass.below_ground = root_mass;
   }
   return biomass;
}

void ForestGrowthModule::onLocalDomainInit() {
   // Pools
   atmosphere_ = _landUnitData->getPool("atmosphereCM");

   above_ground_cm_ = _landUnitData->getPool("aboveGroundCM");
   below_ground_cm_ = _landUnitData->getPool("belowGroundCM");

   initial_values_ = _landUnitData->getPool("initialValues");
}

void ForestGrowthModule::onTimingInit() {
   // Variables
   forest_exists_ = _landUnitData->getVariable("forest_exists");
   forest_age_ = _landUnitData->getVariable("forest_age");
   forest_type_ = _landUnitData->getVariable("forest_type");
}

void ForestGrowthModule::onTimingStep() {
   if (!forest_exists_->value()) return;

   const auto timing = _landUnitData->timing();
   const auto step_len_in_yrs = timing->stepLengthInYears();
   const auto forest_type =
       std::static_pointer_cast<const ForestType>(forest_type_->value().extract<const std::shared_ptr<flint::IFlintData>>()).get();
   const double forest_age_start = forest_age_->value();

   if (!(FloatCmp::greaterThanOrEqualTo(forest_age_start, 0.0))) {
      const auto str = ((boost::format("treeAge (%1%) not >= 0") % forest_age_start).str());
      BOOST_THROW_EXCEPTION(flint::SimulationError()
                            << flint::Details(str) << flint::LibraryName("moja.modules.chapman_richards")
                            << flint::ModuleName("ForestGrowthModule::updateTreeProperties") << flint::ErrorCode(2));
   }
   const auto tree_age_end = forest_age_start + step_len_in_yrs;
   const auto starting_biomass = calculate_forest_biomass(forest_type, forest_age_start);
   const auto ending_biomass = calculate_forest_biomass(forest_type, tree_age_end);

   const auto production_agb = (ending_biomass.above_ground - starting_biomass.above_ground) * forest_type->carbon_frac_ag;
   const auto production_bgb = (ending_biomass.below_ground - starting_biomass.below_ground) * forest_type->carbon_frac_bg;

   // Production
   DynamicVar production_data = std::make_shared<flint::OperationDataPackage>(flint::FluxType::NPP);
   auto production = _landUnitData->createStockOperation(production_data);
   production->addTransfer(atmosphere_, above_ground_cm_, production_agb)
       ->addTransfer(atmosphere_, below_ground_cm_, production_bgb);
   _landUnitData->submitOperation(production);

   forest_age_->set_value(tree_age_end);
}

}  // namespace chapman_richards
}  // namespace modules
}  // namespace moja