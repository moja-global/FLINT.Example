#pragma once

#include "moja/modules/chapman_richards/_modules.chapman_richards_exports.h"

#include <moja/flint/iflintdata.h>

namespace Poco {
namespace Data {
class Session;
}
}  // namespace Poco

namespace moja {
namespace modules {
namespace chapman_richards {

class CHAPMAN_RICHARDS_API RunStatistics : public flint::IFlintData {
  public:
   RunStatistics();
   virtual ~RunStatistics() = default;
   RunStatistics(const RunStatistics&) = delete;
   RunStatistics& operator=(const RunStatistics&) = delete;

   void configure(DynamicObject config, const flint::ILandUnitController& landUnitController,
                  moja::datarepository::DataRepository& dataRepository) override {}

   void writeToSQLite(Poco::Data::Session& session, const std::string& tableName, int localDomainId,
                      int processingUnitId, std::string data);

   // Fixed data
   Int64 land_unit_valid_counties;
   Int64 land_unit_simulated_checked;
   Int64 land_unit_simulated_passed;
   Int64 land_cover_no_data_errors;
   Int64 land_cover_combination_errors;
   Int64 land_cover_combination_filtered;
   Int64 land_unit_transform_counties_passed;
   Int64 land_unit_transform_counties_failed;

   std::vector<std::tuple<std::string, Int64&>> stat_tuple_list;
};

}  // namespace chapman_richards
}  // namespace modules
}  // namespace moja
