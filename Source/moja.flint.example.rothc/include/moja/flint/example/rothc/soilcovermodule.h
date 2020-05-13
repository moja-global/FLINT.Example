#ifndef MOJA_FLINT_EXAMPLE_ROTHC_SOILCOVERMODULE_H_
#define MOJA_FLINT_EXAMPLE_ROTHC_SOILCOVERMODULE_H_

#include "moja/flint/example/rothc/_modules.rothc_exports.h"
#include "moja/flint/modulebase.h"

namespace moja {
namespace flint {
namespace example {
namespace rothc {

class ROTHC_API SoilCoverModule : public flint::ModuleBase {
public:
	SoilCoverModule() : ModuleBase() {}
	virtual ~SoilCoverModule() = default;

	void configure(const DynamicObject& config) override;
	void subscribe(NotificationCenter& notificationCenter) override;

	void onTimingInit() override;
	void onTimingStep() override;

private:
	const flint::IVariable* _soilCover;
	flint::IVariable* _isSoilCovered;
	void setSoilCovered();
};

}
}
}
} // namespace moja::flint::example:rothc

#endif // MOJA_FLINT_EXAMPLE_ROTHC_SOILCOVERMODULE_H_
