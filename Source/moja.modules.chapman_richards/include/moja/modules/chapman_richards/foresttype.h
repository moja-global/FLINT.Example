#pragma once

#include "moja/modules/chapman_richards/_modules.chapman_richards_exports.h"

#include <moja/flint/iflintdata.h>

#include <moja/dynamic.h>

namespace moja::modules::chapman_richards {

class CHAPMAN_RICHARDS_API ForestType : public flint::IFlintData {
  public:
   ForestType() = default;
   virtual ~ForestType() = default;
   ForestType(const ForestType&) = delete;
   ForestType& operator=(const ForestType&) = delete;

   void configure(DynamicObject config, const flint::ILandUnitController& landUnitController,
                  datarepository::DataRepository& dataRepository) override;

   DynamicObject exportObject() const override;
   void setProperty(const DynamicObject& query, const std::string& key, DynamicVar value); // override;

   int id;
   std::string name;
   std::string type;
   double carbon_frac_ag, carbon_frac_bg;
   double turnover_frac_ag, turnover_frac_bg;
   double max, k, m;
   double root_to_shoot;
};

class CHAPMAN_RICHARDS_API ForestTypeList : public flint::IFlintData {
  public:
   typedef std::vector<std::shared_ptr<ForestType>>::value_type value_type;
   typedef std::vector<std::shared_ptr<ForestType>>::iterator iterator;
   typedef std::vector<std::shared_ptr<ForestType>>::const_iterator const_iterator;
   typedef std::vector<std::shared_ptr<ForestType>>::size_type size_type;

   ForestTypeList() = default;
   virtual ~ForestTypeList() = default;
   ForestTypeList(const ForestTypeList&) = delete;
   ForestTypeList& operator=(const ForestTypeList&) = delete;

   void configure(DynamicObject config, const flint::ILandUnitController& landUnitController,
                  datarepository::DataRepository& dataRepository) override;

   DynamicObject exportObject() const override;
   void setProperty(const DynamicObject& query, const std::string& key, DynamicVar value); // override;

   iterator begin();
   const_iterator begin() const MOJA_NOEXCEPT;
   iterator end() MOJA_NOEXCEPT;
   const_iterator end() const MOJA_NOEXCEPT;
   void clear();
   size_type size() const MOJA_NOEXCEPT;
   template <class... Args>
   void emplace_back(Args&&... args);

   value_type find(int species_id) const;

  private:
   void add_species(const DynamicObject& spec, const flint::ILandUnitController& landUnitController,
                    datarepository::DataRepository& dataRepository);
   std::vector<value_type> list_;
};

inline ForestTypeList::iterator ForestTypeList::begin() { return list_.begin(); }

inline ForestTypeList::const_iterator ForestTypeList::begin() const MOJA_NOEXCEPT { return list_.begin(); }

inline ForestTypeList::iterator ForestTypeList::end() MOJA_NOEXCEPT { return list_.end(); }

inline ForestTypeList::const_iterator ForestTypeList::end() const MOJA_NOEXCEPT { return list_.end(); }

inline void ForestTypeList::clear() { list_.clear(); }

inline ForestTypeList::size_type ForestTypeList::size() const MOJA_NOEXCEPT { return list_.size(); }

template <class... Args>
void ForestTypeList::emplace_back(Args&&... args) {
   list_.emplace_back(std::forward<Args>(args)...);
}
}  // namespace chapman_richards
