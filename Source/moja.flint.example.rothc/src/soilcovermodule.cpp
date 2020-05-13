#include "moja/flint/example/rothc/soilcovermodule.h"

#include "moja/flint/variable.h"

#include "moja/notificationcenter.h"
#include "moja/signals.h"

namespace moja {
namespace flint {
namespace example {
namespace rothc {

void SoilCoverModule::configure(const DynamicObject& config) { }

void SoilCoverModule::subscribe(NotificationCenter& notificationCenter) {
	notificationCenter.subscribe(signals::TimingInit, &SoilCoverModule::onTimingInit, *this);
	notificationCenter.subscribe(signals::TimingStep, &SoilCoverModule::onTimingStep, *this);
}

void SoilCoverModule::onTimingInit() {
	_soilCover = _landUnitData->getVariable("soilCover");
	_isSoilCovered = _landUnitData->getVariable("isSoilCovered");
	setSoilCovered();
}

void SoilCoverModule::onTimingStep() {
	setSoilCovered();
}

void SoilCoverModule::setSoilCovered() {
   // This comes from a TimeSeries transform
   _isSoilCovered->set_value(_soilCover->value() >= 0.5);
}

}
}
}
} // namespace moja::flint::example::rothc


