#ifndef __EXEC_FUSION_H__
#define __EXEC_FUSION_H__

#include <exec.h>
#include "utils.h"

namespace X_ROOT_NS::modules::exec {

    ////////// ////////// ////////// ////////// //////////
    // Superinstructions
    //
    // A superinstruction is not an xil: it is a command that does the work of several xils that follow each other, made when the commands
    // of a method are made (modules/exec/parser.cpp). The array of the commands is shorter than the array of the xils then: the places
    // that the jumps, the switch tables and the blocks go to are numbered again (the plan has the numbers).
    //
    // The xils that are joined are those of numbers (they cannot throw, and they do not call): the pushes of locals, arguments and
    // constants, the arithmetic, the bits, the compare, the pop to a local, and the jump that follows a compare. A command cannot have
    // a place in it that a jump or a block goes to, except at its first xil. See docs/xil.md.

    // The kinds of the operand of a binary operation: from the stack, a local variable, an argument, a constant.
    enum class fusion_operand_t : uint8_t { stack = 0, local = 1, argument = 2, constant = 3 };

    // The most xils that a command has.
    constexpr size_t fusion_max_length = 5;

    // The forms of the commands that are made.
    enum class fusion_form_t : uint8_t
    {
        none,
        binary,         // [x] [y] OP|CMP [pop local [jmp] | jmp true/false]
        push2,          // push x, push y
        copy,           // push x, pop local
        this_field,     // push argument this, push field
        convert,        // push x, convert
        store_field,    // [push x] push argument this, pop field
        array_load,     // [push index] [push array] push array_element
        array_store,    // [push x] [push index] [push array] pop array_element [jmp]
        ret_push,       // push x, ret
    };

    // What a matcher found at a place of the xils.
    struct fusion_shape_t
    {
        fusion_form_t       form = fusion_form_t::none;
        uint8_t             length = 0;

        // Binary: where the operands are (an index in the group, -1 when it is the stack), the xil of the operation, the tail.
        fusion_operand_t    left = fusion_operand_t::stack, right = fusion_operand_t::stack;
        int8_t              left_index = -1, right_index = -1, op_index = -1, tail_index = -1, jump_index = -1;
        uint8_t             tail = 0;           // 0: the result is pushed, 1: it is stored in a local, 2: a jump, 3: stored, and a jump, 4: returned.

        // Arrays: the operands of the index and of the array (left is the value that a store writes), and where their xils are.
        // A xil that is the operand of an array command may have any type (a boolean, a character, an object), as the matcher says.
        fusion_operand_t    index = fusion_operand_t::stack, array = fusion_operand_t::stack;
        int8_t              index_index = -1, array_index = -1;
    };

    // Finds the longest superinstruction at the beginning of the xils: at most max_length of them.
    bool fusion_match(const core::xil_base_t * const * xils, size_t available, size_t max_length, fusion_shape_t * shape);

    // A group of xils that is one command.
    struct fusion_group_t
    {
        uint32_t start;
        uint32_t length;
    };

    // The plan of the commands of a method: the groups, and the index of the command of each xil. It depends on the xils only (not on the
    // types that a generic method has), so the instances of a generic method share it.
    struct fusion_plan_t
    {
        std::vector<fusion_group_t> groups;
        std::vector<uint32_t>       index_map;          // The index of the command of each xil (and the count of the commands at the end).
        bool                        has_loop = false;
        bool                        has_blocks = false;

        uint32_t command_count() const { return index_map.back(); }
    };

    // Makes the plan of the xils of a method (or finds the one that was made), nullptr when no group can be made.
    // The jumps, the switch tables and the blocks are used to find the places that a command must begin at.
    const fusion_plan_t * fusion_plan_of(const std::vector<const core::xil_base_t *> & xils, exec_switch_manager_t & switches,
                const al::svector_t<core::method_xil_block_t, 10> & blocks, bool * has_loop);

    // The mode of the superinstructions: APRIL_SUPER=off|auto|eager (auto is the default).
    enum class fusion_mode_t { off, automatic, eager };
    fusion_mode_t fusion_mode();

    // The calls after which a method that has no loop gets its superinstructions: APRIL_SUPER_THRESHOLD (50).
    int fusion_threshold();

}   // namespace X_ROOT_NS::modules::exec

#endif  // __EXEC_FUSION_H__
