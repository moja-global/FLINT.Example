#include "moja/modules/chapman_richards/aggregatorerror.h"

#include "moja/modules/chapman_richards/simulationunitdata.h"

#include <moja/flint/flintexceptions.h>
#include <moja/flint/ivariable.h>
#include <moja/flint/spatiallocationinfo.h>

#include <moja/notificationcenter.h>
#include <moja/signals.h>

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

namespace moja {
namespace modules {
namespace chapman_richards {

void AggregatorError::configure(const DynamicObject& config) {
   system_settings_.log_errors = true;
   if (config.contains("log_errors")) {
      system_settings_.log_errors = config["log_errors"];
   }
   system_settings_.log_error_max = 0;
   if (config.contains("log_errors_max_number")) {
      system_settings_.log_error_max = config["log_errors_max_number"];
   }
   system_settings_.log_error_per_block = false;
   if (config.contains("log_errors_per_block")) {
      system_settings_.log_error_per_block = config["log_errors_per_block"];
   }
}

void AggregatorError::subscribe(NotificationCenter& notificationCenter) {
   notificationCenter.subscribe(signals::LocalDomainInit, &AggregatorError::onLocalDomainInit, *this);
   notificationCenter.subscribe(signals::LocalDomainProcessingUnitInit,
                                &AggregatorError::onLocalDomainProcessingUnitInit, *this);
   notificationCenter.subscribe(signals::Error, &AggregatorError::onError, *this);
}

void AggregatorError::onLocalDomainInit() {
   try {
      simulation_unit_data_ = std::static_pointer_cast<SimulationUnitData>(
          _landUnitData->getVariable("simulationUnitData")->value().extract<std::shared_ptr<flint::IFlintData>>());
      spatial_location_info_ = std::static_pointer_cast<flint::SpatialLocationInfo>(
          _landUnitData->getVariable("spatialLocationInfo")->value().extract<std::shared_ptr<flint::IFlintData>>());
      simulation_unit_data_->log_error_count = 0;
   } catch (flint::VariableNotFoundException& exc) {
      auto str =
          ((boost::format("Variable not found: %1%") % *(boost::get_error_info<flint::VariableName>(exc))).str());
      BOOST_THROW_EXCEPTION(flint::LocalDomainError()
                            << flint::Details(str) << flint::LibraryName("moja,modules.chapman_richards")
                            << flint::ModuleName(BOOST_CURRENT_FUNCTION) << flint::ErrorCode(1));
   } catch (...) {
      if (_landUnitData->hasVariable("spatialLocationInfo")) {
         auto spatial_location_info = std::static_pointer_cast<flint::SpatialLocationInfo>(
             _landUnitData->getVariable("spatialLocationInfo")->value().extract<std::shared_ptr<flint::IFlintData>>());
         spatial_location_info->_library = "moja,modules.chapman_richards";
         spatial_location_info->_module = BOOST_CURRENT_FUNCTION;
         spatial_location_info->_errorCode = 1;
      }
      throw;
   }
}

void AggregatorError::onLocalDomainProcessingUnitInit() {
   if (system_settings_.log_error_per_block) simulation_unit_data_->log_error_count = 0;
}

void AggregatorError::onError(std::string msg) {
   if (system_settings_.log_errors && (system_settings_.log_error_max == 0 ||
                                       simulation_unit_data_->log_error_count < system_settings_.log_error_max)) {
      // Db's weren't handling the newlines very well
      boost::replace_all(msg, "\n", " ");
      simulation_unit_data_->error_log.accumulate(
          {0,
           spatial_location_info_->_tileIdx, spatial_location_info_->_blockIdx, spatial_location_info_->_cellIdx,
           spatial_location_info_->_randomSeedGlobal, spatial_location_info_->_randomSeedBlock,
           spatial_location_info_->_randomSeedCell, spatial_location_info_->_tileLatLon.lat,
           spatial_location_info_->_tileLatLon.lon, spatial_location_info_->_blockLatLon.lat,
           spatial_location_info_->_blockLatLon.lon, spatial_location_info_->_cellLatLon.lat,
           spatial_location_info_->_cellLatLon.lon,
           Poco::Nullable<Int64>(),  // dateRecordId: this was used when in the aggregator, but not now
           spatial_location_info_->_errorCode, spatial_location_info_->_library, spatial_location_info_->_module, msg});
   }
   simulation_unit_data_->log_error_count++;
}

}  // namespace indo
}  // namespace modules
}  // namespace moja
