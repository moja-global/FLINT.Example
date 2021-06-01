#pragma once

#include "moja/modules/chapman_richards/simulationunitdata.h"
#include "moja/modules/chapman_richards/systemsettings.h"

#include "foresttype.h"
#include "landcovertransition.h"

#include <moja/flint/modulebase.h>

#include <boost/optional/optional_fwd.hpp>

namespace moja {

namespace flint {
class EventQueue;
}  // namespace flint

namespace modules::chapman_richards {

class BuildLandUnitModule : public flint::ModuleBase {
  public:
   enum class forest_cover_enum : int { non_forest = 0, forest, no_data };
   enum class forest_cover_change : int { none = 0, defor, refor };
   explicit BuildLandUnitModule(SystemSettings& systemSettings)
       : system_settings_(systemSettings),
         sim_area_id_var_(nullptr),
         forest_cover_var_(nullptr),
         forest_types_var_(nullptr),
         spatial_location_info_var_(nullptr),
         land_unit_build_success_var_(nullptr),
         forest_exists_var_(nullptr),
         forest_age_var_(nullptr) {}

   virtual ~BuildLandUnitModule() = default;

   void configure(const DynamicObject&) override;
   void subscribe(NotificationCenter& notificationCenter) override;

   // Notification handlers

   void onLocalDomainInit() override;
   void onPreTimingSequence() override;

  private:
   int get_forest_type();
   void set_initial_land_cover(LandcoverTransition::landcover_type landcover);

   // -- Shared Data
   SystemSettings& system_settings_;

   // Variable handles
   const flint::IVariable* sim_area_id_var_;
   const flint::IVariable* forest_cover_var_;
   const flint::IVariable* forest_types_var_;
   const flint::IVariable* spatial_location_info_var_;

   flint::IVariable* land_unit_build_success_var_;
   flint::IVariable* forest_exists_var_;
   flint::IVariable* forest_age_var_;
   flint::IVariable* forest_type_var_;
};

}  // namespace modules::chapman_richards
}  // namespace moja