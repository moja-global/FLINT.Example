#ifndef MOJA_FLINT_EXAMPLE_BASE_ERRORSCREENWRITER_H_
#define MOJA_FLINT_EXAMPLE_BASE_ERRORSCREENWRITER_H_

#include "moja/flint/example/base/_modules.base_exports.h"
#include <moja/flint/modulebase.h>

namespace moja {
	namespace flint {
		class SpatialLocationInfo;
	}
}

namespace moja {
namespace flint {
namespace example { namespace base {

class BASE_API ErrorScreenWriter : public flint::ModuleBase {
public:
	ErrorScreenWriter() : ModuleBase(), _logErrorMax(100), _logErrorCount(0), _warningGiven(false) {}

	virtual ~ErrorScreenWriter() = default;
	void configure(const DynamicObject& config) override;
	void subscribe(NotificationCenter& notificationCenter) override;

	flint::ModuleTypes moduleType() override { return flint::ModuleTypes::Aggregator; };

	void onLocalDomainInit						() override;
	void onLocalDomainProcessingUnitInit		() override;
	void onError								(std::string msg) override;

private:
	// -- Shared Data
	int _logErrorMax;
	int _logErrorCount;
	bool _warningGiven;

	// -- flintdata objects data
	std::shared_ptr<const flint::SpatialLocationInfo> _spatiallocationinfo;

	// -- varibales objects data
	flint::IVariable* _systemRunId;
	std::string _systemRunIdStr;
};

}}}} // namespace moja::flint::example::base

#endif // MOJA_FLINT_EXAMPLE_BASE_ERRORSCREENWRITER_H_
