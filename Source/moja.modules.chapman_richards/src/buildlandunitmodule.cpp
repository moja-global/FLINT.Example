#include "moja/modules/chapman_richards/buildlandunitmodule.h"

#include "moja/modules/chapman_richards/forestevents.h"
#include "moja/modules/chapman_richards/foresttype.h"
#include "moja/modules/chapman_richards/landcovertransition.h"

#include <moja/flint/eventqueue.h>
#include <moja/flint/flintexceptions.h>
#include <moja/flint/itiming.h>
#include <moja/flint/ivariable.h>
#include <moja/flint/spatiallocationinfo.h>

#include <moja/notificationcenter.h>
#include <moja/signals.h>

#include <fmt/format.h>

namespace moja::modules::chapman_richards {

void BuildLandUnitModule::subscribe(NotificationCenter& notificationCenter) {
   notificationCenter.subscribe(signals::LocalDomainInit, &BuildLandUnitModule::onLocalDomainInit, *this);
   notificationCenter.subscribe(signals::PreTimingSequence, &BuildLandUnitModule::onPreTimingSequence, *this);
}

void BuildLandUnitModule::configure(const DynamicObject& config) {}

void BuildLandUnitModule::onLocalDomainInit() {
   try {
      sim_area_id_var_ = _landUnitData->getVariable("admin_level");
      forest_cover_var_ = _landUnitData->getVariable("forest_cover");
      spatial_location_info_var_ = _landUnitData->getVariable("spatialLocationInfo");
      land_unit_build_success_var_ = _landUnitData->getVariable("landUnitBuildSuccess");
      forest_types_var_ = _landUnitData->getVariable("forest_types");
      forest_type_var_ = _landUnitData->getVariable("forest_type");
      forest_exists_var_ = _landUnitData->getVariable("forest_exists");
      forest_age_var_ = _landUnitData->getVariable("forest_age");

   } catch (flint::VariableNotFoundException& exc) {
      const auto variable_name = *(boost::get_error_info<flint::VariableName>(exc));
      const auto str = fmt::format("Variable not found: {}", variable_name);
      BOOST_THROW_EXCEPTION(flint::LocalDomainError()
                            << flint::Details(str) << flint::LibraryName("moja.modules.chapman_richards")
                            << flint::ModuleName(BOOST_CURRENT_FUNCTION) << flint::ErrorCode(1));
   } catch (...) {
      if (_landUnitData->hasVariable("spatialLocationInfo")) {
         auto spatial_location_info = std::static_pointer_cast<flint::SpatialLocationInfo>(
             _landUnitData->getVariable("spatialLocationInfo")->value().extract<std::shared_ptr<flint::IFlintData>>());
         spatial_location_info->_library = "moja.modules.chapman_richards";
         spatial_location_info->_module = BOOST_CURRENT_FUNCTION;
         spatial_location_info->_errorCode = 1;
      }
      throw;
   }
}

void BuildLandUnitModule::onPreTimingSequence() {
   try {
      if (sim_area_id_var_->value().isEmpty()) {
         land_unit_build_success_var_->set_value(false);
         return;
      }

      // time series
      auto land_cover_transitions = forest_cover_var_->value().extract<const std::vector<LandcoverTransition>>();

      // static
      const auto timing = _landUnitData->timing();

      auto cover_iterator = land_cover_transitions.begin();

      auto event_queue = std::static_pointer_cast<flint::EventQueue>(
          _landUnitData->getVariable("eventqueue")->value<std::shared_ptr<flint::IFlintData>>());
      event_queue->clear();
      int eventId = 0;

      auto tier1_cover = LandcoverTransition::landcover_type::nodata;
      LandcoverTransition::landcover_type prev_tier1_cover;
      for (auto landcover_transition = land_cover_transitions.begin();
           landcover_transition != land_cover_transitions.end(); ++landcover_transition) {
         prev_tier1_cover = tier1_cover;
         tier1_cover = landcover_transition->landcover;
         if (landcover_transition == land_cover_transitions.begin()) {
            set_initial_land_cover(tier1_cover);
         } else {
            auto ev_date = landcover_transition->date;
            ev_date.addHours(12.0);
            if (tier1_cover != prev_tier1_cover) {
               if (landcover_transition->landcover == LandcoverTransition::landcover_type::non_forest) {
                  auto ev = std::make_shared<ForestClearEvent>(eventId, "Forest Clearing");
                  event_queue->emplace_back(ev_date, ev);
               } else if (landcover_transition->landcover == LandcoverTransition::landcover_type::forest) {
                  auto ev = std::make_shared<ForestPlantEvent>(eventId, "Forest Planting");
                  ev->forest_type_id = get_forest_type();
                  event_queue->emplace_back(ev_date, ev);
               }
               eventId++;
            }
         }
      }

      land_unit_build_success_var_->set_value(true);
   } catch (...) {
      if (_landUnitData->hasVariable("spatialLocationInfo")) {
         auto spatial_location_info = std::static_pointer_cast<flint::SpatialLocationInfo>(
             _landUnitData->getVariable("spatialLocationInfo")->value().extract<std::shared_ptr<flint::IFlintData>>());
         spatial_location_info->_library = "moja.modules.chapman_richards";
         spatial_location_info->_module = BOOST_CURRENT_FUNCTION;
         spatial_location_info->_errorCode = 1;
      }
      throw;
   }
}

int BuildLandUnitModule::get_forest_type() {
   const auto gez_var = _landUnitData->getVariable("gez");
   return int(gez_var->value());
}

void BuildLandUnitModule::set_initial_land_cover(LandcoverTransition::landcover_type landcover) {
   if (landcover == LandcoverTransition::landcover_type::forest) {
      const auto forest_type_id = get_forest_type();
      const auto forest_types_var = _landUnitData->getVariable("forest_types");
      const auto forest_types = std::static_pointer_cast<const ForestTypeList>(
          forest_types_var->value().extract<std::shared_ptr<flint::IFlintData>>());

      auto forest_type = forest_types->find(forest_type_id);

      forest_type_var_->set_value(std::static_pointer_cast<flint::IFlintData>(forest_type));
      forest_exists_var_->set_value(true);
      forest_age_var_->set_value(300.0);
   } else {
      forest_exists_var_->set_value(false);
      forest_age_var_->set_value(0.0);
   }
}

}  // namespace moja::modules::chapman_richards
