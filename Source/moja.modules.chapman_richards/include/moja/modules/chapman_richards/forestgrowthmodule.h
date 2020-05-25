#pragma once
#include "moja/modules/chapman_richards/_modules.chapman_richards_exports.h"

#include <moja/flint/modulebase.h>

namespace moja {
namespace flint {
class IPool;
class IVariable;
}  // namespace flint
namespace modules {
namespace chapman_richards {

class ForestType;

class CHAPMAN_RICHARDS_API ForestGrowthModule : public moja::flint::ModuleBase {
  public:
   ForestGrowthModule() = default;

   virtual ~ForestGrowthModule() = default;

   void configure(const DynamicObject& config) override;
   void subscribe(NotificationCenter& notificationCenter) override;

   void onLocalDomainInit() override;
   void onTimingInit() override;
   void onTimingStep() override;

  private:
   struct partitioned_biomass {
      double above_ground = 0.0;
      double below_ground = 0.0;
   };

   static partitioned_biomass calculate_forest_biomass(const ForestType* forest_type, double age);
   static double calculate_above_ground_biomass(double age, double max, double k, double m);

   const flint::IPool* atmosphere_ = nullptr;
   const flint::IPool* initial_values_ = nullptr;
   const flint::IPool* above_ground_cm_ = nullptr;
   const flint::IPool* below_ground_cm_ = nullptr;

   flint::IVariable* forest_exists_ = nullptr;
   flint::IVariable* forest_age_ = nullptr;
   const flint::IVariable* forest_type_ = nullptr;
};

}  // namespace chapman_richards
}  // namespace modules
}  // namespace moja