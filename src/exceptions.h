#pragma once

#include <defines.h>

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string_view>

namespace shvav8 {

class Exception {
   public:
    virtual ~Exception() = default;
    virtual std::string_view message() const = 0;
};

std::ostream& operator<<(std::ostream& ostream, const Exception& exception);

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
class StateException : public Exception {
   public:
    // TODO: get state when implemented
    StateException(u16 pc, u16 opcode) : m_message(format_message(pc, opcode)) {}

    std::string_view message() const override { return m_message; }

   private:
    const std::string m_message;

    static std::string format_message(u16 pc, u16 opcode) {
        std::ostringstream sstream;
        sstream << std::hex << std::setfill('0') << T::name() << " exception at position $"
                << std::setw(4) << pc << " (opcode: $" << std::setw(4) << opcode << ")";
        return sstream.str();
    }
};

using StackOverflowException = StateException<StackOverflow>;
using StackUnderflowException = StateException<StackUnderflow>;
using MemoryOverflowException = StateException<MemoryOverflow>;
using KeyOutOfRangeException = StateException<KeyOutOfRange>;

}  // namespace shvav8
