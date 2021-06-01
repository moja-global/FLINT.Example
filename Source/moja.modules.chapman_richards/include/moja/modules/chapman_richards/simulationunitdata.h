#pragma once

#include "moja/modules/chapman_richards/_modules.chapman_richards_exports.h"
#include "moja/modules/chapman_richards/record.h"

#include <moja/flint/iflintdata.h>
#include <moja/flint/recordaccumulator.h>

#include <moja/datetime.h>
#include <moja/dynamic.h>

namespace moja::modules::chapman_richards {

class CHAPMAN_RICHARDS_API SimulationUnitData : public flint::IFlintData {
  public:
   SimulationUnitData()
       : local_domain_id(-1),
         land_unit_area(0.0),
         log_error_count(0),
         process_unit_count(0),
         lu_count_processing_unit(0),
         lu_count_local_domain(0) {}

   virtual ~SimulationUnitData() = default;
   SimulationUnitData(const SimulationUnitData&) = delete;
   SimulationUnitData& operator=(const SimulationUnitData&) = delete;

   void configure(DynamicObject config, const flint::ILandUnitController& landUnitController,
                  datarepository::DataRepository& dataRepository) override;

   DynamicVar getProperty(const std::string& key) const override;
   // Common data
   int local_domain_id;
   double land_unit_area;

   // Collections shared within a thread
   flint::RecordAccumulatorMap<FluxRow, FluxRecordConverter, FluxKey, FluxValue> flux_results;
   flint::RecordAccumulatorMap<StockRow, StockRecordConverter, StockKey, StockValue> stock_results;

   // Error log members
   int log_error_count;
   flint::RecordAccumulator2<ErrorRow, ErrorRecord> error_log;

   // -- Run Stats
   int process_unit_count;
   Int64 lu_count_processing_unit;
   Int64 lu_count_local_domain;

   DateTime start_system_time;
   DateTime end_system_time;
   DateTime start_local_domain_time;
   DateTime end_local_domain_time;
   DateTime start_processing_unit_time;
   DateTime end_processing_unit_time;
};

inline void SimulationUnitData::configure(DynamicObject config, const flint::ILandUnitController& landUnitController,
                                          datarepository::DataRepository& dataRepository) {}

inline DynamicVar SimulationUnitData::getProperty(const std::string& key) const { return DynamicVar(); }

}  // namespace moja::modules::chapman_richards
