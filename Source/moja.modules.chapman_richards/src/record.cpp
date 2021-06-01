#include "moja/modules/chapman_richards/record.h"

#include <moja/hash.h>

namespace moja::modules::chapman_richards {

// -- DateRecord
DateRecord::DateRecord(int step, int substep, int year, int month, int day, double fracOfStep, double yearsInStep)
    : _id(-1),
      _step(step),
      _substep(substep),
      _year(year),
      _month(month),
      _day(day),
      _fracOfStep(fracOfStep),
      _yearsInStep(yearsInStep) {}

bool DateRecord::operator==(const DateRecord& other) const {
   return _step == other._step && _substep == other._substep;
}

size_t DateRecord::hash() const {
   if (_hash == -1) _hash = moja::hash::hash_combine(_step, _substep);
   return _hash;
}

DateRow DateRecord::asPersistable() const {
   return DateRow{_id, _step, _substep, _year, _month, _day, _fracOfStep, _yearsInStep};
}

// -- DateAnnualRecord
DateAnnualRecord::DateAnnualRecord(int year) : _id(-1), _year(year) {}

bool DateAnnualRecord::operator==(const DateAnnualRecord& other) const { return _year == other._year; }

size_t DateAnnualRecord::hash() const {
   if (_hash == -1) _hash = hash::hash_combine(_year);
   return _hash;
}

DateAnnualRow DateAnnualRecord::asPersistable() const { return DateAnnualRow{_id, _year}; }

// -- LocationNonTemporalRecord
LocationNonTemporalRecord::LocationNonTemporalRecord(Int64 tileInfoId, Int64 classifierSetId, Int64 simUnitCount,
                                                     double simUnitAreaSum)
    : _id(-1),
      _tileInfoId(tileInfoId),
      _classifierSetId(classifierSetId),
      _simUnitCount(simUnitCount),
      _simUnitAreaSum(simUnitAreaSum) {}

bool LocationNonTemporalRecord::operator==(const LocationNonTemporalRecord& other) const {
   return _tileInfoId == other._tileInfoId && _classifierSetId == other._classifierSetId;
}

size_t LocationNonTemporalRecord::hash() const {
   if (_hash == -1) _hash = hash::hash_combine(_tileInfoId, _classifierSetId);
   return _hash;
}

LocationNonTemporalRow LocationNonTemporalRecord::asPersistable() const {
   return LocationNonTemporalRow{_id, _tileInfoId, _classifierSetId, _simUnitCount, _simUnitAreaSum};
}

void LocationNonTemporalRecord::merge(const LocationNonTemporalRecord& other) {
   _simUnitCount += other._simUnitCount;
   _simUnitAreaSum += other._simUnitAreaSum;
}

// -- LocationTemporalRecord
LocationTemporalRecord::LocationTemporalRecord(Int64 dateId, Int64 classifierSetId, Int64 simUnitCount,
                                               double simUnitAreaSum)
    : _id(-1),
      _dateId(dateId),
      _classifierSetId(classifierSetId),
      _simUnitCount(simUnitCount),
      _simUnitAreaSum(simUnitAreaSum) {}

bool LocationTemporalRecord::operator==(const LocationTemporalRecord& other) const {
   return _dateId == other._dateId && _classifierSetId == other._classifierSetId;
}

size_t LocationTemporalRecord::hash() const {
   if (_hash == -1) _hash = hash::hash_combine(_dateId, _classifierSetId);
   return _hash;
}

LocationTemporalRow LocationTemporalRecord::asPersistable() const {
   return LocationTemporalRow{_id, _dateId, _classifierSetId, _simUnitCount, _simUnitAreaSum};
}

void LocationTemporalRecord::merge(const LocationTemporalRecord& other) {
   _simUnitCount += other._simUnitCount;
   _simUnitAreaSum += other._simUnitAreaSum;
}

#pragma region ClassifierSetRecord

ClassifierSetRecord::ClassifierSetRecord(std::vector<Poco::Nullable<std::string>> classifier_values)
    : _id(0), _numberClassifiers(int(classifier_values.size())), _classifierValues(classifier_values) {}

ClassifierSetRecord::ClassifierSetRecord(int numberClassifiers,
                                         std::vector<Poco::Nullable<std::string>> classifierValues)
    : _id(0), _numberClassifiers(numberClassifiers), _classifierValues(classifierValues) {}

bool ClassifierSetRecord::operator==(const ClassifierSetRecord& other) const {
   for (int i = 0; i < other._classifierValues.size(); i++) {
      if (_classifierValues[i] != other._classifierValues[i]) {
         return false;
      }
   }
   return true;
}

size_t ClassifierSetRecord::hash() const {
   if (_hash == -1) {
      _hash = moja::hash::hash_range(_classifierValues.begin(), _classifierValues.end(), 0, moja::Hash());
   }
   return _hash;
}

ClassifierSetRow ClassifierSetRecord::asPersistable() const {
   return ClassifierSetRow{_id, _numberClassifiers, _classifierValues};
}

#pragma endregion

PoolInfoRecord::PoolInfoRecord(const std::string& name, const std::string& desc, int idx, int order, double scale,
                               const std::string& units)
    : _id(-1), _name(name), _desc(desc), _idx(idx), _order(order), _scale(scale), _units(units) {}

PoolInfoRecord::PoolInfoRecord(const std::string& name)
    : _id(-1), _name(name), _desc(""), _idx(-1), _order(-1), _scale(1.0), _units("") {}

bool PoolInfoRecord::operator==(const PoolInfoRecord& other) const { return _name == other._name; }

size_t PoolInfoRecord::hash() const {
   if (_hash == -1) _hash = hash::hash_combine(_name);
   return _hash;
}

PoolInfoRow PoolInfoRecord::asPersistable() const {
   return PoolInfoRow{_id, _name, _desc, _idx, _order, _scale, _units};
}

void PoolInfoRecord::merge(const PoolInfoRecord& other) {}

// -- TileInfoRecord
bool TileInfoRecord::operator==(const TileInfoRecord& other) const {
   return _tileIdx == other._tileIdx && _blockIdx == other._blockIdx && _cellIdx == other._cellIdx;
}
size_t TileInfoRecord::hash() const {
   if (_hash == -1) _hash = moja::hash::hash_combine(_tileIdx, _blockIdx, _cellIdx);
   return _hash;
}

TileInfoRow TileInfoRecord::asPersistable() const {
   return TileInfoRow{
       _id,       _tileIdx, _blockIdx, _cellIdx,          _tileLat,        _tileLon,         _blockLat,
       _blockLon, _cellLat, _cellLon,  _globalRandomSeed, _tileRandomSeed, _blockRandomSeed, _cellRandomSeed};
}

void TileInfoRecord::merge(const TileInfoRecord& other) {}

}  // namespace moja::modules::chapman_richards
