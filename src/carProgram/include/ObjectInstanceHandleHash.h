#pragma once
#include <RTI/RTIambassador.h>
#include <functional>

namespace std {
    template <>
    struct hash<rti1516e::ObjectInstanceHandle> {
        std::size_t operator()(const rti1516e::ObjectInstanceHandle& handle) const noexcept {
            return std::hash<std::wstring>()(handle.toString());
        }
    };
}