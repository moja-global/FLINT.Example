#pragma once

#include "moja/flint/example/rothc/_modules.rothc_exports.h"

#include <moja/flint/modulebase.h>

namespace moja::flint::example::rothc {

class ROTHC_API SoilCoverModule : public ModuleBase {
  public:
   SoilCoverModule() : ModuleBase() {}
   virtual ~SoilCoverModule() = default;

   void configure(const DynamicObject& config) override;
   void subscribe(NotificationCenter& notificationCenter) override;

   void onTimingInit() override;
   void onTimingStep() override;

  private:
   void setSoilCovered();

   const IVariable* _soilCover;
   IVariable* _isSoilCovered;
};

}  // namespace moja::flint::example::rothc
