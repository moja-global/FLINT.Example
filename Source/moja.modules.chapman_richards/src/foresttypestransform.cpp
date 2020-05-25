#include "moja/modules/chapman_richards/foresttypestransform.h"

#include "moja/modules/chapman_richards/forestmanagement.h"

#include <moja/flint/flintexceptions.h>
#include <moja/flint/iflintdata.h>
#include <moja/flint/ivariable.h>

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

namespace moja {
namespace modules {
namespace chapman_richards {

template <class T>
void BuildListFromVariable(ForestTypeList& arr, const flint::IVariable* var) {
   const auto& result = var->value();

   try {
      auto arr_dyn = result.extract<std::vector<DynamicObject>>();
      for (auto& item : arr_dyn) {
         arr.emplace_back(T{item});
      }
   } catch (...) {
      const auto str =
          ((boost::format("Forest type variable returned incorrect dataset (%1%) ") % var->info().name).str());
      BOOST_THROW_EXCEPTION(flint::SimulationError()
                            << flint::Details(str) << flint::LibraryName("ForestTypesTransform")
                            << flint::ModuleName("value") << flint::ErrorCode(3));
   }
}

ForestTypesTransform::ForestTypesTransform()
    : _landUnitController(nullptr), _dataRepository(nullptr), _dataLoaded(false) {}

void ForestTypesTransform::configure(DynamicObject config, const flint::ILandUnitController& landUnitController,
                                     datarepository::DataRepository& dataRepository) {
   _landUnitController = &landUnitController;
   _dataRepository = &dataRepository;
}

void ForestTypesTransform::controllerChanged(const flint::ILandUnitController& controller) {}

const DynamicVar& ForestTypesTransform::value() const {
   if (!_dataLoaded) {
      // Data to be cached
      const auto var = _landUnitController->getVariable("forest_type_records");
      DynamicObject config;
      auto forest_types = std::make_shared<ForestTypeList>();
      config["items"] = var->value().extract<std::vector<DynamicObject>>();
      forest_types->configure(config, *_landUnitController, *_dataRepository);
      _cached_value = std::static_pointer_cast<flint::IFlintData>(forest_types);
      _dataLoaded = true;
   }
   return _cached_value;
}
}  // namespace chapman_richards
}  // namespace modules
}  // namespace moja
