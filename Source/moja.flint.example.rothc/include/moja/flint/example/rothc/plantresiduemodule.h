#pragma once

#include "moja/flint/example/rothc/_modules.rothc_exports.h"

#include <moja/flint/modulebase.h>

namespace moja::flint::example::rothc {

class ROTHC_API PlantResidueModule : public ModuleBase {
  public:
   PlantResidueModule() : ModuleBase() {}
   virtual ~PlantResidueModule() = default;

   void configure(const DynamicObject& config) override;
   void subscribe(NotificationCenter& notificationCenter) override;

   void onTimingInit() override;
   void onTimingStep() override;

  private:
   double _dFracPres;
   double _rFracPres;

   const IPool* _soilDPM;  // Decomposable Plant Material (DPM)
   const IPool* _soilRPM;  // Resistant Plant Material (RPM)
   const IPool* _plantCM;  // Resistant Plant Material (RPM)

   const IVariable* _presCM;
};

}  // namespace moja::flint::example::rothc
