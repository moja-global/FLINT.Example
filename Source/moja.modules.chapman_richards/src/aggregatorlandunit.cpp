#include "moja/modules/chapman_richards/aggregatorlandunit.h"

#include <moja/flint/flintexceptions.h>
#include <moja/flint/ipool.h>
#include <moja/flint/ivariable.h>
#include <moja/flint/operationresultsimple.h>

#include <moja/notificationcenter.h>
#include <moja/signals.h>

#include <fmt/format.h>

namespace moja::modules::chapman_richards {

void AggregatorLandUnit::configure(const DynamicObject& config) {
   system_settings_.tile_index_on = true;
   if (config.contains("tileIndex_dim_enabled")) {
      system_settings_.tile_index_on = config["tileIndex_dim_enabled"];
   }
   system_settings_.block_index_on = false;
   if (system_settings_.tile_index_on && config.contains("blockIndex_dim_enabled")) {  // can't have block without tile
      system_settings_.block_index_on = config["blockIndex_dim_enabled"];
   }
   system_settings_.cell_index_on = false;
   if (system_settings_.block_index_on && config.contains("cellIndex_dim_enabled")) {  // can't have cell without block
      system_settings_.cell_index_on = config["cellIndex_dim_enabled"];
   }
   system_settings_.do_run_stats_on = false;
   if (config.contains("do_run_statistics")) {
      system_settings_.do_run_stats_on = config["do_run_statistics"];
   }
   system_settings_.output_month12_only = true;
   if (config.contains("output_month_12_only")) {
      system_settings_.output_month12_only = config["output_month_12_only"];
   }
   system_settings_.do_stock = false;
   if (config.contains("do_stock")) {
      system_settings_.do_stock = config["do_stock"];
   }
   if (config.contains("reporting_classifier_set")) {
      classifier_set_var_name_ = config["reporting_classifier_set"].extract<std::string>();
   } else {
      classifier_set_var_name_ = "classifier_set";
   }
}

void AggregatorLandUnit::subscribe(NotificationCenter& notificationCenter) {
   notificationCenter.subscribe(signals::SystemInit, &AggregatorLandUnit::onSystemInit, *this);
   notificationCenter.subscribe(signals::SystemShutdown, &AggregatorLandUnit::onSystemShutdown, *this);
   notificationCenter.subscribe(signals::LocalDomainInit, &AggregatorLandUnit::onLocalDomainInit, *this);
   notificationCenter.subscribe(signals::LocalDomainShutdown, &AggregatorLandUnit::onLocalDomainShutdown, *this);
   notificationCenter.subscribe(signals::LocalDomainProcessingUnitInit,
                                &AggregatorLandUnit::onLocalDomainProcessingUnitInit, *this);
   notificationCenter.subscribe(signals::LocalDomainProcessingUnitShutdown,
                                &AggregatorLandUnit::onLocalDomainProcessingUnitShutdown, *this);
   notificationCenter.subscribe(signals::PreTimingSequence, &AggregatorLandUnit::onPreTimingSequence, *this);
   notificationCenter.subscribe(signals::TimingInit, &AggregatorLandUnit::onTimingInit, *this);
   notificationCenter.subscribe(signals::TimingPostInit, &AggregatorLandUnit::onTimingPostInit, *this);
   notificationCenter.subscribe(signals::TimingEndStep, &AggregatorLandUnit::onTimingEndStep, *this);
   notificationCenter.subscribe(signals::OutputStep, &AggregatorLandUnit::onOutputStep, *this);
   notificationCenter.subscribe(signals::TimingShutdown, &AggregatorLandUnit::onTimingShutdown, *this);
   notificationCenter.subscribe(signals::Error, &AggregatorLandUnit::onError, *this);
   notificationCenter.subscribe(signals::PostDisturbanceEvent, &AggregatorLandUnit::onPostDisturbanceEvent, *this);
}

void AggregatorLandUnit::onSystemInit() {
   try {
      simulation_unit_data_ = std::static_pointer_cast<SimulationUnitData>(
          _landUnitData->getVariable("simulationUnitData")->value().extract<std::shared_ptr<flint::IFlintData>>());
      simulation_unit_data_->start_system_time = DateTime::now();

      // Pools and their Ids are constant for a whole simulation (across threads as well). So create them here
      for (std::shared_ptr<const flint::IPool> pool : _landUnitData->poolCollection()) {
         pool_info_dimension_->accumulate(
             {pool->name(), pool->description(), pool->idx(), pool->order(), pool->scale(), pool->units()},
             pool->idx());
      }
   } catch (flint::VariableNotFoundException& exc) {
      const auto str = fmt::format("Variable not found: {}", *(boost::get_error_info<flint::VariableName>(exc)));
      BOOST_THROW_EXCEPTION(flint::LocalDomainError()
                            << flint::Details(str) << flint::LibraryName("moja.modules.chapman_richards")
                            << flint::ModuleName(BOOST_CURRENT_FUNCTION) << flint::ErrorCode(1));
   } catch (...) {
      if (_landUnitData->hasVariable("spatialLocationInfo")) {
         auto spatial_location_info = std::static_pointer_cast<flint::SpatialLocationInfo>(
             _landUnitData->getVariable("spatialLocationInfo")->value().extract<std::shared_ptr<flint::IFlintData>>());
         spatial_location_info->_library = "moja.modules.chapman_richards";
         spatial_location_info->_module = BOOST_CURRENT_FUNCTION;
         spatial_location_info->_errorCode = 1;
      }
      throw;
   }
}

void AggregatorLandUnit::onSystemShutdown() {
   try {
      simulation_unit_data_->end_system_time = DateTime::now();

   } catch (flint::VariableNotFoundException& exc) {
      const auto str = fmt::format("Variable not found: {}", *(boost::get_error_info<flint::VariableName>(exc)));
      BOOST_THROW_EXCEPTION(flint::LocalDomainError()
                            << flint::Details(str) << flint::LibraryName("moja.modules.chapman_richards")
                            << flint::ModuleName(BOOST_CURRENT_FUNCTION) << flint::ErrorCode(1));
   } catch (...) {
      if (_landUnitData->hasVariable("spatialLocationInfo")) {
         auto spatial_location_info = std::static_pointer_cast<flint::SpatialLocationInfo>(
             _landUnitData->getVariable("spatialLocationInfo")->value().extract<std::shared_ptr<flint::IFlintData>>());
         spatial_location_info->_library = "moja.modules.chapman_richards";
         spatial_location_info->_module = BOOST_CURRENT_FUNCTION;
         spatial_location_info->_errorCode = 1;
      }
      throw;
   }
}

void AggregatorLandUnit::onLocalDomainInit() {
   try {
      simulation_unit_data_ = std::static_pointer_cast<SimulationUnitData>(
          _landUnitData->getVariable("simulationUnitData")->value().extract<std::shared_ptr<flint::IFlintData>>());
      spatial_location_info_ = std::static_pointer_cast<flint::SpatialLocationInfo>(
          _landUnitData->getVariable("spatialLocationInfo")->value().extract<std::shared_ptr<flint::IFlintData>>());
      build_worked_var_ = _landUnitData->getVariable("landUnitBuildSuccess");

      classifier_set_var_ = _landUnitData->getVariable(classifier_set_var_name_);
      simulation_unit_data_->start_local_domain_time = DateTime::now();

      date_record_id_.clear();
      global_random_seed_.clear();
      tile_random_seed_.clear();
      block_random_seed_.clear();
      cell_random_seed_.clear();

      if (!system_settings_.tile_index_on) {
         tile_idx_.clear();
         block_idx_.clear();
         cell_idx_.clear();
         tile_lat_.clear();
         tile_lon_.clear();
         block_lat_.clear();
         block_lon_.clear();
         cell_lat_.clear();
         cell_lon_.clear();
      } else if (!system_settings_.block_index_on) {
         block_idx_.clear();
         cell_idx_.clear();
         block_lat_.clear();
         block_lon_.clear();
         cell_lat_.clear();
         cell_lon_.clear();
      } else if (!system_settings_.cell_index_on) {
         cell_idx_.clear();
         cell_lat_.clear();
         cell_lon_.clear();
      }
      simulation_unit_data_->lu_count_local_domain = 0;
   } catch (flint::VariableNotFoundException& exc) {
      const auto variable_name = *(boost::get_error_info<flint::VariableName>(exc));
      const auto str = fmt::format("Variable not found: {}", variable_name);
      BOOST_THROW_EXCEPTION(flint::LocalDomainError()
                            << flint::Details(str) << flint::LibraryName("moja.modules.chapman_richards")
                            << flint::ModuleName(BOOST_CURRENT_FUNCTION) << flint::ErrorCode(1));
   } catch (...) {
      if (_landUnitData->hasVariable("spatialLocationInfo")) {
         auto spatial_location_info = std::static_pointer_cast<flint::SpatialLocationInfo>(
             _landUnitData->getVariable("spatialLocationInfo")->value().extract<std::shared_ptr<flint::IFlintData>>());
         spatial_location_info->_library = "moja.modules.chapman_richards";
         spatial_location_info->_module = BOOST_CURRENT_FUNCTION;
         spatial_location_info->_errorCode = 1;
      }
      throw;
   }
}

void AggregatorLandUnit::onLocalDomainShutdown() { simulation_unit_data_->end_local_domain_time = DateTime::now(); }

void AggregatorLandUnit::onLocalDomainProcessingUnitInit() {
   simulation_unit_data_->start_processing_unit_time = DateTime::now();
   simulation_unit_data_->process_unit_count++;
   simulation_unit_data_->lu_count_processing_unit = 0;

   // We can't clear them here because multiple outputers might be using the data (i.e. SQLite & PostgreSQL)
   // We also can't rely on the order of the Module declarations as this Aggregator needs to be defined first so
   // fluxes/stock are recorded for outputters 1) Write an extra Module that works as a pair to clear lists, always
   // defined last 2) Have the outputter clear the lists but then we would be limited to a single version per run NOTE:
   // this is only an issue when block index is on
   //_simulationUnitData->_flux2Results.clear();
   //_simulationUnitData->_stock2Results.clear();
}

void AggregatorLandUnit::onLocalDomainProcessingUnitShutdown() {
   simulation_unit_data_->end_processing_unit_time = DateTime::now();
}

void AggregatorLandUnit::onPreTimingSequence() {
   try {
      const bool build_worked = build_worked_var_->value();
      if (!build_worked) return;
   } catch (...) {
      if (_landUnitData->hasVariable("spatialLocationInfo")) {
         auto spatial_location_info = std::static_pointer_cast<flint::SpatialLocationInfo>(
             _landUnitData->getVariable("spatialLocationInfo")->value().extract<std::shared_ptr<flint::IFlintData>>());
         spatial_location_info->_library = "moja.modules.chapman_richards";
         spatial_location_info->_module = BOOST_CURRENT_FUNCTION;
         spatial_location_info->_errorCode = 1;
      }
      throw;
   }
}

void AggregatorLandUnit::onTimingInit() {
   try {
      simulation_unit_data_->lu_count_processing_unit++;
      simulation_unit_data_->lu_count_local_domain++;

      constructed_tile_id_ = 0;  // TileIdx [24 bits], blockIdx [12 bits], cellIdx [28 bits]

      global_random_seed_ = spatial_location_info_->_randomSeedGlobal;

      if (system_settings_.tile_index_on) {
         tile_idx_ = spatial_location_info_->_tileIdx;
         tile_lat_ = spatial_location_info_->_tileLatLon.lat;
         tile_lon_ = spatial_location_info_->_tileLatLon.lon;
         tile_random_seed_ = spatial_location_info_->_randomSeedTile;

         constructed_tile_id_ += (static_cast<Int64>(spatial_location_info_->_tileIdx) << 40);

         if (system_settings_.block_index_on) {
            block_idx_ = spatial_location_info_->_blockIdx;
            block_lat_ = spatial_location_info_->_blockLatLon.lat;
            block_lon_ = spatial_location_info_->_blockLatLon.lon;
            block_random_seed_ = spatial_location_info_->_randomSeedBlock;

            constructed_tile_id_ += (static_cast<Int64>(spatial_location_info_->_blockIdx) << 28);

            if (system_settings_.cell_index_on) {
               cell_idx_ = spatial_location_info_->_cellIdx;
               cell_lat_ = spatial_location_info_->_cellLatLon.lat;
               cell_lon_ = spatial_location_info_->_cellLatLon.lon;
               cell_random_seed_ = spatial_location_info_->_randomSeedCell;

               constructed_tile_id_ += spatial_location_info_->_cellIdx;
            }
         }
      }
      // Clear the LU dataset in prep for this LU processing
      fluxes_lu_.clear();
      stock_values_lu_.clear();
   } catch (...) {
      if (_landUnitData->hasVariable("spatialLocationInfo")) {
         auto spatial_location_info = std::static_pointer_cast<flint::SpatialLocationInfo>(
             _landUnitData->getVariable("spatialLocationInfo")->value().extract<std::shared_ptr<flint::IFlintData>>());
         spatial_location_info->_library = "moja.modules.chapman_richards";
         spatial_location_info->_module = BOOST_CURRENT_FUNCTION;
         spatial_location_info->_errorCode = 1;
      }
      throw;
   }
}

void AggregatorLandUnit::onTimingPostInit() {
   recordStockSet();
   recordFluxSet();
}

void AggregatorLandUnit::onTimingEndStep() { recordFluxSet(); }

void AggregatorLandUnit::onOutputStep() { recordStockSet(); }

void AggregatorLandUnit::recordClassifierNames(const DynamicObject& classifierSet) {
   if (!classifier_names_->empty()) {
      return;
   }
   for (const auto& classifier : classifierSet) {
      auto name = classifier.first;
      std::replace(name.begin(), name.end(), '.', '_');
      std::replace(name.begin(), name.end(), ' ', '_');
      classifier_names_->push_back(name);
   }
}

void AggregatorLandUnit::onTimingShutdown() {
   try {
      // Here we can assume pixel has worked without any errors - if there had been processing would've been aborted
      // So set all LU level dimensions and then accumulate fluxes into
      const auto stored_tile_info_record = tile_info_dimension_->accumulate(
          {tile_idx_, block_idx_, cell_idx_, tile_lat_, tile_lon_, block_lat_, block_lon_, cell_lat_, cell_lon_,
           global_random_seed_, tile_random_seed_, block_random_seed_, cell_random_seed_},
          constructed_tile_id_);
      const short local_domain_id = 0;  // short(_simulationUnitData->_localDomainId);

      /// -------------------------------------
      /// Do new Classifier Dimension
      /// -------------------------------------
      // Classifier set information.
      const auto& land_unit_classifier_set = classifier_set_var_->value().extract<DynamicObject>();
      std::vector<Poco::Nullable<std::string>> classifier_set;
      if (classifier_names_->empty()) {
         recordClassifierNames(land_unit_classifier_set);
      }

      for (const auto& classifier : land_unit_classifier_set) {
         Poco::Nullable<std::string> classifier_value;
         if (!classifier.second.isEmpty()) {
            if (classifier.second.type() == typeid(UInt8)) {
               const auto val = classifier.second.convert<int>();
               classifier_value = std::to_string(val);
            } else {
               classifier_value = classifier.second.convert<std::string>();
            }
         }
         classifier_set.push_back(classifier_value);
      }

      const ClassifierSetRecord record(classifier_set);
      const auto stored_record = classifier_set_dimension_->accumulate(record);
      const auto classifier_set_record_id = stored_record->getId();

      /// -------------------------------------
      /// Do new LocationNonTemporal Dimension
      /// -------------------------------------
      const auto stored_location_non_temporal_record = location_non_temporal_dimension_->accumulate(
          {stored_tile_info_record->getId(), classifier_set_record_id, 1, spatial_location_info_->_landUnitArea});
      const auto location_non_temporal_id = stored_location_non_temporal_record->getId();

      /// -------------------------------------
      /// Handle Flux aggregation
      /// Copy flux record vector into main dataset as we have had no errors in LU
      /// -------------------------------------

      const int uncertainty_interation = 0; //_landUnitData->uncertainty().iteration();

      auto& flux_results = simulation_unit_data_->flux_results;
      for (auto& flux : fluxes_lu_) {
         flux_results.accumulate(
             {local_domain_id, short(uncertainty_interation), short(flux.date_record_id), int(location_non_temporal_id),
              short(flux.flux_type_info_record_id), short(flux.src_idx), short(flux.dst_idx)},
             {flux.flux_value});
      }
      fluxes_lu_.clear();

      /// -------------------------------------
      /// Handle Stock aggregation
      /// Copy stock record vector into main dataset as we have had no errors in LU
      /// -------------------------------------
      auto& stock_results = simulation_unit_data_->stock_results;
      for (const auto& stock : stock_values_lu_) {
         stock_results.accumulate({local_domain_id, short(uncertainty_interation), short(stock.date_record_id),
                                   int(location_non_temporal_id), short(stock.pool_idx)},
                                  {stock.value});
      }
      stock_values_lu_.clear();
   } catch (...) {
      if (_landUnitData->hasVariable("spatialLocationInfo")) {
         auto spatial_location_info = std::static_pointer_cast<flint::SpatialLocationInfo>(
             _landUnitData->getVariable("spatialLocationInfo")->value().extract<std::shared_ptr<flint::IFlintData>>());
         spatial_location_info->_library = "moja.modules.chapman_richards";
         spatial_location_info->_module = BOOST_CURRENT_FUNCTION;
         spatial_location_info->_errorCode = 1;
      }
      throw;
   }
}

void AggregatorLandUnit::onError(std::string msg) {
   fluxes_lu_.clear();  // dump any fluxes stored
   stock_values_lu_.clear();
}

void AggregatorLandUnit::onPostDisturbanceEvent() { recordFluxSet(); }

void AggregatorLandUnit::recordStockSet() {
   if (!system_settings_.do_stock) return;

   const auto timing = _landUnitData->timing();
   const auto cur_step = timing->step();
   const auto cur_sub_step = timing->subStep();

   auto reporting_end_date = timing->curEndDate();
   date_record_id_.clear();

   if (system_settings_.output_month12_only && reporting_end_date.month() % 12 == 0) {
      const auto stored_date_record =
          date_annual_dimension_->accumulate({reporting_end_date.year()}, reporting_end_date.year());
      date_record_id_ = stored_date_record->getId();
   }
   if (!system_settings_.output_month12_only) {
      const auto stored_date_record = date_dimension_->accumulate(
          {cur_step, cur_sub_step, reporting_end_date.year(), reporting_end_date.month(), reporting_end_date.day(),
           timing->fractionOfStep(), timing->stepLengthInYears()});
      date_record_id_ = stored_date_record->getId();
   }

   // Get current pool data.
   if ((system_settings_.output_month12_only && reporting_end_date.month() % 12 == 0) ||
       !system_settings_.output_month12_only) {
      auto pools = _landUnitData->poolCollection();
      for (auto& pool : _landUnitData->poolCollection()) {
         const auto pool_value = pool->value() * spatial_location_info_->_landUnitArea;
         stock_values_lu_.emplace_back(stockDataLU{date_record_id_, pool->idx(), pool_value});
      }
   }
}

void AggregatorLandUnit::recordFluxSet() {
   try {
      const auto timing = _landUnitData->timing();
      const auto cur_step = timing->step();
      const auto cur_sub_step = timing->subStep();

      // If Flux set is empty return immediately
      if (!_landUnitData->hasLastAppliedOperationResults()) return;

      // Find the date dimension record.
      // Int64 dateRecordId;
      auto reporting_end_date = timing->curEndDate();
      reporting_end_date.addMilliseconds(-1);
      date_record_id_.clear();
      if (system_settings_.output_month12_only) {
         const auto stored_date_record =
             date_annual_dimension_->accumulate({reporting_end_date.year()}, reporting_end_date.year());
         date_record_id_ = stored_date_record->getId();
      } else {
         const auto stored_date_record = date_dimension_->accumulate(
             {cur_step, cur_sub_step, reporting_end_date.year(), reporting_end_date.month(), reporting_end_date.day(),
              timing->fractionOfStep(), timing->stepLengthInYears()});
         date_record_id_ = stored_date_record->getId();
      }

      for (const auto& operation_result : _landUnitData->getOperationLastAppliedIterator()) {
         auto simple_result = std::static_pointer_cast<flint::OperationResultSimple>(operation_result);

         auto flux_type_info_record_id = flint::FluxType::Unclassified;

         if (operation_result->hasDataPackage()) {
            const auto data_packet =
                operation_result->dataPackage().extract<std::shared_ptr<flint::OperationDataPackage>>();
            flux_type_info_record_id = data_packet->_fluxType;
         }

         for (const auto& flux : simple_result->fluxes()) {
            const auto src_ix = flux.source();
            const auto dst_ix = flux.sink();
            if (src_ix == dst_ix) continue;  // don't process diagonal - flux to & from same pool is ignored
            const auto flux_value = flux.value() * spatial_location_info_->_landUnitArea;
            fluxes_lu_.emplace_back(fluxDataLU{date_record_id_, flux_type_info_record_id, src_ix, dst_ix, flux_value});
         }
      }
   } catch (...) {
      if (_landUnitData->hasVariable("spatialLocationInfo")) {
         auto spatial_location_info = std::static_pointer_cast<flint::SpatialLocationInfo>(
             _landUnitData->getVariable("spatialLocationInfo")->value().extract<std::shared_ptr<flint::IFlintData>>());
         spatial_location_info->_library = "moja.modules.chapman_richards";
         spatial_location_info->_module = BOOST_CURRENT_FUNCTION;
         spatial_location_info->_errorCode = 1;
      }
      throw;
   }
}

}  // namespace chapman_richards
