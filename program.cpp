// This file is part of the IMP project.

#include "program.h"



// -----------------------------------------------------------------------------
std::ostream &operator<<(std::ostream &os, Opcode op)
{
  switch (op) {
    case Opcode::PUSH_FUNC:  return os << "PUSH_FUNC";
    case Opcode::PUSH_PROTO: return os << "PUSH_PROTO";
    case Opcode::PEEK:       return os << "PEEK";
    case Opcode::POP:        return os << "POP";
    case Opcode::CALL:       return os << "CALL";
    case Opcode::ADD:        return os << "ADD";
    case Opcode::RET:        return os << "RET";
    case Opcode::JUMP_FALSE: return os << "JUMP_FALSE";
    case Opcode::JUMP:       return os << "JUMP";
    case Opcode::STOP:       return os << "STOP";
  }
  assert(!"invalid opcode");
}
