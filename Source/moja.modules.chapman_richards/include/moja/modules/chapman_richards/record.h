#pragma once

#include "moja/modules/chapman_richards/_modules.chapman_richards_exports.h"

#include <moja/flint/record.h>

#include <moja/hash.h>

#include <Poco/Nullable.h>
#include <Poco/Tuple.h>

#include <vector>

namespace moja {
namespace modules {
namespace chapman_richards {

// --------------------------------------------------------------------------------------------
// id, step, substep, year, month, day, frac of step, years in step
typedef Poco::Tuple<Int64, int, int, int, int, int, double, double> DateRow;
class DateRecord {
  public:
   DateRecord(int step, int substep, int year, int month, int day, double fracOfStep, double yearsInStep);
   ~DateRecord() {}

   bool operator==(const DateRecord& other) const;
   size_t hash() const;
   DateRow asPersistable() const;
   void merge(const DateRecord& other) {}
   void setId(Int64 id) { _id = id; }
   Int64 getId() const { return _id; }

  private:
   mutable size_t _hash = -1;
   // Data
   Int64 _id;
   int _step;
   int _substep;
   int _year;
   int _month;
   int _day;
   double _fracOfStep;
   double _yearsInStep;
};

// --------------------------------------------------------------------------------------------
// id, year
typedef Poco::Tuple<Int64, int> DateAnnualRow;
class DateAnnualRecord {
  public:
   DateAnnualRecord(int year);
   ~DateAnnualRecord() {}

   bool operator==(const DateAnnualRecord& other) const;
   size_t hash() const;
   DateAnnualRow asPersistable() const;
   void merge(const DateAnnualRecord& other) {}
   void setId(Int64 id) { _id = id; }
   Int64 getId() const { return _id; }

  private:
   mutable size_t _hash = -1;

   // Data
   Int64 _id;
   int _year;
};

// --------------------------------------------------------------------------------------------
// id, tileInfor Id, classifierSetId, sim unit count sum, sim unit area sum
typedef Poco::Tuple<Int64, Int64, Int64, Int64, double> LocationNonTemporalRow;
class LocationNonTemporalRecord {
  public:
   LocationNonTemporalRecord(Int64 tileInfoId, Int64 classifierSetId, Int64 simUnitCount, double simUnitAreaSum);
   ~LocationNonTemporalRecord() {}

   bool operator==(const LocationNonTemporalRecord& other) const;
   size_t hash() const;
   LocationNonTemporalRow asPersistable() const;
   void merge(const LocationNonTemporalRecord& other);
   void setId(Int64 id) { _id = id; }
   Int64 getId() const { return _id; }

  private:
   mutable size_t _hash = -1;
   Int64 _id;

   // Data
   Int64 _tileInfoId;       // 1
   Int64 _classifierSetId;  // 2
   Int64 _simUnitCount;     // 3
   double _simUnitAreaSum;  // 4
};

// --------------------------------------------------------------------------------------------
// id, dateid, classifierSetId, sim unit count sum, sim unit area sum
typedef Poco::Tuple<Int64, Int64, Int64, Int64, double> LocationTemporalRow;
class LocationTemporalRecord {
  public:
   LocationTemporalRecord(Int64 dateId, Int64 classifierSetId, Int64 simUnitCount, double simUnitAreaSum);
   ~LocationTemporalRecord() {}

   bool operator==(const LocationTemporalRecord& other) const;
   size_t hash() const;
   LocationTemporalRow asPersistable() const;
   void merge(const LocationTemporalRecord& other);
   void setId(Int64 id) { _id = id; }
   Int64 getId() const { return _id; }

  private:
   mutable size_t _hash = -1;
   Int64 _id;

   // Data
   Int64 _dateId;           // 1
   Int64 _classifierSetId;  // 2
   Int64 _simUnitCount;     // 3
   double _simUnitAreaSum;  // 4
};

// --------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------
// id, classifier values
typedef Poco::Tuple<Int64, int, std::vector<Poco::Nullable<std::string>>> ClassifierSetRow;
class ClassifierSetRecord {
  public:
   explicit ClassifierSetRecord(int number_classifiers, std::vector<Poco::Nullable<std::string>> classifier_values);
   explicit ClassifierSetRecord(std::vector<Poco::Nullable<std::string>> classifier_values);
   ~ClassifierSetRecord() {}

   bool operator==(const ClassifierSetRecord& other) const;
   size_t hash() const;
   ClassifierSetRow asPersistable() const;
   void merge(const ClassifierSetRecord& other) {}
   void setId(Int64 id) { _id = id; }
   Int64 getId() const { return _id; }
   int numberClassifiers() const { return _numberClassifiers; };

  private:
   mutable size_t _hash = -1;
   Int64 _id;

   // Data
   int _numberClassifiers;
   std::vector<Poco::Nullable<std::string>> _classifierValues;
};

// --------------------------------------------------------------------------------------------
// id, pool name
typedef Poco::Tuple<Int64, std::string, std::string, int, int, double, std::string> PoolInfoRow;
class PoolInfoRecord {
  public:
   PoolInfoRecord(const std::string& name, const std::string& desc, int idx, int order, double scale,
                  const std::string& units);
   PoolInfoRecord(const std::string& name);
   ~PoolInfoRecord() {}

   bool operator==(const PoolInfoRecord& other) const;
   size_t hash() const;
   PoolInfoRow asPersistable() const;
   void merge(const PoolInfoRecord& other);
   void setId(Int64 id) { _id = id; }
   Int64 getId() const { return _id; }

  private:
   mutable size_t _hash = -1;
   // Data
   Int64 _id;
   std::string _name;
   std::string _desc;
   int _idx;
   int _order;
   double _scale;
   std::string _units;
};

// --------------------------------------------------------------------------------------------
// id, tile index, block index, tile lat, tile Lon
typedef Poco::Tuple<Int64, Poco::Nullable<UInt32>, Poco::Nullable<UInt32>, Poco::Nullable<UInt32>,
                    Poco::Nullable<double>, Poco::Nullable<double>, Poco::Nullable<double>, Poco::Nullable<double>,
                    Poco::Nullable<double>, Poco::Nullable<double>, Poco::Nullable<UInt32>, Poco::Nullable<UInt32>,
                    Poco::Nullable<UInt32>, Poco::Nullable<UInt32>>
    TileInfoRow;
class TileInfoRecord {
  public:
   TileInfoRecord(Poco::Nullable<UInt32> tileIdx, Poco::Nullable<UInt32> blockIdx, Poco::Nullable<UInt32> cellIdx,
                  Poco::Nullable<double> tileLat, Poco::Nullable<double> tileLon, Poco::Nullable<double> blockLat,
                  Poco::Nullable<double> blockLon, Poco::Nullable<double> cellLat, Poco::Nullable<double> cellLon,
                  Poco::Nullable<UInt32> globalRandomSeed, Poco::Nullable<UInt32> tileRandomSeed,
                  Poco::Nullable<UInt32> blockRandomSeed, Poco::Nullable<UInt32> cellRandomSeed)
       : _id(-1),
         _tileIdx(tileIdx),
         _blockIdx(blockIdx),
         _cellIdx(cellIdx),
         _tileLat(tileLat),
         _tileLon(tileLon),
         _blockLat(blockLat),
         _blockLon(blockLon),
         _cellLat(cellLat),
         _cellLon(cellLon),
         _globalRandomSeed(globalRandomSeed),
         _tileRandomSeed(tileRandomSeed),
         _blockRandomSeed(blockRandomSeed),
         _cellRandomSeed(cellRandomSeed) {}

   ~TileInfoRecord() {}

   bool operator==(const TileInfoRecord& other) const;
   size_t hash() const;
   TileInfoRow asPersistable() const;
   void merge(const TileInfoRecord& other);
   void setId(Int64 id) { _id = id; }
   Int64 getId() const { return _id; }

  private:
   mutable size_t _hash = -1;
   // Data
   Int64 _id;
   Poco::Nullable<UInt32> _tileIdx;
   Poco::Nullable<UInt32> _blockIdx;
   Poco::Nullable<UInt32> _cellIdx;
   Poco::Nullable<double> _tileLat;
   Poco::Nullable<double> _tileLon;
   Poco::Nullable<double> _blockLat;
   Poco::Nullable<double> _blockLon;
   Poco::Nullable<double> _cellLat;
   Poco::Nullable<double> _cellLon;
   Poco::Nullable<UInt32> _globalRandomSeed;
   Poco::Nullable<UInt32> _tileRandomSeed;
   Poco::Nullable<UInt32> _blockRandomSeed;
   Poco::Nullable<UInt32> _cellRandomSeed;
};

// --------------------------------------------------------------------------------------------
// id, interation, localdomainid, date id, locn id, flux type id, itemCount, src pool id, dst pool id, flux value
typedef Poco::Tuple<Int64, int, int, Int64, Int64, Poco::Nullable<Int64>, Int64, Int64, double, int> FluxRow;
struct FluxKey {
   short _localDomainId;
   short _interation;
   short _dateId;
   int _locationId;
   short _fluxTypeId;
   short _srcPoolInfoId;
   short _sinkPoolInfoId;
   bool operator==(const FluxKey& other) const;
   bool operator<(const FluxKey& other) const;
};

inline bool FluxKey::operator<(const FluxKey& other) const {
   if (_sinkPoolInfoId < other._sinkPoolInfoId) return true;
   if (_sinkPoolInfoId > other._sinkPoolInfoId) return false;
   if (_srcPoolInfoId < other._srcPoolInfoId) return true;
   if (_srcPoolInfoId > other._srcPoolInfoId) return false;
   if (_fluxTypeId < other._fluxTypeId) return true;
   if (_fluxTypeId > other._fluxTypeId) return false;
   if (_dateId < other._dateId) return true;
   if (_dateId > other._dateId) return false;
   if (_locationId < other._locationId) return true;
   if (_locationId > other._locationId) return false;
   if (_interation < other._interation) return true;
   if (_interation > other._interation) return false;
   if (_localDomainId < other._localDomainId) return true;
   if (_localDomainId > other._localDomainId) return false;
   return false;
}

inline bool FluxKey::operator==(const FluxKey& other) const {
   return _sinkPoolInfoId == other._sinkPoolInfoId && _srcPoolInfoId == other._srcPoolInfoId &&
          _fluxTypeId == other._fluxTypeId && _dateId == other._dateId && _locationId == other._locationId &&
          _localDomainId == other._localDomainId && _interation == other._interation;
}

struct FluxValue {
   FluxValue() : _id(-1), _flux(0.0), _itemCount(0) {}
   FluxValue(double flux) : _id(-1), _flux(flux), _itemCount(1) {}

   Int64 _id;
   double _flux;
   int _itemCount;
   FluxValue& operator+=(const FluxValue& rhs);
};

inline FluxValue& FluxValue::operator+=(const FluxValue& rhs) {
   _itemCount += rhs._itemCount;
   _flux += rhs._flux;
   return *this;
}

struct FluxRecordConverter {
   static FluxRow asPersistable(const FluxKey& key, const FluxValue& value) {
      return FluxRow{value._id,
                     key._interation,
                     key._localDomainId,
                     key._dateId,
                     key._locationId,
                     key._fluxTypeId < 0 ? Poco::Nullable<moja::Int64>() : Poco::Nullable<moja::Int64>(key._fluxTypeId),
                     key._srcPoolInfoId,
                     key._sinkPoolInfoId,
                     value._flux,
                     value._itemCount};
   }
};

// --------------------------------------------------------------------------------------------
// id, localdomainid, date id, locn id, pool id, pool value, count
typedef Poco::Tuple<Int64, int, int, Int64, Int64, Int64, double, int> StockRow;
struct StockKey {
   short _localDomainId;
   short _interation;
   short _dateId;
   int _locationId;
   short _poolId;
   bool operator==(const StockKey& other) const;
   bool operator<(const StockKey& other) const;
};

inline bool StockKey::operator==(const StockKey& other) const {
   return _poolId == other._poolId && _dateId == other._dateId && _locationId == other._locationId &&
          _localDomainId == other._localDomainId && _interation == other._interation;
}

inline bool StockKey::operator<(const StockKey& other) const {
   if (_poolId < other._poolId) return true;
   if (_poolId > other._poolId) return false;
   if (_dateId < other._dateId) return true;
   if (_dateId > other._dateId) return false;
   if (_locationId < other._locationId) return true;
   if (_locationId > other._locationId) return false;
   if (_interation < other._interation) return true;
   if (_interation > other._interation) return false;
   if (_localDomainId < other._localDomainId) return true;
   if (_localDomainId > other._localDomainId) return false;
   return false;
}

struct StockValue {
   StockValue() : _id(-1), _value(0.0), _itemCount(0) {}
   StockValue(double value) : _id(-1), _value(value), _itemCount(1) {}

   Int64 _id;
   double _value;
   int _itemCount;
   StockValue& operator+=(const StockValue& rhs);
};

inline StockValue& StockValue::operator+=(const StockValue& rhs) {
   _itemCount += rhs._itemCount;
   _value += rhs._value;
   return *this;
}

struct StockRecordConverter {
   static StockRow asPersistable(const StockKey& key, const StockValue& value) {
      return StockRow{value._id,       key._interation, key._localDomainId, key._dateId,
                      key._locationId, key._poolId,     value._value,       value._itemCount};
   }
};

/// -- Error handling records

// id, tile index, block index, grs, brs, crs, tile lat, tile Lon, date id, msg
typedef Poco::Tuple<Int64,                   //  0
                    int,                     //  1
                    Poco::Nullable<UInt32>,  //  2
                    Poco::Nullable<UInt32>,  //  3
                    Poco::Nullable<UInt32>,  //  4
                    Poco::Nullable<UInt32>,  //  5
                    Poco::Nullable<UInt32>,  //  6
                    Poco::Nullable<UInt32>,  //  7
                    Poco::Nullable<double>,  //  8
                    Poco::Nullable<double>,  //  9
                    Poco::Nullable<double>,  // 10
                    Poco::Nullable<double>,  // 11
                    Poco::Nullable<double>,  // 12
                    Poco::Nullable<double>,  // 13
                    Poco::Nullable<Int64>,   // 14
                    int,                     // 15
                    std::string,             // library			// 16
                    std::string,             // module			// 17
                    std::string>
    ErrorRow;  // 18
class ErrorRecord {
  public:
   ErrorRecord(int localDomainId, Poco::Nullable<UInt32> tileIdx, Poco::Nullable<UInt32> blockIdx,
               Poco::Nullable<UInt32> cellIdx, Poco::Nullable<UInt32> randomSeedGlobal,
               Poco::Nullable<UInt32> randomSeedBlock, Poco::Nullable<UInt32> randomSeedCell,
               Poco::Nullable<double> tileLat, Poco::Nullable<double> tileLon, Poco::Nullable<double> blockLat,
               Poco::Nullable<double> blockLon, Poco::Nullable<double> cellLat, Poco::Nullable<double> cellLon,
               Poco::Nullable<Int64> dateId, int errorCode, const std::string& library, const std::string& module,
               const std::string& msg);
   ~ErrorRecord() {}

   bool operator==(const ErrorRecord& other) const;
   size_t hash() const;
   ErrorRow asPersistable() const;
   void merge(const ErrorRecord& other);
   void setId(Int64 id) { _id = id; }
   Int64 getId() const { return _id; }

   int getLocalDomainId() const { return _localDomainId; }
   Poco::Nullable<UInt32> gettileIdx() const { return _tileIdx; }
   Poco::Nullable<UInt32> getblockIdx() const { return _blockIdx; }
   Poco::Nullable<UInt32> getcellIdx() const { return _cellIdx; }
   Poco::Nullable<UInt32> getrandomSeedGlobal() const { return _randomSeedGlobal; }
   Poco::Nullable<UInt32> getrandomSeedBlock() const { return _randomSeedBlock; }
   Poco::Nullable<UInt32> getrandomSeedCell() const { return _randomSeedCell; }
   Poco::Nullable<double> gettileLat() const { return _tileLat; }
   Poco::Nullable<double> gettileLon() const { return _tileLon; }
   Poco::Nullable<double> getblockLat() const { return _blockLat; }
   Poco::Nullable<double> getblockLon() const { return _blockLon; }
   Poco::Nullable<double> getcellLat() const { return _cellLat; }
   Poco::Nullable<double> getcellLon() const { return _cellLon; }
   Poco::Nullable<Int64> getdateId() const { return _dateId; }
   int geterrorCode() const { return _errorCode; }
   std::string getlibrary() const { return _library; };
   std::string getmodule() const { return _module; };
   std::string getmsg() const { return _msg; };

  private:
   mutable size_t _hash = -1;
   // Data
   Int64 _id;
   int _localDomainId;                        //  1
   Poco::Nullable<UInt32> _tileIdx;           //	2
   Poco::Nullable<UInt32> _blockIdx;          //	3
   Poco::Nullable<UInt32> _cellIdx;           //	4
   Poco::Nullable<UInt32> _randomSeedGlobal;  //	5
   Poco::Nullable<UInt32> _randomSeedBlock;   //	6
   Poco::Nullable<UInt32> _randomSeedCell;    //	7
   Poco::Nullable<double> _tileLat;           //	8
   Poco::Nullable<double> _tileLon;           //	9
   Poco::Nullable<double> _blockLat;          // 10
   Poco::Nullable<double> _blockLon;          // 11
   Poco::Nullable<double> _cellLat;           // 12
   Poco::Nullable<double> _cellLon;           // 13
   Poco::Nullable<Int64> _dateId;             // 14
   int _errorCode;                            // 15
   std::string _library;                      // 16
   std::string _module;                       // 17
   std::string _msg;                          // 18
};

inline ErrorRecord::ErrorRecord(int localDomainId, Poco::Nullable<UInt32> tileIdx, Poco::Nullable<UInt32> blockIdx,
                                Poco::Nullable<UInt32> cellIdx, Poco::Nullable<UInt32> randomSeedGlobal,
                                Poco::Nullable<UInt32> randomSeedBlock, Poco::Nullable<UInt32> randomSeedCell,
                                Poco::Nullable<double> tileLat, Poco::Nullable<double> tileLon,
                                Poco::Nullable<double> blockLat, Poco::Nullable<double> blockLon,
                                Poco::Nullable<double> cellLat, Poco::Nullable<double> cellLon,
                                Poco::Nullable<Int64> dateId, int errorCode, const std::string& library,
                                const std::string& module, const std::string& msg)
    : _id(-1),
      _localDomainId(localDomainId),
      _tileIdx(tileIdx),
      _blockIdx(blockIdx),
      _cellIdx(cellIdx),
      _randomSeedGlobal(randomSeedGlobal),
      _randomSeedBlock(randomSeedBlock),
      _randomSeedCell(randomSeedCell),
      _tileLat(tileLat),
      _tileLon(tileLon),
      _blockLat(blockLat),
      _blockLon(blockLon),
      _cellLat(cellLat),
      _cellLon(cellLon),
      _dateId(dateId),
      _errorCode(errorCode),
      _library(library),
      _module(module),
      _msg(msg) {}

inline bool ErrorRecord::operator==(const ErrorRecord& other) const { return false; }

inline size_t ErrorRecord::hash() const {
   if (_hash == -1) _hash = hash::hash_combine(_id);
   return _hash;
}

inline ErrorRow ErrorRecord::asPersistable() const {
   return ErrorRow{
       _id,      _localDomainId, _tileIdx,  _blockIdx, _cellIdx, _randomSeedGlobal, _randomSeedBlock, _randomSeedCell,
       _tileLat, _tileLon,       _blockLat, _blockLon, _cellLat, _cellLon,          _dateId,          _errorCode,
       _library, _module,        _msg};
}

inline void ErrorRecord::merge(const ErrorRecord& other) {}
}  // namespace chapman_richards
}  // namespace modules
}  // namespace moja
