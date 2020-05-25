#include "moja/modules/chapman_richards/landunitsqlitewriter.h"

#include "moja/modules/chapman_richards/commondata.h"
#include "moja/modules/chapman_richards/runstatistics.h"

#include <moja/flint/idnamedesccollection.h>
#include <moja/flint/ivariable.h>
#include <moja/flint/spatiallocationinfo.h>
#include <moja/flint/variableandpoolstringbuilder.h>

#include <moja/logging.h>
#include <moja/notificationcenter.h>
#include <moja/signals.h>

#include <Poco/Data/SQLite/Connector.h>
#include <Poco/Data/SQLite/SQLiteException.h>
#include <Poco/Data/Session.h>
#include <Poco/Exception.h>
#include <Poco/File.h>
#include <Poco/Path.h>

#include <boost/format.hpp>

#include <thread>

using namespace Poco::Data::Keywords;
using namespace Poco::Data;

namespace moja {
namespace modules {
namespace chapman_richards {

constexpr unsigned int SQLITE_RETRY_ATTEMPTS = 10000;
constexpr std::chrono::milliseconds SQLITE_RETRY_SLEEP = std::chrono::milliseconds(200);
// --------------------------------------------------------------------------------------------

void LandUnitSQLiteWriter::configure(const DynamicObject& config) {
   system_settings_.db_name_sqlite = config["databasename"].convert<std::string>();
   system_settings_.generated_db_name_sqlite = system_settings_.db_name_sqlite;
}

// --------------------------------------------------------------------------------------------

void LandUnitSQLiteWriter::subscribe(NotificationCenter& notificationCenter) {
   notificationCenter.subscribe(signals::SystemInit, &LandUnitSQLiteWriter::onSystemInit, *this);
   notificationCenter.subscribe(signals::SystemShutdown, &LandUnitSQLiteWriter::onSystemShutdown, *this);
   notificationCenter.subscribe(signals::LocalDomainInit, &LandUnitSQLiteWriter::onLocalDomainInit, *this);
   notificationCenter.subscribe(signals::LocalDomainShutdown, &LandUnitSQLiteWriter::onLocalDomainShutdown, *this);
   notificationCenter.subscribe(signals::LocalDomainProcessingUnitShutdown,
                                &LandUnitSQLiteWriter::onLocalDomainProcessingUnitShutdown, *this);
}

// --------------------------------------------------------------------------------------------

void LandUnitSQLiteWriter::onSystemInit() {
   simulation_unit_data_ = std::static_pointer_cast<SimulationUnitData>(
       _landUnitData->getVariable("simulationUnitData")->value().extract<std::shared_ptr<flint::IFlintData>>());

   try {
      // build the filename using pools and variable values
      flint::VariableAndPoolStringBuilder databaseNameBuilder(_landUnitData.get(), system_settings_.db_name_sqlite);
      system_settings_.generated_db_name_sqlite = databaseNameBuilder.result();

      // Force creation of specified directory, after expanded system environment variables
      Poco::File file(Poco::Path(Poco::Path::expand(system_settings_.generated_db_name_sqlite), Poco::Path::PATH_NATIVE)
                          .parent()
                          .makeAbsolute());
      try {
         file.createDirectories();
      } catch (Poco::FileExistsException&) { /* Poco has a bug here, exception shouldn't be thrown, has been fixed
                                                in 1.7.8 */
      }

      MOJA_LOG_INFO << "LandUnitSQLiteWriter resolved output path is - " << file.path();

      auto retry = false;
      auto max_retries = SQLITE_RETRY_ATTEMPTS;
      do {
         try {
            retry = false;
            SQLite::Connector::registerConnector();
            Session session("SQLite", system_settings_.generated_db_name_sqlite);

            std::vector<std::string> ddl{
                "DROP TABLE IF EXISTS run_information",
                "DROP TABLE IF EXISTS date_dimension",
                "DROP TABLE IF EXISTS poolinfo_dimension",
                "DROP TABLE IF EXISTS tileinfo_dimension",
                "DROP TABLE IF EXISTS flux_reporting_results",
                "DROP TABLE IF EXISTS stock_reporting_results",
                "DROP TABLE IF EXISTS error_log",
                "DROP TABLE IF EXISTS locationnontemp_dimension",
                "DROP TABLE IF EXISTS locationtemp_dimension",
                "DROP TABLE IF EXISTS classifierset_dimension",
                "CREATE TABLE poolinfo_dimension				(poolinfo_dimension_id_pk	"
                "			UNSIGNED BIG INT NOT NULL, name VARCHAR(255), description VARCHAR(255), "
                "internalIndex INTEGER, displayOrder INTEGER, scale FLOAT, units VARCHAR(255))",
                "CREATE TABLE tileinfo_dimension				(tileinfo_dimension_id_pk	"
                "			UNSIGNED BIG INT NOT NULL, tile_index INTEGER, block_index INTEGER, cell_index "
                "INTEGER, tile_latitude FLOAT, tile_logitude FLOAT, block_latitude FLOAT, block_logitude FLOAT, "
                "cell_latitude FLOAT, cell_logitude FLOAT, random_seed_global UNSIGNED BIG INT, random_seed_tile "
                "UNSIGNED BIG INT, random_seed_block UNSIGNED BIG INT, random_seed_cell UNSIGNED BIG INT)",
                "CREATE TABLE flux_reporting_results 			(flux_reporting_results_id_pk 		"
                "	UNSIGNED BIG INT NOT NULL, iteration INTEGER NOT NULL, localDomainId INTEGER NOT NULL, date_dimension_id_fk UNSIGNED BIG "
                "INT NOT NULL, location_dimension_id_fk UNSIGNED BIG INT NOT NULL, fluxtypeinfo_dimension_id_fk "
                "UNSIGNED BIG INT, source_poolinfo_dimension_id_fk UNSIGNED BIG INT NOT NULL, "
                "sink_poolinfo_dimension_id_fk UNSIGNED BIG INT NOT NULL, flux FLOAT, itemCount INTEGER NOT NULL)",

                "CREATE TABLE locationnontemp_dimension 		(locationnontemp_dimension_id_pk	"
                "	UNSIGNED BIG INT, tileinfo_dimension_id_fk UNSIGNED BIG INT, classifierset_dimension_id_fk "
                "UNSIGNED BIG INT, unitCount UNSIGNED BIG INT, unitAreaSum FLOAT)",
                "CREATE TABLE locationtemp_dimension 			(locationntemp_dimension_id_pk		"
                "	UNSIGNED BIG INT, date_dimension_id_fk UNSIGNED BIG INT NOT NULL, "
                "classifierset_dimension_id_fk UNSIGNED BIG INT, unitCount UNSIGNED BIG INT, unitAreaSum FLOAT)"};

            if (system_settings_.do_run_stats_on)
               ddl.push_back(
                   "CREATE TABLE run_information			(localDomainId				"
                   "	INTEGER NOT NULL, processingUnit INTEGER, module VARCHAR(255), name VARCHAR(255), value "
                   "VARCHAR(255))");
            if (system_settings_.output_month12_only)
               ddl.push_back(
                   "CREATE TABLE date_dimension			(date_dimension_id_pk			UNSIGNED BIG "
                   "INT, year INTEGER)");
            else
               ddl.push_back(
                   "CREATE TABLE date_dimension			(date_dimension_id_pk			UNSIGNED BIG "
                   "INT, step INTEGER, substep INTEGER, year INTEGER, month INTEGER, day INTEGER, fracOfStep FLOAT, "
                   "lengthOfStepInYears FLOAT)");
            if (system_settings_.do_stock)
               ddl.push_back(
                   "CREATE TABLE stock_reporting_results	(stock_reporting_results_id_pk	UNSIGNED BIG INT NOT "
                   "NULL, iteration INTEGER NOT NULL, localDomainId INTEGER NOT NULL, date_dimension_id_fk UNSIGNED BIG INT NOT NULL, "
                   "location_dimension_id_fk UNSIGNED BIG INT NOT NULL, poolinfo_dimension_id_fk UNSIGNED BIG INT NOT "
                   "NULL, value FLOAT, itemCount INTEGER NOT NULL)");
            if (system_settings_.log_errors)
               ddl.push_back(
                   "CREATE TABLE error_log				(error_log_id_pk			"
                   "	UNSIGNED BIG INT, localDomainId INTEGER NOT NULL, tile_index INTEGER, block_index INTEGER, "
                   "cell_index INTEGER, global_seed UNSIGNED BIG INT, block_seed UNSIGNED BIG INT, cell_seed UNSIGNED "
                   "BIG INT, tile_latitude DOUBLE PRECISION, tile_logitude DOUBLE PRECISION, block_latitude DOUBLE "
                   "PRECISION, block_logitude DOUBLE PRECISION, cell_latitude DOUBLE PRECISION, cell_logitude DOUBLE "
                   "PRECISION, date_dimension_id_fk UNSIGNED BIG INT, errorCode INTEGER, library VARCHAR(256), module "
                   "VARCHAR(512), msg VARCHAR(2056))");

            for (const auto& sql : ddl) {
               tryExecute(session, [&sql](auto& session) { session << sql, now; });
            }

            SQLite::Connector::unregisterConnector();
         } catch (SQLite::DBLockedException&) {
            MOJA_LOG_DEBUG << ":DBLockedException - " << max_retries << " retries remaining";
            std::this_thread::sleep_for(SQLITE_RETRY_SLEEP);
            retry = max_retries-- > 0;
            if (!retry) {
               MOJA_LOG_DEBUG << "Exceeded MAX RETIRES (" << SQLITE_RETRY_ATTEMPTS << ")";
               throw;
            }
         } catch (const SQLite::SQLiteException& /*e*/) {
            MOJA_LOG_ERROR << "SQLite operations failed, filename: " << system_settings_.generated_db_name_sqlite;
            throw;
         } catch (const Poco::Exception& /*e*/) {
            MOJA_LOG_ERROR << "SQLite operations failed, filename: " << system_settings_.generated_db_name_sqlite;
            throw;
         }
      } while (retry);
   } catch (Poco::AssertionViolationException& exc) {
      MOJA_LOG_DEBUG << "AssertionViolationException - " << exc.displayText();
      throw;
   } catch (SQLite::InvalidSQLStatementException& exc) {
      MOJA_LOG_DEBUG << "InvalidSQLStatementException - " << exc.displayText();
      throw;
   } catch (SQLite::ConstraintViolationException& exc) {
      MOJA_LOG_DEBUG << "ConstraintViolationException: - " << exc.displayText();
      throw;
   } catch (...) {
      MOJA_LOG_ERROR << "SQLite operations failed, filename: " << system_settings_.generated_db_name_sqlite;
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

// --------------------------------------------------------------------------------------------

void LandUnitSQLiteWriter::onSystemShutdown() {
   try {
      std::vector<runStatDataRecord> run_stat_data;

      if (system_settings_.do_run_stats_on) {
         run_stat_data.emplace_back(0, 0, "LandUnitSQLiteWriter", "System_flag_dbNameSQLite",
                                    system_settings_.db_name_sqlite);
         run_stat_data.emplace_back(0, 0, "LandUnitSQLiteWriter", "System_flag_dbName",
                                    system_settings_.generated_db_name_sqlite);
         run_stat_data.emplace_back(0, 0, "LandUnitSQLiteWriter", "System_flag_tileIndexOn",
                                    system_settings_.tile_index_on ? "true" : "false");
         run_stat_data.emplace_back(0, 0, "LandUnitSQLiteWriter", "System_flag_blockIndexOn",
                                    system_settings_.block_index_on ? "true" : "false");
         run_stat_data.emplace_back(0, 0, "LandUnitSQLiteWriter", "System_flag_cellIndexOn",
                                    system_settings_.cell_index_on ? "true" : "false");
         run_stat_data.emplace_back(0, 0, "LandUnitSQLiteWriter", "System_flag_doRunStatsOn",
                                    system_settings_.do_run_stats_on ? "true" : "false");
         run_stat_data.emplace_back(0, 0, "LandUnitSQLiteWriter", "System_flag_outputMonth12Only",
                                    system_settings_.output_month12_only ? "true" : "false");
         run_stat_data.emplace_back(0, 0, "LandUnitSQLiteWriter", "System_flag_logErrors",
                                    system_settings_.log_errors ? "true" : "false");
         run_stat_data.emplace_back(0, 0, "LandUnitSQLiteWriter", "System_start",
                                    simulation_unit_data_->start_system_time.toString());
         run_stat_data.emplace_back(0, 0, "LandUnitSQLiteWriter", "System_finish", DateTime::now().toString());
      }

      auto retry = false;
      auto max_retries = SQLITE_RETRY_ATTEMPTS;
      do {
         try {
            retry = false;
            SQLite::Connector::registerConnector();
            Session session("SQLite", system_settings_.generated_db_name_sqlite);

            // -- Create Tables --
            std::vector<std::string> classifierStrings;

            std::vector<std::string> ddl{
                (boost::format("CREATE TABLE classifierset_dimension (classifierset_dimension_id_pk UNSIGNED BIG INT "
                               "PRIMARY KEY, number_classifiers INT, %1% VARCHAR)") %
                 boost::join(*classifier_names_, " VARCHAR, "))
                    .str()};
            for (const auto& sql : ddl) {
               tryExecute(session, [&sql](auto& session) { session << sql, now; });
            }

            // -- Write Dimensions --
            if (system_settings_.output_month12_only) {
               load(session, "date_dimension", date_annual_dimension_);
            } else {
               load(session, "date_dimension", date_dimension_);
            }

            load(session, "poolinfo_dimension", pool_info_dimension_);
            load(session, "tileinfo_dimension", tile_info_dimension_);
            load(session, "locationnontemp_dimension", location_non_temporal_dimension_);
            load(session, "locationtemp_dimension", location_temporal_dimension_);

            std::vector<std::string> placeholders;
            auto classifier_count = classifier_names_->size();
            for (auto i = 0; classifier_count > i; i++) {
               placeholders.emplace_back("?");
            }

            auto sql = (boost::format("INSERT INTO classifierset_dimension VALUES (?, ?, %1%)") %
                        boost::join(placeholders, ", "))
                           .str();

            tryExecute(session, [this, &sql, &classifier_count](auto& session) {
               for (auto classifier_set_row : this->classifier_set_dimension_->getPersistableCollection()) {
                  Statement insert(session);
                  insert << sql, bind(classifier_set_row.get<0>());
                  insert, bind(classifier_set_row.get<1>());
                  auto values = classifier_set_row.get<2>();
                  for (auto i = 0; i < classifier_count; i++) {
                     insert, bind(values[i]);
                  }
                  insert.execute();
               }
            });

            // -- Run Stats
            if (system_settings_.do_run_stats_on) {
               session.begin();
               session << "INSERT INTO run_information VALUES(?, ?, ?, ?, ?)", bind(run_stat_data), now;
               session.commit();
            }

            SQLite::Connector::unregisterConnector();
         } catch (SQLite::DBLockedException&) {
            MOJA_LOG_DEBUG << "DBLockedException - " << max_retries << " retries remaining";
            std::this_thread::sleep_for(SQLITE_RETRY_SLEEP);
            retry = max_retries-- > 0;
            if (!retry) {
               MOJA_LOG_DEBUG << "Exceeded MAX RETIRES (" << SQLITE_RETRY_ATTEMPTS << ")";
               throw;
            }
         }
      } while (retry);
   } catch (Poco::AssertionViolationException& exc) {
      MOJA_LOG_DEBUG << "AssertionViolationException - " << exc.displayText();
      throw;
   } catch (SQLite::InvalidSQLStatementException& exc) {
      MOJA_LOG_DEBUG << "InvalidSQLStatementException: - " << exc.displayText();
      throw;
   } catch (SQLite::ConstraintViolationException& exc) {
      MOJA_LOG_DEBUG << "ConstraintViolationException: - " << exc.displayText();
      throw;
   } catch (const SQLite::SQLiteException& /*e*/) {
      MOJA_LOG_ERROR << "SQLite operations failed, filename: " << system_settings_.generated_db_name_sqlite;
      throw;
   } catch (const Poco::Exception& /*e*/) {
      MOJA_LOG_ERROR << "SQLite operations failed, filename: " << system_settings_.generated_db_name_sqlite;
      throw;
   } catch (...) {
      MOJA_LOG_ERROR << "SQLite operations failed, filename: " << system_settings_.generated_db_name_sqlite;
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

// --------------------------------------------------------------------------------------------

void LandUnitSQLiteWriter::onLocalDomainInit() {
   simulation_unit_data_ = std::static_pointer_cast<SimulationUnitData>(
       _landUnitData->getVariable("simulationUnitData")->value().extract<std::shared_ptr<flint::IFlintData>>());
}

// --------------------------------------------------------------------------------------------

void LandUnitSQLiteWriter::onLocalDomainShutdown() {
   if (!system_settings_.block_index_on) {
      writeFlux();
      writeStock();
   }
   if (!system_settings_.log_error_per_block) {
      writeErrorLog();
   }
   writeRunStats("LocalDomain", simulation_unit_data_->start_local_domain_time,
                 simulation_unit_data_->end_local_domain_time, simulation_unit_data_->lu_count_local_domain);
   writeRunSummary("LocalDomain", simulation_unit_data_->start_local_domain_time,
                   simulation_unit_data_->end_local_domain_time, simulation_unit_data_->lu_count_local_domain);
}

// --------------------------------------------------------------------------------------------

void LandUnitSQLiteWriter::onLocalDomainProcessingUnitShutdown() {
   if (system_settings_.block_index_on) {
      writeFlux();
      writeStock();
   }
   if (system_settings_.log_error_per_block) {
      writeErrorLog();
   }
   writeRunStats("ProcessingUnit", simulation_unit_data_->start_processing_unit_time,
                 simulation_unit_data_->end_processing_unit_time, simulation_unit_data_->lu_count_processing_unit);
}

// --------------------------------------------------------------------------------------------

void LandUnitSQLiteWriter::writeStock() const {
   try {
      auto persistables = simulation_unit_data_->stock_results.getPersistableCollection();

      auto retry = false;
      auto max_retries = SQLITE_RETRY_ATTEMPTS;
      do {
         try {
            retry = false;
            SQLite::Connector::registerConnector();
            Session session("SQLite", system_settings_.generated_db_name_sqlite);

            // -- Stock Facts
            if (!persistables.empty()) {
               MOJA_LOG_DEBUG << "SQLite stock_reporting_results - inserted " << persistables.size() << " records";
               session.begin();
               session << "INSERT INTO stock_reporting_results VALUES(?, ?, ?, ?, ?, ?, ?, ?)", bind(persistables), now;
               session.commit();
            }
            SQLite::Connector::unregisterConnector();
         } catch (SQLite::DBLockedException&) {
            MOJA_LOG_DEBUG << "DBLockedException - " << max_retries << " retries remaining";
            std::this_thread::sleep_for(SQLITE_RETRY_SLEEP);
            retry = max_retries-- > 0;
            if (!retry) {
               MOJA_LOG_DEBUG << "Exceeded MAX RETIRES (" << SQLITE_RETRY_ATTEMPTS << ")";
               throw;
            }
         }
      } while (retry);
   } catch (Poco::AssertionViolationException& exc) {
      MOJA_LOG_DEBUG << "AssertionViolationException - " << exc.displayText();
      throw;
   } catch (SQLite::InvalidSQLStatementException& exc) {
      MOJA_LOG_DEBUG << "InvalidSQLStatementException: - " << exc.displayText();
      throw;
   } catch (SQLite::ConstraintViolationException& exc) {
      MOJA_LOG_DEBUG << "ConstraintViolationException: - " << exc.displayText();
      throw;
   } catch (...) {
      MOJA_LOG_ERROR << "SQLite operations failed, filename: " << system_settings_.generated_db_name_sqlite;
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

// --------------------------------------------------------------------------------------------

void LandUnitSQLiteWriter::writeFlux() const {
   try {
      auto persistables = simulation_unit_data_->flux_results.getPersistableCollection();

      auto retry = false;
      auto max_retries = SQLITE_RETRY_ATTEMPTS;
      do {
         try {
            retry = false;
            SQLite::Connector::registerConnector();
            Session session("SQLite", system_settings_.generated_db_name_sqlite);

            // -- Flux Facts
            if (!persistables.empty()) {
               MOJA_LOG_DEBUG << "SQLite flux_reporting_results - inserted " << persistables.size() << " records";
               session.begin();
               session << "INSERT INTO flux_reporting_results VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?)", bind(persistables),
                   now;
               session.commit();
            }
            SQLite::Connector::unregisterConnector();
         } catch (SQLite::DBLockedException&) {
            MOJA_LOG_DEBUG << "DBLockedException - " << max_retries << " retries remaining";
            std::this_thread::sleep_for(SQLITE_RETRY_SLEEP);
            retry = max_retries-- > 0;
            if (!retry) {
               MOJA_LOG_DEBUG << "Exceeded MAX RETIRES (" << SQLITE_RETRY_ATTEMPTS << ")";
               throw;
            }
         }
      } while (retry);
   } catch (Poco::AssertionViolationException& exc) {
      MOJA_LOG_DEBUG << "AssertionViolationException - " << exc.displayText();
      throw;
   } catch (SQLite::InvalidSQLStatementException& exc) {
      MOJA_LOG_DEBUG << "InvalidSQLStatementException: - " << exc.displayText();
      throw;
   } catch (SQLite::ConstraintViolationException& exc) {
      MOJA_LOG_DEBUG << "ConstraintViolationException: - " << exc.displayText();
      throw;
   } catch (...) {
      MOJA_LOG_ERROR << "SQLite operations failed, filename: " << system_settings_.generated_db_name_sqlite;
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

// --------------------------------------------------------------------------------------------

void LandUnitSQLiteWriter::writeErrorLog() const {
   try {
      // MOJA_LOG_DEBUG << "SQLIte start write (retries " << SQLITE_RETRY_ATTEMPTS << ")";

      auto retry = false;
      auto max_retries = SQLITE_RETRY_ATTEMPTS;
      do {
         try {
            retry = false;
            SQLite::Connector::registerConnector();
            Session session("SQLite", system_settings_.generated_db_name_sqlite);

            // -- Error Log
            if (simulation_unit_data_->error_log.size() != 0) {
               MOJA_LOG_DEBUG << "SQLite errors - inserted " << simulation_unit_data_->error_log.size() << " records";
               session.begin();
               session << "INSERT INTO error_log VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)",
                   bind(simulation_unit_data_->error_log.getPersistableCollection()), now;
               session.commit();
            }
            SQLite::Connector::unregisterConnector();
         } catch (SQLite::DBLockedException&) {
            MOJA_LOG_DEBUG << "DBLockedException - " << max_retries << " retries remaining";
            std::this_thread::sleep_for(SQLITE_RETRY_SLEEP);
            retry = max_retries-- > 0;
            if (!retry) {
               MOJA_LOG_DEBUG << "Exceeded MAX RETIRES (" << SQLITE_RETRY_ATTEMPTS << ")";
               throw;
            }
         }
      } while (retry);
      //_simulationUnitData->_errorLog.clear();				/// TODO: CHECK WHEN TO DO THE CLEAR
   } catch (Poco::AssertionViolationException& exc) {
      MOJA_LOG_DEBUG << "AssertionViolationException - " << exc.displayText();
      throw;  // TODO: throw a special exception here so we don't get in a loop
   } catch (SQLite::InvalidSQLStatementException& exc) {
      MOJA_LOG_DEBUG << "InvalidSQLStatementException: - " << exc.displayText();
      throw;  // TODO: throw a special exception here so we don't get in a loop
   } catch (SQLite::ConstraintViolationException& exc) {
      MOJA_LOG_DEBUG << "ConstraintViolationException: - " << exc.displayText();
      throw;  // TODO: throw a special exception here so we don't get in a loop
   } catch (...) {
      MOJA_LOG_ERROR << "SQLite operations failed, filename: " << system_settings_.generated_db_name_sqlite;
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

// --------------------------------------------------------------------------------------------

void LandUnitSQLiteWriter::writeRunStats(std::string unitLabel, DateTime& startTime, DateTime& finishTime,
                                         Int64 luCount) const {
   std::vector<runStatDataRecord> run_stat_data;

   if (system_settings_.do_run_stats_on) {
      auto span = finishTime - startTime;
      const auto unit_span_in_microseconds = span.microseconds();

      const auto start_label = (boost::format("%1%_start") % unitLabel).str();
      const auto finish_label = (boost::format("%1%_finish") % unitLabel).str();
      const auto lu_count_label = (boost::format("%1%_lu_count") % unitLabel).str();
      const auto lu_span_label = (boost::format("%1%_span_microseconds") % unitLabel).str();

      const auto start = (boost::format("%1%") % startTime).str();
      const auto finish = (boost::format("%1%") % finishTime).str();
      const auto lu_count = (boost::format("%1%") % luCount).str();
      const auto lu_span = (boost::format("%1%") % unit_span_in_microseconds).str();

      run_stat_data.emplace_back(0, simulation_unit_data_->process_unit_count, "LandUnitSQLiteWriter", start_label,
                                 start);
      run_stat_data.emplace_back(0, simulation_unit_data_->process_unit_count, "LandUnitSQLiteWriter", finish_label,
                                 finish);
      run_stat_data.emplace_back(0, simulation_unit_data_->process_unit_count, "LandUnitSQLiteWriter", lu_count_label,
                                 lu_count);
      run_stat_data.emplace_back(0, simulation_unit_data_->process_unit_count, "LandUnitSQLiteWriter", lu_span_label,
                                 lu_span);
   }

   try {
      // MOJA_LOG_DEBUG << "SQLIte start write (retries " << SQLITE_RETRY_ATTEMPTS << ")";

      auto retry = false;
      auto max_retries = SQLITE_RETRY_ATTEMPTS;
      do {
         try {
            retry = false;
            SQLite::Connector::registerConnector();
            Session session("SQLite", system_settings_.generated_db_name_sqlite);

            if (system_settings_.do_run_stats_on) {
               // -- Run Stats
               session.begin();
               session << "INSERT INTO run_information VALUES(?, ?, ?, ?, ?)", bind(run_stat_data), now;
               session.commit();
            }
            SQLite::Connector::unregisterConnector();
         } catch (SQLite::DBLockedException&) {
            MOJA_LOG_DEBUG << "DBLockedException - " << max_retries << " retries remaining";
            std::this_thread::sleep_for(SQLITE_RETRY_SLEEP);
            retry = max_retries-- > 0;
            if (!retry) {
               MOJA_LOG_DEBUG << "Exceeded MAX RETIRES (" << SQLITE_RETRY_ATTEMPTS << ")";
               throw;
            }
         }
      } while (retry);
   } catch (Poco::AssertionViolationException& exc) {
      MOJA_LOG_DEBUG << "AssertionViolationException - " << exc.displayText();
      throw;
   } catch (SQLite::InvalidSQLStatementException& exc) {
      MOJA_LOG_DEBUG << "InvalidSQLStatementException: - " << exc.displayText();
      throw;
   } catch (SQLite::ConstraintViolationException& exc) {
      MOJA_LOG_DEBUG << "ConstraintViolationException: - " << exc.displayText();
      throw;
   } catch (...) {
      MOJA_LOG_ERROR << "SQLite operations failed, filename: " << system_settings_.generated_db_name_sqlite;
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

// --------------------------------------------------------------------------------------------

void LandUnitSQLiteWriter::writeRunSummary(std::string unitLabel, DateTime& startTime, DateTime& finishTime,
                                           Int64 luCount) const {
   try {
      auto retry = false;
      auto max_retries = SQLITE_RETRY_ATTEMPTS;
      do {
         try {
            retry = false;
            SQLite::Connector::registerConnector();
            Session session("SQLite", system_settings_.generated_db_name_sqlite);

            if (system_settings_.do_run_stats_on) {
               session.begin();
               auto run_statistics = std::static_pointer_cast<RunStatistics>(
                   _landUnitData->getVariable("run_statistics")->value().extract<std::shared_ptr<flint::IFlintData>>());
               run_statistics->writeToSQLite(session, "run_information", 0, simulation_unit_data_->process_unit_count,
                                             "LandUnitSQLiteWriter");
               session.commit();
            }
            SQLite::Connector::unregisterConnector();
         } catch (SQLite::DBLockedException&) {
            MOJA_LOG_DEBUG << "DBLockedException - " << max_retries << " retries remaining";
            std::this_thread::sleep_for(SQLITE_RETRY_SLEEP);
            retry = max_retries-- > 0;
            if (!retry) {
               MOJA_LOG_DEBUG << "Exceeded MAX RETIRES (" << SQLITE_RETRY_ATTEMPTS << ")";
               throw;
            }
         }
      } while (retry);
   } catch (Poco::AssertionViolationException& exc) {
      MOJA_LOG_DEBUG << "AssertionViolationException - " << exc.displayText();
      throw;
   } catch (SQLite::InvalidSQLStatementException& exc) {
      MOJA_LOG_DEBUG << "InvalidSQLStatementException: - " << exc.displayText();
      throw;
   } catch (SQLite::ConstraintViolationException& exc) {
      MOJA_LOG_DEBUG << "ConstraintViolationException: - " << exc.displayText();
      throw;
   } catch (...) {
      MOJA_LOG_ERROR << "SQLite operations failed, filename: " << system_settings_.generated_db_name_sqlite;
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

// --------------------------------------------------------------------------------------------

template <typename TAccumulator>
void LandUnitSQLiteWriter::load(Session& session, const std::string& table,
                                std::shared_ptr<TAccumulator> dataDimension) {
   MOJA_LOG_INFO << (boost::format("Loading %1%") % table).str();
   tryExecute(session, [table, dataDimension](auto& session) {
      auto data = dataDimension->getPersistableCollection();
      if (!data.empty()) {
         std::vector<std::string> placeholders;
         for (auto i = 0; i < data[0].length; i++) {
            placeholders.push_back("?");
         }
         auto sql = (boost::format("INSERT INTO %1% VALUES (%2%)") % table % boost::join(placeholders, ", ")).str();
         session << sql, use(data), now;
      }
   });
}

// --------------------------------------------------------------------------------------------

void LandUnitSQLiteWriter::tryExecute(Session& session, std::function<void(Session&)> fn) {
   try {
      session.begin();
      fn(session);
      session.commit();
   } catch (Poco::AssertionViolationException& exc) {
      MOJA_LOG_FATAL << exc.displayText() << std::endl;
   } catch (SQLite::InvalidSQLStatementException& exc) {
      MOJA_LOG_FATAL << exc.displayText() << std::endl;
   } catch (SQLite::ConstraintViolationException& exc) {
      MOJA_LOG_FATAL << exc.displayText() << std::endl;
   } catch (BindingException& exc) {
      MOJA_LOG_FATAL << exc.displayText() << std::endl;
   } catch (const std::exception& e) {
      MOJA_LOG_FATAL << e.what() << std::endl;
   } catch (...) {
      MOJA_LOG_FATAL << "Unknown exception." << std::endl;
   }
}

}  // namespace chapman_richards
}  // namespace modules
}  // namespace moja
