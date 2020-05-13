#ifndef MOJA_FLINT_EXAMPLE_ROTHC_PLANTRESIDUEMODULE_H_
#define MOJA_FLINT_EXAMPLE_ROTHC_PLANTRESIDUEMODULE_H_

#include "moja/flint/example/rothc/_modules.rothc_exports.h"
#include "moja/flint/modulebase.h"

namespace moja {
namespace flint {
namespace example {
namespace rothc {

class ROTHC_API PlantResidueModule : public flint::ModuleBase {
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

	const flint::IPool* _soilDPM; // Decomposable Plant Material (DPM)
	const flint::IPool* _soilRPM; // Resistant Plant Material (RPM)
	const flint::IPool* _plantCM; // Resistant Plant Material (RPM)

	const flint::IVariable* _presCM;
};

}
}
}
} // namespace moja::flint::example:rothc
#endif // MOJA_FLINT_EXAMPLE_ROTHC_PLANTRESIDUEMODULE_H_