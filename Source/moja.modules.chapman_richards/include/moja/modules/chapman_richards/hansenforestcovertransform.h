#pragma once

#include "moja/modules/chapman_richards/_modules.chapman_richards_exports.h"

#include <moja/flint/itransform.h>
#include <moja/flint/ivariable.h>
#include <moja/flint/spatiallocationinfo.h>

#include <moja/datarepository/iproviderspatialrasterinterface.h>
#include <moja/datetime.h>

namespace moja {
namespace modules {
namespace chapman_richards {

/*
Tree canopy cover for year 2000 (treecover2000)
Tree cover in the year 2000, defined as canopy closure for all vegetation taller than 5m in height. Encoded as a
percentage per output grid cell, in the range 0–100.

Global forest cover gain 2000–2012 (gain)
Forest gain during the period 2000–2012, defined as the inverse of loss, or a non-forest to forest change entirely
within the study period. Encoded as either 1 (gain) or 0 (no gain).

Year of gross forest cover loss event (lossyear)
Forest loss during the period 2000–2018, defined as a stand-replacement disturbance, or a change from a forest to
non-forest state. Encoded as either 0 (no loss) or else a value in the range 1–17, representing loss detected primarily
in the year 2001–2018, respectively.
*/

class CHAPMAN_RICHARDS_API HansenForestCoverTransform : public flint::ITransform {
  public:
   HansenForestCoverTransform() : land_unit_controller_(nullptr) {}

   void configure(DynamicObject config, const flint::ILandUnitController& landUnitController,
                  datarepository::DataRepository& dataRepository) override;

   void controllerChanged(const flint::ILandUnitController& controller) override;

   const DynamicVar& value() const override;

  private:
   DateTime calculate_transition_date(DateTime start_date, DateTime end_date) const;
   const flint::ILandUnitController* land_unit_controller_;
   
   mutable DynamicVar cached_value_;

   // Variable handles
   const flint::IVariable* tree_gain_var_;
   const flint::IVariable* tree_loss_year_var_;
   const flint::IVariable* tree_cover2000_;
   const flint::IVariable* spatial_location_info_var_;
};

}  // namespace chapman_richards
}  // namespace modules
}  // namespace moja