#pragma once

#include <moja/flint/itransform.h>

#include <moja/dynamic.h>

namespace moja::flint {
class IVariable;
namespace example::base {

class TimeSeriesTransform : public ITransform {
  public:
   enum Format { Wide, Long };

   void configure(DynamicObject config, const ILandUnitController& landUnitController,
                  moja::datarepository::DataRepository& dataRepository) override;

   void controllerChanged(const ILandUnitController& controller) override;
   const DynamicVar& value() const override;

  private:
   const ILandUnitController* _landUnitController = nullptr;
   datarepository::DataRepository* _dataRepository = nullptr;

   std::vector<double> _values;
   mutable DynamicVar _currentValue;
   mutable std::string _dataPropertyName;
};

}  // namespace example::base
}  // namespace moja::flint
