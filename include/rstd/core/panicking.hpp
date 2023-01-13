namespace rstd {
namespace core {
namespace panicking {

 __attribute__((noreturn))
void panic(void);

}
}

using core::panicking::panic;
}
