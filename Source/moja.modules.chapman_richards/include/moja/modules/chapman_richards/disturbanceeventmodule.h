#pragma once

#include "moja/modules/chapman_richards/_modules.chapman_richards_exports.h"
#include "moja/modules/chapman_richards/disturbanceevents.h"

#include <moja/flint/modulebase.h>

namespace moja {
namespace flint {
class IPool;
class IVariable;
}  // namespace flint

namespace modules::chapman_richards {
class ForestFireEvent;

class CHAPMAN_RICHARDS_API DisturbanceEventModule : public flint::ModuleBase, DisturbanceEventHandler {
  public:
   DisturbanceEventModule() = default;
   virtual ~DisturbanceEventModule() = default;

   void configure(const DynamicObject& config) override;
   void subscribe(NotificationCenter& notificationCenter) override;

   void onTimingInit() override;

   void disturbanceEventHandler(const flint::EventQueueItem* event);  // special handler for events

  private:
   void simulate(const ForestPlantEvent& plant) override;
   flint::IPool* get_cohort_pool(const std::string& parent, const std::string& forest_type);
   void simulate(const ForestClearEvent& thin) override;

   const flint::IPool* atmosphere_;

   flint::IPool* above_ground_cm_ = nullptr;
   flint::IPool* below_ground_cm_ = nullptr;
   flint::IPool* dead_organic_cm_ = nullptr;

   flint::IVariable* forest_exists_;
   flint::IVariable* forest_age_;
   flint::IVariable* forest_type_;
};

}  // namespace modules::chapman_richards
}  // namespace moja