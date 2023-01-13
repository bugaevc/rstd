#include <rstd/core/panicking.hpp>

namespace rstd {
namespace core {
namespace panicking {

void panic(void) {
    throw "Panic!";
}

}
}
}
