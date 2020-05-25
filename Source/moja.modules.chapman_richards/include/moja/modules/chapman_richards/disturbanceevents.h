#pragma once

#include "moja/modules/chapman_richards/_modules.chapman_richards_exports.h"

#include <moja/flint/eventqueue.h>
#include <moja/flint/iflintdata.h>

#include <moja/dynamic.h>

namespace moja {
namespace modules {
namespace chapman_richards {

class ForestPlantEvent;
class ForestClearEvent;

class CHAPMAN_RICHARDS_API DisturbanceEventHandler {
  public:
   virtual ~DisturbanceEventHandler() {}

   virtual void simulate(const ForestPlantEvent& fire) {}
   virtual void simulate(const ForestClearEvent& fire) {}
};

class CHAPMAN_RICHARDS_API DisturbanceEventBase : public flint::EventBase {
  public:
   virtual ~DisturbanceEventBase() = default;
   virtual void simulate(DisturbanceEventHandler&) const = 0;
   virtual bool is_clearing() const = 0;

   void configure(DynamicObject config, const flint::ILandUnitController& landUnitController,
                  moja::datarepository::DataRepository& dataRepository) override {
      EventBase::configure(config, landUnitController, dataRepository);
   }

   DynamicVar getProperty(const std::string& key) const override { return DynamicVar(); }

   DynamicObject exportObject() const override { return DynamicObject(); }

  protected:
   DisturbanceEventBase(int id, const std::string& type, const std::string& name) : EventBase(id, type, name) {}
};

class CHAPMAN_RICHARDS_API DisturbanceEventsList {
  public:
   typedef std::vector<std::shared_ptr<DisturbanceEventBase>>::value_type value_type;
   typedef std::vector<std::shared_ptr<DisturbanceEventBase>>::iterator iterator;
   typedef std::vector<std::shared_ptr<DisturbanceEventBase>>::const_iterator const_iterator;
   typedef std::vector<std::shared_ptr<DisturbanceEventBase>>::size_type size_type;

   DisturbanceEventsList() = default;
   virtual ~DisturbanceEventsList() = default;
   DisturbanceEventsList(const DisturbanceEventsList&) = delete;
   DisturbanceEventsList& operator=(const DisturbanceEventsList&) = delete;

   iterator begin();
   const_iterator begin() const MOJA_NOEXCEPT;
   iterator end() MOJA_NOEXCEPT;
   const_iterator end() const MOJA_NOEXCEPT;
   void clear();
   size_type size() const MOJA_NOEXCEPT;
   template <class... Args>
   void emplace_back(Args&&... args);

   value_type find(int event_id) const;

   void push_back(const value_type& event);
   void push_back(value_type&& event);

  private:
   std::vector<value_type> _list;
};

inline DisturbanceEventsList::iterator DisturbanceEventsList::begin() { return _list.begin(); }

inline DisturbanceEventsList::const_iterator DisturbanceEventsList::begin() const MOJA_NOEXCEPT {
   return _list.begin();
}

inline DisturbanceEventsList::iterator DisturbanceEventsList::end() MOJA_NOEXCEPT { return _list.end(); }

inline DisturbanceEventsList::const_iterator DisturbanceEventsList::end() const MOJA_NOEXCEPT { return _list.end(); }

inline void DisturbanceEventsList::clear() { _list.clear(); }

inline DisturbanceEventsList::size_type DisturbanceEventsList::size() const MOJA_NOEXCEPT { return _list.size(); }

inline void DisturbanceEventsList::push_back(const value_type& event) { _list.push_back(event); }
inline void DisturbanceEventsList::push_back(value_type&& event) { _list.push_back(event); }

inline DisturbanceEventsList::value_type DisturbanceEventsList::find(int event_id) const {
   const auto event = std::find_if(begin(), end(), [&event_id](const std::shared_ptr<const DisturbanceEventBase> item) {
      return item->id == event_id;
   });
   return event == end() ? nullptr : *event;
}

template <class... Args>
void DisturbanceEventsList::emplace_back(Args&&... args) {
   _list.emplace_back(std::forward<Args>(args)...);
}

}  // namespace chapman_richards
}  // namespace modules
}  // namespace moja
