#pragma once
#include "moja/modules/chapman_richards/disturbanceevents.h"

#include <moja/flint/iflintdata.h>

#include <moja/dynamic.h>

namespace moja::modules::chapman_richards {

std::shared_ptr<flint::IFlintData> createForestEventsFactory(const std::string& eventTypeStr, int id,
                                                             const std::string& name, const DynamicObject& other);

class ForestPlantEvent final : public DisturbanceEventBase {
  public:
   ForestPlantEvent(int id, const std::string& name)
       : DisturbanceEventBase(id, "chapman_richards.ForestPlantEvent", name) {}
   virtual ~ForestPlantEvent() = default;

   void configure(DynamicObject config, const flint::ILandUnitController& landUnitController,
                  datarepository::DataRepository& dataRepository) override;
   DynamicObject exportObject() const override;
   void simulate(DisturbanceEventHandler& event_handler) const override;

   bool is_clearing() const override { return false; }
   int forest_type_id;
};

class ForestClearEvent final : public DisturbanceEventBase {
  public:
   ForestClearEvent(int id, const std::string& name)
       : DisturbanceEventBase(id, "chapman_richards.ForestClearEvent", name) {}
   virtual ~ForestClearEvent() = default;

   void configure(DynamicObject config, const flint::ILandUnitController& landUnitController,
                  datarepository::DataRepository& dataRepository) override;
   DynamicObject exportObject() const override;
   void simulate(DisturbanceEventHandler& event_handler) const override;

   bool is_clearing() const override;
};

}  // namespace moja::modules::chapman_richards
