#include "moja/modules/chapman_richards/forestgrowthmodule.h"

#include "moja/modules/chapman_richards/foresttype.h"

#include <moja/flint/flintexceptions.h>
#include <moja/flint/iflintdata.h>
#include <moja/flint/ioperation.h>
#include <moja/flint/ipool.h>
#include <moja/flint/ivariable.h>
#include <moja/flint/operationdatapackage.h>
#include <moja/flint/timing.h>

#include <moja/notificationcenter.h>
#include <moja/signals.h>

#include <fmt/format.h>

namespace moja::modules::chapman_richards {

inline double decay_frac(double f, double yrs) { return 1.0 - std::pow(1 - f, yrs); }

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
   initial_values_ = _landUnitData->getPool("initialValues");
}

void ForestGrowthModule::onTimingInit() {
   // Variables
   forest_exists_ = _landUnitData->getVariable("forest_exists");
   forest_age_ = _landUnitData->getVariable("forest_age");
   forest_type_ = _landUnitData->getVariable("forest_type");
}

flint::IPool* ForestGrowthModule::get_cohort_pool(const std::string& parent, const std::string& forest_type) {
   return _landUnitData->getPool(fmt::format("{} {}", forest_type, parent));
}

void ForestGrowthModule::onTimingStep() {
   if (!forest_exists_->value()) return;

   const auto timing = _landUnitData->timing();
   const auto step_len_in_yrs = timing->stepLengthInYears();
   auto forest_type = std::static_pointer_cast<const ForestType>(
       forest_type_->value().extract<const std::shared_ptr<flint::IFlintData>>());
   const double forest_age_start = forest_age_->value();

   if (!(FloatCmp::greaterThanOrEqualTo(forest_age_start, 0.0))) {
      const auto str = fmt::format("treeAge ({}) not >= 0", forest_age_start);
      BOOST_THROW_EXCEPTION(flint::SimulationError()
                            << flint::Details(str) << flint::LibraryName("moja.modules.chapman_richards")
                            << flint::ModuleName("ForestGrowthModule::updateTreeProperties") << flint::ErrorCode(2));
   }
   auto* above_ground_cm = get_cohort_pool("AG", forest_type->name);
   auto* below_ground_cm = get_cohort_pool("BG", forest_type->name);
   auto* dead_organic_cm = get_cohort_pool("DOM", forest_type->name);

   const auto tree_age_end = forest_age_start + step_len_in_yrs;
   const auto starting_biomass = calculate_forest_biomass(forest_type.get(), forest_age_start);
   const auto ending_biomass = calculate_forest_biomass(forest_type.get(), tree_age_end);

   const auto production_agb =
       (ending_biomass.above_ground - starting_biomass.above_ground) * forest_type->carbon_frac_ag;
   const auto production_bgb =
       (ending_biomass.below_ground - starting_biomass.below_ground) * forest_type->carbon_frac_bg;

   // Turnover
   const auto turnover_abg = above_ground_cm->value() * decay_frac(forest_type->turnover_frac_ag, step_len_in_yrs);
   const auto turnover_bbg = below_ground_cm->value() * decay_frac(forest_type->turnover_frac_bg, step_len_in_yrs);

   // Production
   DynamicVar production_data = std::make_shared<flint::OperationDataPackage>(flint::FluxType::NPP);
   auto production = _landUnitData->createStockOperation(production_data);
   production->addTransfer(atmosphere_, above_ground_cm, production_agb + turnover_abg)
       ->addTransfer(atmosphere_, below_ground_cm, production_bgb + turnover_bbg);
   _landUnitData->submitOperation(production);

   DynamicVar turnover_data = std::make_shared<flint::OperationDataPackage>(flint::FluxType::Turnover);
   auto turnover = _landUnitData->createStockOperation(turnover_data);
   turnover->addTransfer(above_ground_cm, dead_organic_cm, turnover_abg)
       ->addTransfer(above_ground_cm, dead_organic_cm, turnover_bbg);
   _landUnitData->submitOperation(turnover);

   forest_age_->set_value(tree_age_end);
}

}  // namespace moja::modules::chapman_richards
