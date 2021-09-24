#pragma once

#include <iomanip>
#include <sstream>
#include <string_view>

#include "defines.h"

namespace shvav8 {

class StateException : public std::exception {
   public:
    // TODO: get state when implemented
    StateException(u16 pc, u16 opcode) : m_message(format_message(pc, opcode)) {}

    const char* what() const noexcept override { return m_message.c_str(); }

   protected:
    virtual std::string_view name() = 0;

   private:
    const std::string m_message;

    std::string format_message(u16 pc, u16 opcode) {
        std::ostringstream sstream;
        sstream << std::hex << std::setfill('0') << name() << " exception at position $"
                << std::setw(4) << pc << " (opcode: $" << std::setw(4) << opcode << ")";
        return sstream.str();
    }
};

class StackOverflowException : public StateException {
   public:
    StackOverflowException(u16 pc, u16 opcode) : StateException(pc, opcode) {}

   protected:
    std::string_view name() override { return "Stack overflow"; }
};
class StackUnderflowException : public StateException {
   public:
    StackUnderflowException(u16 pc, u16 opcode) : StateException(pc, opcode) {}

   protected:
    std::string_view name() override { return "Stack underflow"; }
};
class MemoryOverflowException : public StateException {
   public:
    MemoryOverflowException(u16 pc, u16 opcode) : StateException(pc, opcode) {}

   protected:
    std::string_view name() override { return "Memory overflow"; }
};
class KeyOutOfRangeException : public StateException {
   public:
    KeyOutOfRangeException(u16 pc, u16 opcode) : StateException(pc, opcode) {}

   protected:
    std::string_view name() override { return "Key out of range"; }
};

}  // namespace shvav8
