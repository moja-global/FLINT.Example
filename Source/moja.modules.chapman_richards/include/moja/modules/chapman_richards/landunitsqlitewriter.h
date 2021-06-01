#pragma once

#include "moja/modules/chapman_richards/_modules.chapman_richards_exports.h"
#include "moja/modules/chapman_richards/commondata.h"
#include "moja/modules/chapman_richards/record.h"
#include "moja/modules/chapman_richards/simulationunitdata.h"
#include "moja/modules/chapman_richards/systemsettings.h"

#include <moja/flint/modulebase.h>
#include <moja/flint/recordaccumulatorwithmutex.h>

#include <functional>

namespace Poco::Data {
class Session;
}  // namespace Poco::Data

namespace moja::modules::chapman_richards {

class CHAPMAN_RICHARDS_API LandUnitSQLiteWriter : public flint::ModuleBase {
  public:
   LandUnitSQLiteWriter(ObjectHolder& commonData)
       : system_settings_(commonData.systemSettings),
         date_dimension_(commonData.date_dimension),
         date_annual_dimension_(commonData.date_annual_dimension),
         tile_info_dimension_(commonData.tile_info_dimension),
         pool_info_dimension_(commonData.pool_info_dimension),

         location_non_temporal_dimension_(commonData.location_non_temporal_dimension),
         location_temporal_dimension_(commonData.location_temporal_dimension),
         classifier_set_dimension_(commonData.classifier_set_dimension),
         classifier_names_(commonData.classifier_names) {}

   virtual ~LandUnitSQLiteWriter() = default;

   void configure(const DynamicObject& config) override;
   void subscribe(NotificationCenter& notificationCenter) override;

   flint::ModuleTypes moduleType() override { return flint::ModuleTypes::Aggregator; }

   void onSystemInit() override;
   void onSystemShutdown() override;
   void onLocalDomainInit() override;
   void onLocalDomainShutdown() override;
   void onLocalDomainProcessingUnitShutdown() override;

  private:
   void writeStock() const;
   void writeFlux() const;
   void writeErrorLog() const;
   void writeRunStats(std::string unitLabel, DateTime& startTime, DateTime& finishTime, Int64 luCount) const;
   void writeRunSummary(std::string unitLabel, DateTime& startTime, DateTime& finishTime, Int64 luCount) const;

   template <typename TAccumulator>
   void load(Poco::Data::Session& session, const std::string& table, std::shared_ptr<TAccumulator> dataDimension);

   static void tryExecute(Poco::Data::Session& session, std::function<void(Poco::Data::Session&)> fn);

   typedef Poco::Tuple<Int64, int, std::string, std::string, std::string> runStatDataRecord;

   // -- Shared Data
   SystemSettings& system_settings_;
   std::shared_ptr<flint::RecordAccumulatorWithMutex2<DateRow, DateRecord>> date_dimension_;
   std::shared_ptr<flint::RecordAccumulatorWithMutex2<DateAnnualRow, DateAnnualRecord>> date_annual_dimension_;
   std::shared_ptr<flint::RecordAccumulatorWithMutex2<TileInfoRow, TileInfoRecord>> tile_info_dimension_;
   std::shared_ptr<flint::RecordAccumulatorWithMutex2<PoolInfoRow, PoolInfoRecord>> pool_info_dimension_;

   std::shared_ptr<flint::RecordAccumulatorWithMutex2<LocationNonTemporalRow, LocationNonTemporalRecord>>
       location_non_temporal_dimension_;
   std::shared_ptr<flint::RecordAccumulatorWithMutex2<LocationTemporalRow, LocationTemporalRecord>>
       location_temporal_dimension_;
   std::shared_ptr<flint::RecordAccumulatorWithMutex2<ClassifierSetRow, ClassifierSetRecord>> classifier_set_dimension_;
   std::shared_ptr<std::vector<std::string>> classifier_names_;

   // -- flintdata objects data
   std::shared_ptr<SimulationUnitData> simulation_unit_data_;
};

}  // namespace moja::modules::chapman_richards
