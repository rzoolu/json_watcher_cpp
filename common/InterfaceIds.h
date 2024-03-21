#pragma once

#include <cstdint>

namespace msg
{

enum class IfaceId : std::uint32_t
{
    UNUSED = 0,
    ApWatchI = 1,
};

constexpr const char* toStr(IfaceId id)
{
    switch (id)
    {
#define ENUM_CASE(val) \
    case val:          \
        return #val;

        ENUM_CASE(IfaceId::UNUSED)
        ENUM_CASE(IfaceId::ApWatchI)

#undef ENUM_CASE

    default:
        return "ERROR: updAte msg::toStr(IfaceId)";
    }
}
} // namespace msg