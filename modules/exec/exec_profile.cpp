
#include "exec_profile.h"
#include <lib.h>

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <map>
#include <mutex>

namespace X_ROOT_NS::modules::core {

    // Converts xil to a string (core/xil.cpp).
    string_t __to_string(const xil_t * xil) _NE;

}

namespace X_ROOT_NS::modules::exec {

    using namespace core;

    ////////// ////////// ////////// ////////// //////////

    // Returns whether the profile is on.
    bool exec_profile_enabled()
    {
        static const bool enabled = []() {
            const char * value = std::getenv("APRIL_EXEC_PROFILE");
            return value != nullptr && *value != '\0';
        }();

        return enabled;
    }

    ////////// ////////// ////////// ////////// //////////

    // The places of a method that a command must begin at.
    std::vector<uint8_t> exec_cut_points(const std::vector<const xil_base_t *> & xils,
                exec_switch_manager_t & switches, const al::svector_t<method_xil_block_t, 10> & blocks,
                bool * has_loop)
    {
        long long count = (long long)xils.size();
        std::vector<uint8_t> cuts((size_t)count + 1, 0);

        auto mark = [&](long long index) {
            if (index >= 0 && index <= count)
                cuts[(size_t)index] = 1;
        };

        *has_loop = false;

        for (long long index = 0; index < count; index++)
        {
            const xil_t * xil = xils[(size_t)index];
            if (command_of(xil) != xil_command_t::jmp)
                continue;

            const jmp_xil_t * jmp = (const jmp_xil_t *)xil;

            if (jmp->model() == xil_jmp_model_t::switch_)
            {
                exec_switch_table_t * table = switches.get_table(jmp->tbl());
                if (table == nullptr)
                    continue;

                for (int k = 0; k < table->row_count; k++)
                    mark(index + table->rows[k].step);

                mark(index + table->default_step);
                continue;
            }

            long long target = index + (long long)jmp->step();
            mark(target);

            if (target <= index)
                *has_loop = true;
        }

        for (const method_xil_block_t & block : blocks)
        {
            mark(block.xil_start);
            mark(block.xil_end);
            mark(block.entry_point);
        }

        return cuts;
    }

    // The text of a xil with no numbers.
    string_t exec_xil_shape(const xil_base_t * xil)
    {
        string_t text = __to_string((const xil_t *)xil);
        string_t shape;
        bool digit = false;

        for (auto c : text)
        {
            bool is_digit = (c >= _T('0') && c <= _T('9'));
            if (is_digit)
            {
                if (!digit)
                    shape += _T('#');
            }
            else
            {
                shape += c;
            }

            digit = is_digit;
        }

        return shape;
    }

    ////////// ////////// ////////// ////////// //////////
    // The registry

    namespace {

        struct __method_t
        {
            string_t                name;
            std::vector<string_t>   shapes;
            std::vector<uint8_t>    cuts;
            std::vector<uint64_t>   counts;
            bool                    has_loop = false;
        };

        std::mutex                  __lock;
        std::vector<__method_t *>   __methods;
        uint64_t                    __parse_ns = 0, __cut_ns = 0;
        size_t                      __xil_count = 0;
        bool                        __registered = false;

        // Whether the xil goes elsewhere than to the xil that follows it (it can only be the last of the xils of a command).
        bool __changes_flow(const string_t & shape)
        {
            static const wchar_t * prefixes[] = { L"jmp", L"ret", L"throw", L"rethrow", L"leave", L"switch", L"end finally" };

            for (const wchar_t * prefix : prefixes)
            {
                if (shape.compare(0, wcslen(prefix), prefix) == 0)
                    return true;
            }

            return false;
        }

        std::wstring __percent(double value, double total)
        {
            wchar_t buffer[32];
            swprintf(buffer, 32, L"%.1f%%", total > 0? value * 100 / total : 0.0);
            return buffer;
        }

        ////////// ////////// ////////// ////////// //////////
        // The simulation of the superinstructions

        enum class __kind_t
        {
            other, push_value, push_const, alu, cmp, pop_local, jump_cond, jump, convert, push_this, push_field,
        };

        bool __starts(const string_t & s, const wchar_t * prefix)
        {
            return s.compare(0, wcslen(prefix), prefix) == 0;
        }

        bool __has(const string_t & s, const wchar_t * text)
        {
            return s.find(text) != string_t::npos;
        }

        // The kind of a xil, as the superinstructions see it: what they join are the xils of numbers, that cannot throw.
        __kind_t __kind_of(const string_t & s)
        {
            bool object = __has(s, L"object") || __has(s, L"string") || __has(s, L"ptr") || __has(s, L"addr");

            if ((__starts(s, L"push local ") || __starts(s, L"push argument ")) && !object)
                return __kind_t::push_value;

            if (__starts(s, L"push argument object"))
                return __kind_t::push_this;

            if (__starts(s, L"push constant (") && !object)
                return __kind_t::push_const;

            if (__starts(s, L"push field ") && !object)
                return __kind_t::push_field;

            if (__starts(s, L"pop local ") && !object)
                return __kind_t::pop_local;

            static const wchar_t * alus[] = { L"add ", L"sub ", L"mul ", L"bit_and ", L"bit_or ", L"bit_xor ", L"left_shift ", L"right_shift " };
            for (const wchar_t * alu : alus)
            {
                if (__starts(s, alu) && !object)
                    return __kind_t::alu;
            }

            static const wchar_t * cmps[] = { L"less ", L"greater ", L"equal ", L"not_equal ", L"less_equal ", L"greater_equal " };
            for (const wchar_t * cmp : cmps)
            {
                if (__starts(s, cmp) && !object && !__has(s, L"bool"))
                    return __kind_t::cmp;
            }

            if (__starts(s, L"jmp true") || __starts(s, L"jmp false"))
                return __kind_t::jump_cond;

            if (__starts(s, L"jmp "))
                return __kind_t::jump;

            if (__starts(s, L"convert ") && !object)
                return __kind_t::convert;

            return __kind_t::other;
        }

        struct __pattern_t
        {
            const wchar_t *             name;
            std::vector<__kind_t>       kinds;
            uint64_t                    uses = 0, saved = 0;
        };

        // The superinstructions that are tried: the longest first.
        std::vector<__pattern_t> __patterns()
        {
            typedef __kind_t K;
            std::vector<__pattern_t> list = {
                { L"value value cmp jump_cond",     { K::push_value, K::push_value, K::cmp, K::jump_cond } },
                { L"value const cmp jump_cond",     { K::push_value, K::push_const, K::cmp, K::jump_cond } },
                { L"value const alu pop",           { K::push_value, K::push_const, K::alu, K::pop_local } },
                { L"value value alu pop",           { K::push_value, K::push_value, K::alu, K::pop_local } },
                { L"value const cmp",               { K::push_value, K::push_const, K::cmp } },
                { L"value value cmp",               { K::push_value, K::push_value, K::cmp } },
                { L"value const alu",               { K::push_value, K::push_const, K::alu } },
                { L"value value alu",               { K::push_value, K::push_value, K::alu } },
                { L"value alu pop",                 { K::push_value, K::alu, K::pop_local } },
                { L"const alu pop",                 { K::push_const, K::alu, K::pop_local } },
                { L"cmp jump_cond",                 { K::cmp, K::jump_cond } },
                { L"value cmp",                     { K::push_value, K::cmp } },
                { L"const cmp",                     { K::push_const, K::cmp } },
                { L"value alu",                     { K::push_value, K::alu } },
                { L"const alu",                     { K::push_const, K::alu } },
                { L"alu pop",                       { K::alu, K::pop_local } },
                { L"alu jump",                      { K::alu, K::jump } },
                { L"pop jump",                      { K::pop_local, K::jump } },
                { L"value pop",                     { K::push_value, K::pop_local } },
                { L"const pop",                     { K::push_const, K::pop_local } },
                { L"value convert",                 { K::push_value, K::convert } },
                { L"this field",                    { K::push_this, K::push_field } },
                { L"value value",                   { K::push_value, K::push_value } },
                { L"value const",                   { K::push_value, K::push_const } },
                { L"const value",                   { K::push_const, K::push_value } },
            };

            return list;
        }

        // Joins the xils of a method as the superinstructions would, from the first to the last: returns the number of the dispatches that
        // are saved. use_cuts: a command cannot have a place that a jump or a block goes to in it.
        uint64_t __simulate(__method_t * method, std::vector<__pattern_t> & patterns, bool use_cuts)
        {
            size_t size = method->shapes.size();
            std::vector<__kind_t> kinds(size);
            for (size_t k = 0; k < size; k++)
                kinds[k] = __kind_of(method->shapes[k]);

            uint64_t saved = 0;

            for (size_t k = 0; k < size;)
            {
                size_t taken = 1;

                if (method->counts[k] > 0)
                {
                    for (__pattern_t & pattern : patterns)
                    {
                        size_t n = pattern.kinds.size();
                        if (k + n > size)
                            continue;

                        bool match = true;
                        uint64_t weight = method->counts[k];

                        for (size_t j = 0; j < n && match; j++)
                        {
                            match = kinds[k + j] == pattern.kinds[j];
                            weight = std::min(weight, method->counts[k + j]);

                            if (j > 0 && use_cuts && method->cuts[k + j])
                                match = false;
                        }

                        if (!match)
                            continue;

                        pattern.uses += weight;
                        pattern.saved += weight * (n - 1);
                        saved += weight * (n - 1);
                        taken = n;
                        break;
                    }
                }

                k += taken;
            }

            return saved;
        }

        // The report, when the process ends.
        void __report()
        {
            std::lock_guard<std::mutex> guard(__lock);

            uint64_t executed = 0;
            size_t entered = 0;

            for (__method_t * method : __methods)
            {
                for (uint64_t count : method->counts)
                    executed += count;

                if (!method->counts.empty() && method->counts[0] > 0)
                    entered++;
            }

            std::wostream & out = std::wcerr;

            out << L"\n== exec profile\n";
            out << L"methods parsed: " << __methods.size() << L", entered: " << entered << L", xils: " << __xil_count
                << L", commands executed: " << executed << L"\n";
            out << L"time to parse the commands: " << (__parse_ns / 1000) << L" us, of that the cut points: "
                << (__cut_ns / 1000) << L" us\n";

            // The methods by the times that they are entered (the first command is run at each entry; a jump back to it is counted too).
            out << L"\n-- methods by calls (calls >= N): methods, share of the methods, share of the commands executed\n";

            static const uint64_t thresholds[] = { 1, 2, 5, 10, 20, 50, 100, 500, 1000, 10000, 100000 };
            for (uint64_t threshold : thresholds)
            {
                size_t methods = 0;
                uint64_t commands = 0;

                for (__method_t * method : __methods)
                {
                    if (method->counts.empty() || method->counts[0] < threshold)
                        continue;

                    methods++;
                    for (uint64_t count : method->counts)
                        commands += count;
                }

                out << L"  >= " << threshold << L":\t" << methods << L"\t" << __percent((double)methods, (double)entered)
                    << L"\t" << __percent((double)commands, (double)executed) << L"\n";
            }

            {
                size_t methods = 0;
                uint64_t commands = 0, single_call_commands = 0;

                for (__method_t * method : __methods)
                {
                    if (!method->has_loop || method->counts.empty() || method->counts[0] == 0)
                        continue;

                    methods++;

                    uint64_t sum = 0;
                    for (uint64_t count : method->counts)
                        sum += count;

                    commands += sum;
                    if (method->counts[0] < 20)
                        single_call_commands += sum;
                }

                out << L"  with a loop:\t" << methods << L"\t" << __percent((double)methods, (double)entered) << L"\t"
                    << __percent((double)commands, (double)executed) << L"  (of which entered < 20 times: "
                    << __percent((double)single_call_commands, (double)executed) << L")\n";
            }

            // The rule that is proposed: a method that has a loop is made fast at once, the others when they were called N times.
            for (uint64_t threshold : { 10, 20, 50, 100 })
            {
                size_t methods = 0;
                uint64_t commands = 0;

                for (__method_t * method : __methods)
                {
                    if (method->counts.empty() || method->counts[0] == 0)
                        continue;

                    if (!method->has_loop && method->counts[0] < threshold)
                        continue;

                    methods++;
                    for (uint64_t count : method->counts)
                        commands += count;
                }

                out << L"  rule (loop, or calls >= " << threshold << L"):\t" << methods << L"\t" << __percent((double)methods, (double)entered)
                    << L"\t" << __percent((double)commands, (double)executed) << L"\n";
            }

            // The methods that run the most.
            out << L"\n-- the methods that execute the most commands: commands, calls, xils, name\n";
            {
                std::vector<std::pair<uint64_t, __method_t *>> sorted;
                for (__method_t * method : __methods)
                {
                    uint64_t sum = 0;
                    for (uint64_t count : method->counts)
                        sum += count;

                    if (sum > 0)
                        sorted.push_back({ sum, method });
                }

                std::sort(sorted.begin(), sorted.end(), [](auto & a, auto & b) { return a.first > b.first; });

                for (size_t k = 0; k < sorted.size() && k < 15; k++)
                {
                    __method_t * method = sorted[k].second;
                    out << L"  " << sorted[k].first << L"\t" << method->counts[0] << L"\t" << method->shapes.size() << L"\t"
                        << method->name << L"\n";
                }
            }

            // The kinds of xils that are executed the most.
            out << L"\n-- the xils executed (by the kind)\n";
            {
                std::map<string_t, uint64_t> shapes;
                for (__method_t * method : __methods)
                {
                    for (size_t k = 0; k < method->shapes.size(); k++)
                    {
                        if (method->counts[k] > 0)
                            shapes[method->shapes[k]] += method->counts[k];
                    }
                }

                std::vector<std::pair<uint64_t, string_t>> sorted;
                for (auto && it : shapes)
                    sorted.push_back({ it.second, it.first });

                std::sort(sorted.begin(), sorted.end(), [](auto & a, auto & b) { return a.first > b.first; });

                for (size_t k = 0; k < sorted.size() && k < 30; k++)
                    out << L"  " << sorted[k].first << L"\t" << __percent((double)sorted[k].first, (double)executed)
                        << L"\t" << sorted[k].second << L"\n";
            }

            // The sequences of xils that a command can take: those that go on one after the other and have no place that a jump or a block
            // goes to in them. The weight of one is the number of times that its first xil is executed, the saving is the dispatches
            // that one command takes the place of (the length minus one) for each of them.
            out << L"\n-- sequences of xils (2 - 4, they overlap: read the simulation below for the total) by the dispatches that a command for "
                   L"each would save: saved, times, length, xils\n";
            {
                const size_t max_length = 4;
                std::map<string_t, std::pair<uint64_t, size_t>> windows;
                uint64_t blocked[max_length + 1] = { 0 }, allowed[max_length + 1] = { 0 };

                for (__method_t * method : __methods)
                {
                    size_t size = method->shapes.size();

                    for (size_t k = 0; k + 1 < size; k++)
                    {
                        if (method->counts[k] == 0)
                            continue;

                        string_t key;
                        uint64_t weight = method->counts[k];
                        bool cut = false;

                        for (size_t n = 1; n <= max_length && k + n <= size; n++)
                        {
                            size_t last = k + n - 1;

                            // The xil that is added is the last of the sequence: the one before it must go on to it.
                            if (n >= 2 && __changes_flow(method->shapes[last - 1]))
                                break;

                            if (n >= 2 && method->cuts[last])
                                cut = true;

                            weight = std::min(weight, method->counts[last]);

                            if (n > 1)
                                key += L"  ;  ";
                            key += method->shapes[last];

                            if (n < 2)
                                continue;

                            if (cut)
                            {
                                blocked[n] += weight;
                                continue;
                            }

                            allowed[n] += weight;
                            auto & entry = windows[key];
                            entry.first += weight;
                            entry.second = n;
                        }
                    }
                }

                std::vector<std::pair<uint64_t, string_t>> sorted;
                for (auto && it : windows)
                    sorted.push_back({ it.second.first * (it.second.second - 1), it.first });

                std::sort(sorted.begin(), sorted.end(), [](auto & a, auto & b) { return a.first > b.first; });

                for (size_t k = 0; k < sorted.size() && k < 30; k++)
                {
                    auto & entry = windows[sorted[k].second];
                    out << L"  " << sorted[k].first << L"\t" << entry.first << L"\t" << entry.second << L"\t" << sorted[k].second << L"\n";
                }

                out << L"\n-- the sequences that a place of a jump or a block cuts: of the executions of the sequences of each length, "
                       L"the share that has such a place in it\n";
                for (size_t n = 2; n <= max_length; n++)
                {
                    out << L"  length " << n << L":\t" << __percent((double)blocked[n], (double)(blocked[n] + allowed[n])) << L"\n";
                }
            }

            // The simulation: the superinstructions that are tried, the longest first, as the matcher would join the xils of each method.
            out << L"\n-- simulation of the superinstructions (what a matcher would do, greedy from the first xil): uses, dispatches saved, pattern\n";
            {
                std::vector<__pattern_t> with_cuts = __patterns(), without_cuts = __patterns();
                uint64_t saved_with = 0, saved_without = 0, saved_loops = 0;

                for (__method_t * method : __methods)
                {
                    uint64_t saved = __simulate(method, with_cuts, true);
                    saved_with += saved;
                    if (method->has_loop)
                        saved_loops += saved;

                    saved_without += __simulate(method, without_cuts, false);
                }

                std::vector<__pattern_t *> sorted;
                for (__pattern_t & pattern : with_cuts)
                    sorted.push_back(&pattern);

                std::sort(sorted.begin(), sorted.end(), [](__pattern_t * a, __pattern_t * b) { return a->saved > b->saved; });

                for (__pattern_t * pattern : sorted)
                {
                    if (pattern->uses == 0)
                        continue;

                    out << L"  " << pattern->uses << L"\t" << pattern->saved << L"\t" << pattern->name << L"\n";
                }

                out << L"  commands executed now: " << executed << L", after the superinstructions: " << (executed - saved_with)
                    << L"  (saved " << __percent((double)saved_with, (double)executed) << L", " << __percent((double)saved_loops, (double)saved_with)
                    << L" of it in methods that have a loop)\n";
                out << L"  if the places of jumps and blocks did not cut: saved " << __percent((double)saved_without, (double)executed) << L"\n";
            }

            out.flush();
        }

    }   // anonymous namespace

    // Registers a method that was parsed.
    uint64_t * exec_profile_register(const string_t & name, std::vector<string_t> && shapes,
                std::vector<uint8_t> && cuts, bool has_loop, uint64_t parse_ns, uint64_t cut_ns)
    {
        std::lock_guard<std::mutex> guard(__lock);

        if (!__registered)
        {
            __registered = true;
            std::atexit(__report);
        }

        __method_t * method = new __method_t();
        method->name = name;
        method->shapes = std::move(shapes);
        method->cuts = std::move(cuts);
        method->counts.assign(method->shapes.size() + 1, 0);
        method->has_loop = has_loop;

        __methods.push_back(method);
        __parse_ns += parse_ns;
        __cut_ns += cut_ns;
        __xil_count += method->shapes.size();

        return method->counts.data();
    }

}   // namespace X_ROOT_NS::modules::exec
