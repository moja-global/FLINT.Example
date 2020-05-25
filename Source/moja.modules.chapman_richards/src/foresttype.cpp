#include "moja/modules/chapman_richards/foresttype.h"

#include <moja/exception.h>
#include <moja/logging.h>

namespace moja {
namespace modules {
namespace chapman_richards {

void ForestType::configure(DynamicObject config, const flint::ILandUnitController& landUnitController,
                           datarepository::DataRepository& dataRepository) {
   if (config.empty()) return;
   id = config["id"];
   name = config["name"].extract<const std::string>();
   type = config["type"].extract<const std::string>();

   carbon_frac_ag = config["carbon_frac_ag"];
   carbon_frac_bg = config["carbon_frac_bg"];

   max = config["max"];
   k = config["k"];
   m = config["m"];

   root_to_shoot = config["root_to_shoot"];
}

DynamicObject ForestType::exportObject() const {
   DynamicObject object;

   object["id"] = id;
   object["name"] = name;
   object["type"] = type;

   object["carbon_frac_ag"] = carbon_frac_ag;
   object["carbon_frac_bg"] = carbon_frac_bg;

   object["max"] = max;
   object["k"] = k;
   object["m"] = m;

   object["root_to_shoot"] = root_to_shoot;
   return object;
}

void ForestType::setProperty(const DynamicObject& query, const std::string& key, DynamicVar value) {
   if (key == "k") {
      k = value;
   } else if (key == "m") {
      m = value;
   } else if (key == "max") {
      max = value;
   } else if (key == "root_to_shoot") {
      root_to_shoot = value;
   } else if (key == "carbon_frac_ag") {
      carbon_frac_ag = value;
   } else if (key == "carbon_frac_bg") {
      carbon_frac_bg = value;
   }
}

void ForestTypeList::configure(DynamicObject config, const flint::ILandUnitController& landUnitController,
                               datarepository::DataRepository& dataRepository) {
   list_.clear();
   if (!config.contains("items")) return;

   if (config["items"].type() == typeid(std::vector<DynamicObject>)) {
      auto& species_list = config["items"].extract<const std::vector<DynamicObject>>();
      for (const auto& spec : species_list) {
         add_species(spec, landUnitController, dataRepository);
      }
   } else {
      const auto& species_list = config["items"].extract<const DynamicVector>();
      for (const auto& spec_dyn : species_list) {
         const auto& spec = spec_dyn.extract<const DynamicObject>();
         add_species(spec, landUnitController, dataRepository);
      }
   }
}

void ForestTypeList::add_species(const DynamicObject& spec, const flint::ILandUnitController& landUnitController,
                                 datarepository::DataRepository& dataRepository) {
   std::shared_ptr<ForestType> forest_type = nullptr;
   const std::string type = spec["type"];
   if (type == "chapman_richards.ForestType") {
      forest_type = std::make_shared<ForestType>();
   } else {
      throw ApplicationException("Unknown Forest type");
   }
   forest_type->configure(spec, landUnitController, dataRepository);
   list_.push_back(forest_type);
}

DynamicObject ForestTypeList::exportObject() const {
   DynamicObject object;
   return object;
}

void ForestTypeList::setProperty(const DynamicObject& query, const std::string& key, DynamicVar value) {
   for (auto& forest_type : list_) {
      if (!query.empty()) {
         for (const auto& pair : query) {
            if (pair.first == "id") {
               if (forest_type->id == pair.second) {
                  forest_type->setProperty({}, key, value);
               }
            }
         }
      }
   }
}

ForestTypeList::value_type ForestTypeList::find(int id) const {
   const auto tree_species =
       std::find_if(begin(), end(), [&id](std::shared_ptr<ForestType> item) { return item->id == id; });
   return tree_species == end() ? nullptr : *tree_species;
}

}  // namespace chapman_richards
}  // namespace modules
}  // namespace moja
