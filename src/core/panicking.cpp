#include <rstd/core/panicking.hpp>
#include <rstd/core/primitive.hpp>

namespace rstd {
namespace core {
namespace panicking {

Never panic(void) {
    throw "Panic!";
}

}
}
}
