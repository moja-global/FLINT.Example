#include "moja/modules/chapman_richards/commondata.h"

namespace moja::modules::chapman_richards {

ObjectHolder& ObjectHolder::Instance() {
   static ObjectHolder instance;
   return instance;
}

}  // namespace moja::modules::chapman_richards