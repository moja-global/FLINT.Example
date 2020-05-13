#ifndef MOJA_FLINT_EXAMPLE_ROTHC_ROTHCMODULE_H_
#define MOJA_FLINT_EXAMPLE_ROTHC_ROTHCMODULE_H_

#include "moja/_core_exports.h"
#include "moja/flint/imodule.h"
#include "moja/flint/modulebase.h"
#include <moja/flint/itiming.h>
#include "moja/flint/example/rothc/_modules.rothc_exports.h"

namespace moja {
namespace flint {
namespace example {
namespace rothc {

class ROTHC_API RothCModule : public flint::ModuleBase
{
	double evapoOpenRatio;

	double fracHumsToBios;
	double fracHumsToHums;

	double fracPbioToBiof;
	double fracPbioToHums;
	double fracSdcmToAtms;

	double maxTSMDBare;
	double maxTSMDCvrd;

	double rateModifyingFactor;

	/// <summary>
	/// Decomposition rate constants
	/// The decomposition rate constants (k), in years-1, for each compartment
	/// </summary>
	double sdcmRateMultDpm;
	double sdcmRateMultRpm;
	double sdcmRateMultBiof;
	double sdcmRateMultBios;
	double sdcmRateMultHums;

public:
	RothCModule() = default;
	~RothCModule() = default;

	void configure(const DynamicObject& config) override;
	void subscribe(NotificationCenter& notificationCenter) override;

	void onLocalDomainInit() override;
	void onTimingInit() override;
	void onTimingStep() override;

private:
	void ComputeTSMDParams(double clayFrac, double sampleDepth, double bareToCvrdRatio);
	void InitializeForASimulation();
	void UpdateRateProperties();
	void SubmitMoves();

	/// <summary>
	///  Soil organic carbon active compartments
	/// </summary>
	const flint::IPool* _soilDPM; // Decomposable Plant Material (DPM)
	const flint::IPool* _soilRPM; // Resistant Plant Material (RPM)
	const flint::IPool* _soilBioF; // Microbial Biomass (BIO) Fast
	const flint::IPool* _soilBioS; // Microbial Biomass (BIO) Slow
	const flint::IPool* _soilHUM; // Humified Organic Matter (HUM).
	const flint::IPool* _soilIOM;
	const flint::IPool* _atmosphere;
	const flint::IPool* _initialValues;

	const flint::IVariable* _rainfall;
	const flint::IVariable* _avgAirTemp;
	const flint::IVariable* _openPanEvap;
	flint::IVariable* _TSMD;
	const flint::IVariable* _isSoilCovered;
	const flint::IVariable* _clayFrac;
	const flint::IVariable* _initSoil;
	const flint::IVariable* _soil;
};

}
}
}
} // namespace moja::flint::example::rothc

#endif // MOJA_FLINT_EXAMPLE_ROTHC_ROTHCMODULE_H_
