#pragma once

#include <rstd/core/slice.hpp>
#include <rstd/core/result.hpp>
#include <rstd/core/macros.hpp>

namespace rstd {
namespace core {
namespace ops {

template<typename Idx>
struct Range {
    Idx start;
    Idx end;

    Range() = delete;

    Range(Idx start, Idx end)
        : start(start)
        , end(end)
    { }
};

template<typename Idx>
struct RangeFrom {
    Idx start;

    RangeFrom() = delete;

    RangeFrom(Idx start)
        : start(start)
    { }
};

}
}
}
