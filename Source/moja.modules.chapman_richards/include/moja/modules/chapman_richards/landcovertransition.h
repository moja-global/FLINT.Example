#pragma once

#include "moja/modules/chapman_richards/_modules.chapman_richards_exports.h"

#include <moja/flint/iflintdata.h>

#include <moja/datetime.h>
#include <moja/dynamic.h>

namespace moja::modules::chapman_richards {

class CHAPMAN_RICHARDS_API LandcoverTransition : public flint::IFlintData {
  public:
   enum class landcover_type { nodata, forest, non_forest };
   LandcoverTransition(DateTime date, landcover_type landcover) : date(date), landcover(landcover) {}

   LandcoverTransition() = default;
   virtual ~LandcoverTransition() = default;
   // LandcoverTransition(const LandcoverTransition&) = delete;
   // LandcoverTransition& operator=(const LandcoverTransition&) = delete;

   void configure(DynamicObject config, const flint::ILandUnitController& landUnitController,
                  datarepository::DataRepository& dataRepository) override;

   DynamicObject exportObject() const override;

   DateTime date;
   landcover_type landcover;
};
}  // namespace moja::modules::chapman_richards
