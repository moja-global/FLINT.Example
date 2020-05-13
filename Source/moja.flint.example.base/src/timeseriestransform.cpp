#include "moja/flint/example/base/timeseriestransform.h"

#include "moja/flint/flintexceptions.h"
#include "moja/flint/ilandunitcontroller.h"
#include "moja/flint/ivariable.h"
#include <moja/flint/itiming.h>

#include <moja/logging.h>

#include <boost/algorithm/string.hpp>

using moja::flint::IncompleteConfigurationException;
using moja::flint::VariableNotFoundException;

namespace moja {
namespace flint {
namespace example {
namespace base {

void TimeSeriesTransform::configure(DynamicObject config, const flint::ILandUnitController& landUnitController,
                                    moja::datarepository::DataRepository& dataRepository) {
   _landUnitController = &landUnitController;
   _dataRepository = &dataRepository;

   _dataPropertyName = "data";
   if (config.contains("data_property")) {
      _dataPropertyName = config["data_property"].extract<std::string>();
   }

   if (!config.contains(_dataPropertyName)) {
      BOOST_THROW_EXCEPTION(IncompleteConfigurationException() << flint::Item(_dataPropertyName));
   }

   auto timeseries = config[_dataPropertyName];

   if (timeseries.isVector()) {
      auto data = timeseries.extract<std::vector<DynamicVar>>();
      if (!data.empty()) {
         for (double value : data) {
            _values.emplace_back(value);
         }
      }
   }
}

void TimeSeriesTransform::controllerChanged(const flint::ILandUnitController& controller) {
   // opportunity to change cache if there is any kept
   //_cachedValue = nullptr;
   _landUnitController = &controller;
};

// Step 0 will be the Init step, so I've added a blank value to the start of each dataset
const DynamicVar& TimeSeriesTransform::value() const {
   // simply look into the data array and get the value out for the current step.
   const auto timing = &_landUnitController->timing();
   int curStep = timing->step();

   // If past the end of values given for timeseries, use last valid value
   // Brutal, but effective here
   if (curStep > (_values.size() - 1)) 
	   curStep = _values.size() - 1;

   _currentValue = _values[curStep];
   return _currentValue;
}

}  // namespace base
}  // namespace example
}  // namespace flint
}  // namespace moja
