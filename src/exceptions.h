#pragma once

#include <defines.h>

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string_view>
#include <type_traits>

namespace shvav8 {

class Exception {
   public:
    virtual ~Exception() = default;
    virtual std::string_view message() const = 0;
};

std::ostream& operator<<(std::ostream& ostream, const Exception& exception);

struct Overflow {};
struct Underflow {};

template <class T>
class StackException : public Exception {
    static_assert(std::is_same<T, Overflow>() || std::is_same<T, Underflow>(),
                  "T must be type of Overflow or Underflow");

   public:
    StackException(u16 pc, u16 opcode) : m_message(format_message(pc, opcode)) {}

    std::string_view message() const override { return m_message; }

   private:
    const std::string m_message;

    static std::string format_message(u16 pc, u16 opcode) {
        std::ostringstream sstream;
        sstream << std::hex << std::setfill('0');

        if constexpr (std::is_same<T, Overflow>()) {
            sstream << "Stack overflow exception at position $";
        } else {
            sstream << "Stack underflow exception at position $";
        }

        sstream << std::setw(4) << pc << " (opcode: $" << std::setw(4) << opcode << ")";
        return sstream.str();
    }
};

using StackOverflowException = StackException<Overflow>;
using StackUnderflowException = StackException<Underflow>;

}  // namespace shvav8
