/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 4 -*-
 * vim: set ts=8 sts=4 et sw=4 tw=99:
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef jsopcodeinlines_h
#define jsopcodeinlines_h

#include "jsopcode.h"

#include "jsscript.h"

namespace js {

static inline unsigned
GetDefCount(JSScript *script, unsigned offset)
{
    jsbytecode *pc = script->offsetToPC(offset);

    /*
     * Add an extra pushed value for OR/AND opcodes, so that they are included
     * in the pushed array of stack values for type inference.
     */
    switch (JSOp(*pc)) {
      case JSOP_OR:
      case JSOP_AND:
        return 1;
      case JSOP_PICK:
        /*
         * Pick pops and pushes how deep it looks in the stack + 1
         * items. i.e. if the stack were |a b[2] c[1] d[0]|, pick 2
         * would pop b, c, and d to rearrange the stack to |a c[0]
         * d[1] b[2]|.
         */
        return (pc[1] + 1);
      default:
        return StackDefs(script, pc);
    }
}

static inline unsigned
GetUseCount(JSScript *script, unsigned offset)
{
    jsbytecode *pc = script->offsetToPC(offset);

    if (JSOp(*pc) == JSOP_PICK)
        return (pc[1] + 1);
    if (js_CodeSpec[*pc].nuses == -1)
        return StackUses(script, pc);
    return js_CodeSpec[*pc].nuses;
}

class BytecodeRange {
  public:
    BytecodeRange(JSContext *cx, JSScript *script)
      : script(cx, script), pc(script->code()), end(pc + script->length())
    {}
    bool empty() const { return pc == end; }
    jsbytecode *frontPC() const { return pc; }
    JSOp frontOpcode() const { return JSOp(*pc); }
    size_t frontOffset() const { return script->pcToOffset(pc); }
    void popFront() { pc += GetBytecodeLength(pc); }

  private:
    RootedScript script;
    jsbytecode *pc, *end;
};

}

#endif /* jsopcodeinlines_h */
