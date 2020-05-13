#include "moja/modules/fullcam/soilcovermodule.h"

#include "moja/flint/variable.h"

#include "moja/notificationcenter.h"
#include "moja/timeseries.h"
#include "moja/signals.h"

namespace moja {
namespace modules {
namespace fullcam {

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
	const auto& soilCover = _soilCover->value().extract<TimeSeries>();
	_isSoilCovered->set_value(soilCover.value() >= 0.5);
}

}
}
} // namespace moja::modules::fullcam

