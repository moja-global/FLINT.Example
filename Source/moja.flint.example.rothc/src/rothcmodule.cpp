#include "moja/flint/example/rothc/rothcmodule.h"

#include "moja/flint/flintexceptions.h"
#include "moja/flint/ivariable.h"
#include "moja/flint/ioperation.h"

#include "moja/mathex.h"
#include "moja/notificationcenter.h"
#include "moja/signals.h"

#include <boost/format.hpp>

namespace moja {
namespace flint {
namespace example {
namespace rothc {

void RothCModule::configure(const DynamicObject& config) { }

void RothCModule::subscribe(NotificationCenter& notificationCenter) {
	notificationCenter.subscribe(signals::LocalDomainInit	, &RothCModule::onLocalDomainInit	, *this);
	notificationCenter.subscribe(signals::TimingInit		, &RothCModule::onTimingInit		, *this);
	notificationCenter.subscribe(signals::TimingStep		, &RothCModule::onTimingStep		, *this);
}

/// <summary>
/// Soil moisture deficit (SMD)
/// </summary>
/// <param name="clayFrac"></param>
/// <param name="sampleDepth"></param>
/// <param name="bareToCvrdRatio"></param>
void RothCModule::ComputeTSMDParams(double clayFrac, double sampleDepth, double bareToCvrdRatio) {
	double clayFracSq = clayFrac*clayFrac;

	maxTSMDCvrd = (20.0 + 130.0*clayFrac - 100.0*clayFracSq)*(sampleDepth / 23.0);
	maxTSMDBare = bareToCvrdRatio*maxTSMDCvrd;

	if (!(maxTSMDCvrd > MathEx::k0Plus && maxTSMDBare > MathEx::k0Plus)) {  // ie assert(maxTSMD > k0Plus)
		auto str = ((boost::format("maxTSMDCvrd (%1%) && maxTSMDBare (%2%) > k0Plus") % maxTSMDCvrd % maxTSMDBare).str());
		BOOST_THROW_EXCEPTION(flint::SimulationError()
			<< flint::Details(str)
			<< flint::LibraryName("moja.flint.example.rothc")
			<< flint::ModuleName("RothCModule::ComputeTSMDParams")
			<< flint::ErrorCode(2));
	}
}

void RothCModule::InitializeForASimulation() {

	double clayFrac = _clayFrac->value();
	const auto initSoil = _initSoil->value();
	const auto soil = _soil->value();

	auto operation = _landUnitData->createStockOperation();
	operation
		->addTransfer(_initialValues, _soilDPM, initSoil["dpmaCMInit"])
		->addTransfer(_initialValues, _soilRPM, initSoil["rpmaCMInit"])
		->addTransfer(_initialValues, _soilBioF, initSoil["biofCMInit"])
		->addTransfer(_initialValues, _soilBioS, initSoil["biosCMInit"])
		->addTransfer(_initialValues, _soilHUM, initSoil["humsCMInit"])
		->addTransfer(_initialValues, _soilIOM, initSoil["inrtCMInit"]);
	_landUnitData->submitOperation(operation);

	// SMD parameters
	_TSMD->set_value(initSoil["TSMDInit"]);
	ComputeTSMDParams(clayFrac, soil["sampleDepth"], soil["bToCMaxTSMDRatio"]);

	// Evaporation
	evapoOpenRatio = soil["evapoOpenRatio"];

	// Soil decomposition
	double atmsToSoldRatio = 1.67 * (1.85 + 1.6 * exp(-7.86 * clayFrac));
	double fracSdcmToSolid = 1.0 / (1.0 + atmsToSoldRatio);
	fracSdcmToAtms = 1.0 - fracSdcmToSolid;

	fracPbioToBiof = fracSdcmToSolid * soil["fracPbioToBiof"];
	fracPbioToHums = fracSdcmToSolid * (1.0 - soil["fracPbioToBiof"]);
	fracHumsToBios = fracSdcmToSolid * soil["fracHumsToBios"];
	fracHumsToHums = fracSdcmToSolid * (1.0 - soil["fracHumsToBios"]);

	sdcmRateMultDpm = soil["sdcmRateMultDpm"];
	sdcmRateMultRpm = soil["sdcmRateMultRpm"];
	sdcmRateMultHums = soil["sdcmRateMultHums"];
	sdcmRateMultBiof = soil["sdcmRateMultBiof"];
	sdcmRateMultBios = soil["sdcmRateMultBios"];
}

void RothCModule::UpdateRateProperties() {
	// Water
   // This comes from a TimeSeries transform
   double rainPS = _rainfall->value();

	//Temperature
    // This comes from a TimeSeries transform
    double tm = _avgAirTemp->value();
    double a = (tm < -5.0) ? 0.0 : 47.91 / (1.0 + exp(106.06 / (18.27 + tm)));
	// Moisture
	double maxTSMD = _isSoilCovered->value() ? maxTSMDCvrd : maxTSMDBare;
    // This comes from a TimeSeries transform
    double evapPS = _openPanEvap->value() * evapoOpenRatio;

	double netWaterInput = (rainPS - evapPS);
    double TSMD = _TSMD->value();

    TSMD = std::max(0.0, netWaterInput >= 0
                             ? TSMD - netWaterInput
                             : _isSoilCovered->value()
                                   ? std::min(maxTSMDCvrd, TSMD - netWaterInput)
                                   : TSMD > maxTSMDBare ? TSMD : std::min(maxTSMDBare, TSMD - netWaterInput));
	_TSMD->set_value(TSMD);

	double b;
	if (tm < -5.0)
		b = 0.0;
	else {
		b = 0.2 + 0.8 * (maxTSMDCvrd - TSMD) / maxTSMDBare;

		if (b > 1.0) b = 1.0;
		if (b < 0.0) b = 0.0;
	}

	//Plant retainment factor
	double c = _isSoilCovered->value() ? 0.6 : 1.0;
	rateModifyingFactor = a * b * c;
}

void RothCModule::SubmitMoves() {
	const auto timing = _landUnitData->timing();
	// Soil decomposition
	double x = -rateModifyingFactor*timing->stepLengthInYears();
	double dpmSdcmFrac = 1.0 - std::exp(x*sdcmRateMultDpm);
	double rpmSdcmFrac = 1.0 - std::exp(x*sdcmRateMultRpm);
	double humsSdcmFrac = 1.0 - std::exp(x*sdcmRateMultHums);
	double biofSdcmFrac = 1.0 - std::exp(x*sdcmRateMultBiof);
	double biosSdcmFrac = 1.0 - std::exp(x*sdcmRateMultBios);

	auto operation = _landUnitData->createProportionalOperation();
	operation
		->addTransfer(_soilDPM, _atmosphere, dpmSdcmFrac*fracSdcmToAtms)
		->addTransfer(_soilRPM, _atmosphere, rpmSdcmFrac*fracSdcmToAtms)
		->addTransfer(_soilBioF, _atmosphere, biofSdcmFrac*fracSdcmToAtms)
		->addTransfer(_soilBioS, _atmosphere, biosSdcmFrac*fracSdcmToAtms)
		->addTransfer(_soilHUM, _atmosphere, humsSdcmFrac*fracSdcmToAtms)

		->addTransfer(_soilDPM, _soilBioF, dpmSdcmFrac*fracPbioToBiof)
		->addTransfer(_soilRPM, _soilBioF, rpmSdcmFrac*fracPbioToBiof)
		->addTransfer(_soilBioS, _soilBioF, biosSdcmFrac*fracPbioToBiof)
		->addTransfer(_soilHUM, _soilBioS, humsSdcmFrac*fracHumsToBios)

		->addTransfer(_soilDPM, _soilHUM, dpmSdcmFrac*fracPbioToHums)
		->addTransfer(_soilRPM, _soilHUM, rpmSdcmFrac*fracPbioToHums)
		->addTransfer(_soilBioF, _soilHUM, biofSdcmFrac*fracPbioToHums)
		->addTransfer(_soilBioS, _soilHUM, biosSdcmFrac*fracPbioToHums)
		->addTransfer(_soilHUM, _soilHUM, humsSdcmFrac*fracHumsToHums);

	_landUnitData->submitOperation(operation);
}

void RothCModule::onLocalDomainInit() {
	// Pools
	_soilDPM = _landUnitData->getPool("SoilDPM");
	_soilRPM = _landUnitData->getPool("SoilRPM");
	_soilBioF = _landUnitData->getPool("SoilBioF");
	_soilBioS = _landUnitData->getPool("SoilBioS");
	_soilHUM = _landUnitData->getPool("SoilHUM");
	_soilIOM = _landUnitData->getPool("SoilIOM");
	_atmosphere = _landUnitData->getPool("atmosphere");
	_initialValues = _landUnitData->getPool("initialValues");

	// Variables
	_rainfall = _landUnitData->getVariable("rainfall");
	_avgAirTemp = _landUnitData->getVariable("avgAirTemp");
	_openPanEvap = _landUnitData->getVariable("openPanEvap");
	_TSMD = _landUnitData->getVariable("TSMD");
	_isSoilCovered = _landUnitData->getVariable("isSoilCovered");
	_clayFrac = _landUnitData->getVariable("clayFrac");
	_initSoil = _landUnitData->getVariable("initSoil");
	_soil = _landUnitData->getVariable("soil");
}

void RothCModule::onTimingInit() {
	InitializeForASimulation();
}

void RothCModule::onTimingStep() {
	UpdateRateProperties();
	SubmitMoves();
}

}
}
}
} // namespace moja::flint::example::rothc




