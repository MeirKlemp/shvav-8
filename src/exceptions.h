#pragma once

#include <defines.h>

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string_view>
#include <type_traits>

namespace shvav8 {

struct StackOverflow {
    constexpr static auto name() { return "Stack overflow"; }
};
struct StackUnderflow {
    constexpr static auto name() { return "Stack underflow"; }
};
struct MemoryOverflow {
    constexpr static auto name() { return "Memory overflow"; }
};
struct KeyOutOfRange {
    constexpr static auto name() { return "Key out of range"; }
};

template <class T>
class Exception {
   public:
    Exception(u16 pc, u16 opcode) : m_message(format_message(pc, opcode)) {}

    std::string_view message() const { return m_message; }

   private:
    const std::string m_message;

    static std::string format_message(u16 pc, u16 opcode) {
        std::ostringstream sstream;
        sstream << std::hex << std::setfill('0') << T::name() << " exception at position $"
                << std::setw(4) << pc << " (opcode: $" << std::setw(4) << opcode << ")";
        return sstream.str();
    }
};

template <class T>
std::ostream& operator<<(std::ostream& ostream, const Exception<T>& exception) {
    return ostream << exception.message();
}

using StackOverflowException = Exception<StackOverflow>;
using StackUnderflowException = Exception<StackUnderflow>;
using MemoryOverflowException = Exception<MemoryOverflow>;
using KeyOutOfRangeException = Exception<KeyOutOfRange>;

}  // namespace shvav8
