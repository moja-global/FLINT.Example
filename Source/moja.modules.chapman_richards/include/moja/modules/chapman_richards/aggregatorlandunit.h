#pragma once

#include "moja/modules/chapman_richards/commondata.h"
#include "moja/modules/chapman_richards/record.h"
#include "moja/modules/chapman_richards/simulationunitdata.h"
#include "moja/modules/chapman_richards/systemsettings.h"

#include <moja/flint/modulebase.h>
#include <moja/flint/operationdatapackage.h>
#include <moja/flint/recordaccumulatorwithmutex.h>
#include <moja/flint/spatiallocationinfo.h>

#include <string>
#include <vector>

namespace moja::modules::chapman_richards {

class CHAPMAN_RICHARDS_API AggregatorLandUnit : public flint::ModuleBase {
  public:
   explicit AggregatorLandUnit(ObjectHolder& commonData)
       : system_settings_(commonData.systemSettings),
         date_dimension_(commonData.date_dimension),
         date_annual_dimension_(commonData.date_annual_dimension),
         tile_info_dimension_(commonData.tile_info_dimension),
         pool_info_dimension_(commonData.pool_info_dimension),
         location_non_temporal_dimension_(commonData.location_non_temporal_dimension),
         location_temporal_dimension_(commonData.location_temporal_dimension),
         classifier_set_dimension_(commonData.classifier_set_dimension),
         classifier_names_(commonData.classifier_names),
         classifier_set_var_(nullptr),
         build_worked_var_(nullptr),
         constructed_tile_id_(0) {}

   virtual ~AggregatorLandUnit() = default;

   void configure(const DynamicObject& config) override;
   void subscribe(NotificationCenter& notificationCenter) override;

   flint::ModuleTypes moduleType() override { return flint::ModuleTypes::Aggregator; }

   void onSystemInit() override;
   void onSystemShutdown() override;
   void onLocalDomainInit() override;
   void onLocalDomainShutdown() override;
   void onLocalDomainProcessingUnitInit() override;
   void onLocalDomainProcessingUnitShutdown() override;
   void onPreTimingSequence() override;
   void onTimingInit() override;
   void onTimingPostInit() override;
   void onTimingEndStep() override;
   void onOutputStep() override;
   void onTimingShutdown() override;
   void onError(std::string msg) override;
   void onPostDisturbanceEvent() override;

  private:
   void recordClassifierNames(const DynamicObject& classifierSet);
   void recordStockSet();
   void recordFluxSet();

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
   std::shared_ptr<const flint::SpatialLocationInfo> spatial_location_info_;

   std::string classifier_set_var_name_;

   // Variable handles
   flint::IVariable* classifier_set_var_;
   flint::IVariable* build_worked_var_;

   // -- Land Unit level Collections
   // -- these collections will be kep for each land unit and put into greater aggregation on LU success.
   // -- handles the case when a LU fails during processing rather than in the build land unit phase
   struct fluxDataLU {
      Int64 date_record_id;
      flint::FluxType flux_type_info_record_id;
      int src_idx;
      int dst_idx;
      double flux_value;
   };
   std::vector<fluxDataLU> fluxes_lu_;

   struct stockDataLU {
      Int64 date_record_id;
      int pool_idx;
      double value;
   };
   std::vector<stockDataLU> stock_values_lu_;

   // -- Data
   Poco::Nullable<Int64> date_record_id_;

   Poco::Nullable<UInt32> tile_idx_;
   Poco::Nullable<UInt32> block_idx_;
   Poco::Nullable<UInt32> cell_idx_;
   Poco::Nullable<double> tile_lat_;
   Poco::Nullable<double> tile_lon_;
   Poco::Nullable<double> block_lat_;
   Poco::Nullable<double> block_lon_;
   Poco::Nullable<double> cell_lat_;
   Poco::Nullable<double> cell_lon_;
   Poco::Nullable<UInt32> global_random_seed_;
   Poco::Nullable<UInt32> tile_random_seed_;
   Poco::Nullable<UInt32> block_random_seed_;
   Poco::Nullable<UInt32> cell_random_seed_;

   Int64 constructed_tile_id_;
};

}  // namespace moja::modules::chapman_richards
