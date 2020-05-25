#include <moja/flint/configuration/configuration.h>
#include <moja/flint/configuration/library.h>
#include <moja/flint/configuration/localdomain.h>

#include <moja/flint/localdomaincontrollerbase.h>

#include <moja/datetime.h>

#include <boost/test/unit_test.hpp>

#include <random>

BOOST_AUTO_TEST_SUITE(CRModuleTests);

BOOST_AUTO_TEST_CASE(modules_chapman_richards_ForestType_Growth) {
   namespace flint = moja::flint;
   using namespace flint::configuration;

   Configuration config(moja::DateTime(2000, 1, 1), moja::DateTime(2100, 12, 31));

   config.addLibrary("moja.modules.chapman_richards", LibraryType::External);

   config.setLocalDomain(LocalDomainType::Point, LocalDomainIterationType::NotAnIteration, true, 1, "internal.flint",
                         "CalendarAndEventFlintDataSequencer", "simulateLandUnit", "landUnitBuildSuccess",
                         moja::DynamicObject());

   config.addModule("internal.flint", "CalendarAndEventFlintDataSequencer", 1, false);
   config.addModule("internal.flint", "TransactionManagerEndOfStepModule", 2, false);
   config.addModule("moja.modules.chapman_richards", "ForestGrowthModule", 3, false);
   config.addModule("moja.modules.chapman_richards", "DisturbanceEventModule", 5, false);
   config.addModule("internal.flint", "AggregatorStockStep", 6, false);
   config.addModule(
       "internal.flint", "OutputerStream", 7, false,
       moja::DynamicObject({{"output_filename", std::string("Growth_Stock.txt")}, {"output_to_screen", false}}));
   config.addModule(
       "internal.flint", "OutputerStreamFlux", 8, false,
       moja::DynamicObject({{"output_filename", std::string("Growth_Fluxes.txt")}, {"output_to_screen", false}}));

   int poolOrder = 0;

   for (auto pool : {"initialValues",
                     "atmosphereCM",
                     "aboveGroundCM",
                     "belowGroundCM"
                     }) {
      std::string desc(pool);
      desc += " description";
      std::string unit("units for ");
      unit += pool;
      config.addPool(pool, desc, unit, 1.0, poolOrder++);
   }

   config.addVariable("simulateLandUnit", true);
   config.addVariable("landUnitBuildSuccess", true);

   config.addVariable("forest_exists", true);
   config.addVariable("forest_age", 0.0);

   config.addFlintDataVariable("forest_type", "moja.modules.chapman_richards", "ForestType",
                               moja::DynamicObject({{"name", "Dryland Forest"},
                                                    {"carbonFracAG", 0.49},
                                                    {"carbonFracBG", 0.48},
                                                    {"max", 52},
                                                    {"k", 0.037},
                                                    {"m", 0.5},
                                                    {"rootToShoot", 0.24}}));
   config.addFlintDataVariable(
       "eventqueue", "internal.flint", "EventQueue",
       moja::DynamicObject(
           {{"events", moja::DynamicVector({moja::DynamicObject({{"type", "chapman_richards.ClearEvent"},
                                                                 {"id", 1},
                                                                 {"date", moja::DateTime(2050, 1, 1)},
                                                                 {"name", "Clear Forest"}
                                                                })})}}));

   flint::LocalDomainControllerBase ldc;

   ldc.configure(config);
   ldc._notificationCenter.postNotification(moja::signals::SystemInit);
   ldc.startup();
   ldc.run();
   ldc.shutdown();
   ldc._notificationCenter.postNotification(moja::signals::SystemShutdown);
}

BOOST_AUTO_TEST_SUITE_END();