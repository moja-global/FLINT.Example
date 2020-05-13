#ifndef MOJA_FLINT_EXAMPLE_BASE_COMPOSITETRANSFORM_H_
#define MOJA_FLINT_EXAMPLE_BASE_COMPOSITETRANSFORM_H_

#include "moja/flint/example/base/_modules.base_exports.h"

#include <moja/flint/itransform.h>
#include <moja/dynamic.h>

namespace moja {
    namespace flint {
        class IVariable;
    }
}

namespace moja {
namespace flint {
namespace example {
namespace base {

class TimeSeriesTransform : public flint::ITransform {
  public:
   enum Format { Wide, Long };

   void configure(DynamicObject config, const flint::ILandUnitController& landUnitController,
                  moja::datarepository::DataRepository& dataRepository) override;

   void controllerChanged(const flint::ILandUnitController& controller) override;
   const DynamicVar& value() const override;

  private:
   const flint::ILandUnitController* _landUnitController;
   datarepository::DataRepository* _dataRepository;

   std::vector<double> _values;
   mutable DynamicVar _currentValue;
   mutable std::string _dataPropertyName;
};

}  // namespace base
}  // namespace example
}  // namespace flint
}  // namespace moja

#endif // MOJA_FLINT_EXAMPLE_BASE_COMPOSITETRANSFORM_H_
