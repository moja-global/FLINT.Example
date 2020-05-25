#include "moja/modules/chapman_richards/landcovertransition.h"

#include <moja/datetime.h>

#include <fmt/format.h>

namespace moja {
namespace modules {
namespace chapman_richards {

namespace constants {
const std::string no_data_str = "no_data";
const std::string forest_str = "forest";
const std::string non_forest_str = "non_forest";
}  // namespace constants

const std::string& landcover_type_str(LandcoverTransition::landcover_type landcover_type) {
   switch (landcover_type) {
      case LandcoverTransition::landcover_type::forest:
         return constants::forest_str;
      case LandcoverTransition::landcover_type::non_forest:
         return constants::non_forest_str;
      default:
         return constants::no_data_str;
   }
}

LandcoverTransition::landcover_type landcover_type_enum(const std::string& landcover_type_str) {
   if (landcover_type_str == constants::forest_str) return LandcoverTransition::landcover_type::forest;
   if (landcover_type_str == constants::non_forest_str) return LandcoverTransition::landcover_type::non_forest;
   return LandcoverTransition::landcover_type::nodata;
}

void LandcoverTransition::configure(DynamicObject config, const flint::ILandUnitController& landUnitController,
                                    datarepository::DataRepository& dataRepository) {
   if (config.empty()) return;
   date = config["date"].extract<const DateTime>();
   landcover = landcover_type_enum(config["landcover"].extract<const std::string>());
}

DynamicObject LandcoverTransition::exportObject() const {
   DynamicObject object;
   std::string date_str = fmt::format("{}/{}/{}", date.year(), date.month(), date.day());
   DynamicObject date_obj({{"$date", date_str}});
   object["date"] = date_obj;
   object["landcover"] = landcover_type_str(landcover);
   return object;
}

}  // namespace chapman_richards
}  // namespace modules
}  // namespace moja