#include "moja/flint/example/base/errorscreenwriter.h"

#include <moja/logging.h>
#include <moja/flint/ivariable.h>
#include <moja/flint/spatiallocationinfo.h>

#include <moja/signals.h>
#include <moja/notificationcenter.h>

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

namespace moja {
namespace flint {
namespace example { namespace base {

// --------------------------------------------------------------------------------------------

void ErrorScreenWriter::configure(const DynamicObject& config) {
	_logErrorMax = 100;

	// this is a per block count on how many to output, default is 100
	if (config.contains("log_errors_max_number")) {
		_logErrorMax = config["log_errors_max_number"];
	}
	if (_logErrorMax < 0)
		_logErrorMax = 0;
}

// --------------------------------------------------------------------------------------------

void ErrorScreenWriter::subscribe(NotificationCenter& notificationCenter) {
	notificationCenter.subscribe(signals::LocalDomainInit,						&ErrorScreenWriter::onLocalDomainInit,					*this);
	notificationCenter.subscribe(signals::LocalDomainProcessingUnitInit,		&ErrorScreenWriter::onLocalDomainProcessingUnitInit, 	*this);
	notificationCenter.subscribe(signals::Error,								&ErrorScreenWriter::onError,							*this);
}

// --------------------------------------------------------------------------------------------

void ErrorScreenWriter::onLocalDomainInit() {
	_spatiallocationinfo = std::static_pointer_cast<flint::SpatialLocationInfo>(_landUnitData->getVariable("spatialLocationInfo")->value().extract<std::shared_ptr<flint::IFlintData>>());

	if (_landUnitData->hasVariable("system_runid")) {
		_systemRunId = _landUnitData->getVariable("system_runid");
		_systemRunIdStr = _systemRunId->value().extract<const std::string>();
	} else {
		_systemRunIdStr = "NA";
	}
	_logErrorCount = 0;
}

// --------------------------------------------------------------------------------------------

void ErrorScreenWriter::onLocalDomainProcessingUnitInit() {
	_logErrorCount = 0;
	_warningGiven = false;
}

// --------------------------------------------------------------------------------------------

void ErrorScreenWriter::onError(std::string msg) {
	if (_logErrorMax == 0 || _logErrorCount < _logErrorMax) {
		boost::replace_all(msg, "\n", " ");
        if (_spatiallocationinfo != nullptr) {
			auto errorMsg = (boost::format("RunId (%11%) - Library %5%, Module %6%: Error at location [tileIdx %1%, blockIdx %2%, cellIdx %3%], random seed [global %8%, block %9%, cell %10%], code %4% : Message - %7%")
										% _spatiallocationinfo->_tileIdx 
										% _spatiallocationinfo->_blockIdx
										% _spatiallocationinfo->_cellIdx
										% _spatiallocationinfo->_errorCode
										% _spatiallocationinfo->_library
										% _spatiallocationinfo->_module
										% msg
										% _spatiallocationinfo->_randomSeedGlobal
										% _spatiallocationinfo->_randomSeedBlock
										% _spatiallocationinfo->_randomSeedCell
										% _systemRunIdStr
							).str();
			MOJA_LOG_ERROR << errorMsg;
        } else {
			auto errorMsg = (boost::format("RunId (%11%) - Library %5%, Module %6%: Error, code %4% : Message - %7%")
										% _spatiallocationinfo->_errorCode
										% _spatiallocationinfo->_library
										% _spatiallocationinfo->_module
										% msg
										% _systemRunIdStr
							).str();
			MOJA_LOG_ERROR << errorMsg;
		}
		// _logErrorCount++;
	} else {
		if ( (!_warningGiven || ((_logErrorCount % 10000) == 0)) && (_logErrorCount >= _logErrorMax)) {
			MOJA_LOG_ERROR << "Max Screen error messages exceeded (max: " << _logErrorMax << ", count: " << _logErrorCount << ")";
			_warningGiven = true;
		}
	}
	_logErrorCount++;
}

}}}} // namespace moja::flint::example::base
