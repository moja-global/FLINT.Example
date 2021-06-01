#pragma once

#include "moja/modules/chapman_richards/_modules.chapman_richards_exports.h"

namespace moja::modules::chapman_richards {

// --------------------------------------------------------------------------------------------
// This object will be created in the LibraryFactory and shared across all Modules/Threads that
// want to use it. References should passed into constructors in LibraryFactory
// Each thread will be using the same object - good for system settings/configurations

class CHAPMAN_RICHARDS_API SystemSettings {
  public:
   SystemSettings()
       : tile_index_on(false),
         block_index_on(false),
         cell_index_on(false),
         do_run_stats_on(false),
         output_month12_only(true),
         do_stock(false),
         log_errors(true),
         log_error_max(0),
         log_error_per_block(true) {}

   virtual ~SystemSettings() = default;
   SystemSettings(const SystemSettings& src) = delete;
   SystemSettings& operator=(const SystemSettings&) = delete;

   // Common config
   std::string db_name;
   std::string db_name_sqlite;
   std::string generated_db_name_sqlite;

   // Run flags for Modules
   bool tile_index_on;
   bool block_index_on;
   bool cell_index_on;
   bool do_run_stats_on;
   bool output_month12_only;

   bool do_stock;

   bool log_errors;
   int log_error_max;
   bool log_error_per_block;
};

}  // namespace moja::modules::chapman_richards
