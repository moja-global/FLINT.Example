#pragma once

#include "moja/flint/example/base/_modules.base_exports.h"

#include <moja/flint/modulebase.h>

namespace moja::flint {

class SpatialLocationInfo;

namespace example::base {

class BASE_API ErrorScreenWriter : public ModuleBase {
  public:
   ErrorScreenWriter() : ModuleBase(), _logErrorMax(100), _logErrorCount(0), _warningGiven(false) {}

   virtual ~ErrorScreenWriter() = default;
   void configure(const DynamicObject& config) override;
   void subscribe(NotificationCenter& notificationCenter) override;

   ModuleTypes moduleType() override { return ModuleTypes::Aggregator; };

   void onLocalDomainInit() override;
   void onLocalDomainProcessingUnitInit() override;
   void onError(std::string msg) override;

  private:
   // -- Shared Data
   int _logErrorMax;
   int _logErrorCount;
   bool _warningGiven;

   // -- flintdata objects data
   std::shared_ptr<const SpatialLocationInfo> _spatiallocationinfo;

   // -- varibales objects data
   IVariable* _systemRunId;
   std::string _systemRunIdStr;
};

}  // namespace example::base
}  // namespace moja::flint
