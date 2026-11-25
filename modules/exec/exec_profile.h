#ifndef __EXEC_PROFILE_H__
#define __EXEC_PROFILE_H__

#include <exec.h>
#include "utils.h"

namespace X_ROOT_NS::modules::exec {

    ////////// ////////// ////////// ////////// //////////
    // The profile of the commands of the interpreter (APRIL_EXEC_PROFILE=1)
    //
    // It is made to choose the superinstructions (docs/xil.md): each command of each method that is parsed is counted when it is
    // executed, and when the process ends the report tells the methods that are called most, and the sequences of xils that follow each
    // other (that can be joined into one command) by the number of times that they are executed. The sequences do not cross the
    // places that a jump or a block goes to: the commands that are joined begin at such a place or have none in them.

    // Returns whether the profile is on.
    bool exec_profile_enabled();

    // The places of a method that a command must begin at: a jump or a switch goes to it, a block begins, ends or is entered there.
    // has_loop is true when a jump goes back.
    std::vector<uint8_t> exec_cut_points(const std::vector<const core::xil_base_t *> & xils,
                exec_switch_manager_t & switches, const al::svector_t<core::method_xil_block_t, 10> & blocks,
                bool * has_loop);

    // The text of a xil with no numbers: the xils of the same kind have the same one.
    string_t exec_xil_shape(const core::xil_base_t * xil);

    // Registers a method that was parsed. Returns the counters of its commands (one for each), that the profile commands add to.
    uint64_t * exec_profile_register(const string_t & name, std::vector<string_t> && shapes,
                std::vector<uint8_t> && cuts, bool has_loop, uint64_t parse_ns, uint64_t cut_ns);

}   // namespace X_ROOT_NS::modules::exec

#endif  // __EXEC_PROFILE_H__
