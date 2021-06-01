#include "moja/modules/chapman_richards/hansenforestcovertransform.h"

#include "moja/modules/chapman_richards/landcovertransition.h"

#include <moja/flint/ilandunitcontroller.h>
#include <moja/flint/itiming.h>
#include <moja/flint/spatiallocationinfo.h>

#include <moja/datarepository/datarepository.h>

namespace moja::modules::chapman_richards {

void HansenForestCoverTransform::configure(DynamicObject config, const flint::ILandUnitController& landUnitController,
                                           datarepository::DataRepository& dataRepository) {
   land_unit_controller_ = &landUnitController;

   tree_gain_var_ = land_unit_controller_->getVariable("tree_gain");
   tree_loss_year_var_ = land_unit_controller_->getVariable("tree_loss_year");
   tree_cover2000_ = land_unit_controller_->getVariable("tree_cover_2000");

   spatial_location_info_var_ = land_unit_controller_->getVariable("spatialLocationInfo");
}

void HansenForestCoverTransform::controllerChanged(const flint::ILandUnitController& controller) {
   cached_value_ = nullptr;
   land_unit_controller_ = &controller;
}

DateTime HansenForestCoverTransform::calculate_transition_date(DateTime start_date, DateTime end_date) const {
   auto spatial_location_info = std::static_pointer_cast<flint::SpatialLocationInfo>(
       spatial_location_info_var_->value().extract<std::shared_ptr<flint::IFlintData>>());
   const auto day_span = (end_date - start_date).days();
   std::uniform_int_distribution<> distribution(0, day_span - 1);  // define the range
   const auto random_days = distribution(spatial_location_info->_engCell);
   return start_date.addDays(random_days);
}

const DynamicVar& HansenForestCoverTransform::value() const {
   const auto& timing = land_unit_controller_->timing();
   const int days_in_sim = (timing.endDate() - timing.startDate()).days();

   auto spatial_location_info = std::static_pointer_cast<flint::SpatialLocationInfo>(
       spatial_location_info_var_->value().extract<std::shared_ptr<flint::IFlintData>>());

   auto& tree_gain = tree_gain_var_->value();
   auto& tree_loss_year = tree_loss_year_var_->value();

   std::vector<LandcoverTransition> land_cover_transitions;

   if (tree_loss_year.isEmpty() || tree_loss_year == 0) {
      auto& tree_cover_2000 = tree_cover2000_->value();
      if (!tree_cover_2000.isEmpty() && tree_cover_2000 > 30) {
         land_cover_transitions.emplace_back(DateTime(2000, 1, 1), LandcoverTransition::landcover_type::forest);
      } else {
         land_cover_transitions.emplace_back(DateTime(2000, 1, 1), LandcoverTransition::landcover_type::non_forest);
         if (!tree_gain.isEmpty() && tree_gain == true) {  // Forest gain during the period 2000-2012
            auto gain_date = calculate_transition_date(DateTime(2000, 1, 1), DateTime(2012, 12, 31));
            land_cover_transitions.emplace_back(gain_date, LandcoverTransition::landcover_type::forest);
         }
      }
   } else {
      land_cover_transitions.emplace_back(DateTime(2000, 1, 1), LandcoverTransition::landcover_type::forest);
      const auto loss_year = 2000 + tree_loss_year;
      auto loss_date = calculate_transition_date(DateTime(loss_year, 1, 1), DateTime(loss_year, 12, 31));
      land_cover_transitions.emplace_back(loss_date, LandcoverTransition::landcover_type::non_forest);
      if (!tree_gain.isEmpty() && tree_gain == true) {  // Forest gain during the period 2000-2012,
         auto plant_date = loss_date.addYears(1);       // Plant at least 1 year after clearing
         if (plant_date < DateTime(2012, 12, 31)) {
            auto gain_date = calculate_transition_date(plant_date, DateTime(2012, 12, 31));
            land_cover_transitions.emplace_back(gain_date, LandcoverTransition::landcover_type::forest);
         }
      }
   }

   cached_value_ = land_cover_transitions;

   return cached_value_;
}

}  // namespace moja::modules::chapman_richards
