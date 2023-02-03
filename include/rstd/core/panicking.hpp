namespace rstd {
namespace core {

class Never;

namespace panicking {

 __attribute__((noreturn))
Never panic(void);

}
}

using core::panicking::panic;
}
