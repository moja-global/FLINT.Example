#include "moja/modules/chapman_richards/forestevents.h"

#include "moja/modules/chapman_richards/disturbanceevents.h"
#include "moja/modules/chapman_richards/foresttype.h"

#include <moja/flint/ilandunitcontroller.h>
#include <moja/flint/ivariable.h>

namespace moja {
namespace modules {
namespace chapman_richards {

// Flint Data Factory
std::shared_ptr<flint::IFlintData> createForestEventsFactory(const std::string& eventTypeStr, int id,
                                                             const std::string& name, const DynamicObject& other) {
   if (eventTypeStr == "chapman_richards.ForestPlantEvent") return std::make_shared<ForestPlantEvent>(id, name);
   if (eventTypeStr == "chapman_richards.ForestClearEvent") return std::make_shared<ForestClearEvent>(id, name);
   return nullptr;
}

void ForestPlantEvent::configure(DynamicObject config, const flint::ILandUnitController& landUnitController,
                                 datarepository::DataRepository& dataRepository) {
   DisturbanceEventBase::configure(config, landUnitController, dataRepository);
   forest_type_id = config["forest_type_id"];
}

DynamicObject ForestPlantEvent::exportObject() const {
   auto object = DisturbanceEventBase::exportObject();
   object["forest_type_id"] = forest_type_id;
   return object;
}

void ForestPlantEvent::simulate(DisturbanceEventHandler& event_handler) const { event_handler.simulate(*this); }

void ForestClearEvent::configure(DynamicObject config, const flint::ILandUnitController& landUnitController,
                                 datarepository::DataRepository& dataRepository) {
   DisturbanceEventBase::configure(config, landUnitController, dataRepository);
}

DynamicObject ForestClearEvent::exportObject() const {
   auto object = DisturbanceEventBase::exportObject();
   return object;
}

void ForestClearEvent::simulate(DisturbanceEventHandler& event_handler) const { event_handler.simulate(*this); }

bool ForestClearEvent::is_clearing() const { return true; }

}  // namespace chapman_richards
}  // namespace modules
}  // namespace moja
