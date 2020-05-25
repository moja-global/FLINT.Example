#include "moja/modules/chapman_richards/runstatistics.h"

#include <moja/logging.h>

#include <Poco/Data/Session.h>

#include <boost/format.hpp>

namespace moja {
namespace modules {
namespace chapman_richards {

RunStatistics::RunStatistics()
    : land_unit_valid_counties(0),
      land_unit_simulated_checked(0),
      land_unit_simulated_passed(0),
      land_cover_no_data_errors(0),
      land_cover_combination_errors(0),
      land_cover_combination_filtered(0),
      land_unit_transform_counties_passed(0),
      land_unit_transform_counties_failed(0) {
   stat_tuple_list = {
       std::tuple<std::string, Int64&>("landUnitTransformCountiesPassed", land_unit_transform_counties_passed),
       std::tuple<std::string, Int64&>("landUnitTransformCountiesFailed", land_unit_transform_counties_failed),
       std::tuple<std::string, Int64&>("landUnitValidCounties", land_unit_valid_counties),
       std::tuple<std::string, Int64&>("landUnitSimulatedChecked", land_unit_simulated_checked),
       std::tuple<std::string, Int64&>("landUnitSimulatedPassed", land_unit_simulated_passed),
       std::tuple<std::string, Int64&>("landCoverNoDataErrors", land_cover_no_data_errors),
       std::tuple<std::string, Int64&>("landCoverCombinationFiltered", land_cover_combination_filtered),
       std::tuple<std::string, Int64&>("landCoverCombinationErrors", land_cover_combination_errors)};
}

void RunStatistics::writeToSQLite(Poco::Data::Session& session, const std::string& tableName, int localDomainId,
                                  int processingUnitId, std::string data) {
   for (auto& record : stat_tuple_list) {
      Poco::Data::Statement insert(session);
      insert << "INSERT INTO " << tableName << " VALUES(?, ?, ?, ?, ?)", Poco::Data::Keywords::useRef(localDomainId),
          Poco::Data::Keywords::useRef(processingUnitId), Poco::Data::Keywords::useRef(data),
          Poco::Data::Keywords::useRef(std::get<0>(record)), Poco::Data::Keywords::useRef(std::get<1>(record));
      insert.execute();
   }
}

}  // namespace chapman_richards
}  // namespace modules
}  // namespace moja
