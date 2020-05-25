#include "moja/modules/chapman_richards/commondata.h"

namespace moja {
namespace modules {
namespace chapman_richards {

ObjectHolder& ObjectHolder::Instance() {
   static ObjectHolder instance;
   return instance;
}
}  // namespace chapman_richards
}  // namespace modules
}  // namespace moja
