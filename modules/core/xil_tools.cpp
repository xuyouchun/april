
#include <core.h>
#include <lib.h>

#include <cstdlib>
#include <iostream>
#include <map>
#include <set>
#include <vector>

namespace X_ROOT_NS::modules::core {

    // Converts xil to a string (xil.cpp).
    string_t __to_string(const xil_t * xil) _NE;

    ////////// ////////// ////////// ////////// //////////
    // The environment

    namespace {

        // The value of an environment variable; null when it is not set, or is empty.
        const char * __env(const char * name)
        {
            const char * value = std::getenv(name);
            return value != nullptr && *value != '\0'? value : nullptr;
        }

        // Converts a string to a narrow one, for the comparison with the environment.
        std::string __narrow_text(const string_t & text)
        {
            std::string result;
            result.reserve(text.size());

            for (auto c : text)
                result += (char)(c < 128? c : '?');

            return result;
        }

        // The text of a xil, without the numbers: the key that the xils of the same kind have (push 3 and push 7 are one).
        string_t __shape_of(const xil_t * xil)
        {
            string_t text = __to_string(xil);
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
        // The counts of the xils of the process

        struct __stats_t
        {
            int                         level = 0;      // 1: the commands, 2: the pairs too.
            size_t                      methods = 0;
            size_t                      total = 0;
            std::map<string_t, size_t>  shapes;
            std::map<string_t, size_t>  pairs;

            ~__stats_t()
            {
                if (level == 0)
                    return;

                std::wcerr << L"\n== xil stats: " << methods << L" methods, " << total << L" xils\n";

                auto print = [](const std::map<string_t, size_t> & counts, size_t limit) {
                    std::vector<std::pair<size_t, string_t>> sorted;
                    for (auto && it : counts)
                        sorted.push_back({ it.second, it.first });

                    std::sort(sorted.begin(), sorted.end(), [](auto & a, auto & b) { return a.first > b.first; });

                    for (size_t k = 0; k < sorted.size() && k < limit; k++)
                        std::wcerr << L"  " << sorted[k].first << L"\t" << sorted[k].second << L"\n";
                };

                std::wcerr << L"-- the xils (by the kind)\n";
                print(shapes, 40);

                if (level >= 2)
                {
                    std::wcerr << L"-- the pairs of xils that follow each other\n";
                    print(pairs, 40);
                }

                std::wcerr.flush();
            }
        };

        __stats_t & __stats()
        {
            static __stats_t stats;
            return stats;
        }

        // Counts the xils of a method.
        void __count(xil_pool_t & pool, int level)
        {
            __stats_t & stats = __stats();
            stats.level = std::max(stats.level, level);
            stats.methods++;

            string_t previous;
            for (xil_t * xil : pool)
            {
                string_t shape = __shape_of(xil);

                stats.total++;
                stats.shapes[shape]++;

                if (level >= 2 && !previous.empty())
                    stats.pairs[previous + L"  ;  " + shape]++;

                previous = shape;
            }
        }

        ////////// ////////// ////////// ////////// //////////
        // The structure of the xils

        // Checks the jumps of a method: a jump goes to a xil of the method, or to the end of it.
        void __verify(method_t * method, xil_pool_t & pool)
        {
            long long size = (long long)pool.size();

            for (xil_t * xil : pool)
            {
                if (command_of(xil) != xil_command_t::jmp)
                    continue;

                jmp_xil_t * jmp = (jmp_xil_t *)xil;
                if (jmp->model() == xil_jmp_model_t::switch_)
                    continue;

                long long target = (long long)pool.index_of(xil) + (long long)jmp->step();
                if (target < 0 || target > size)
                {
                    throw _EC(unexpected, _F(_T("xil verify: the jump at %1% of the method %2% goes to %3%, out of 0 - %4%"),
                                                pool.index_of(xil), method, target, size));
                }
            }
        }

    }   // anonymous namespace

    ////////// ////////// ////////// ////////// //////////
    // The tools

    // The tools to see the xils of a method.
    void xil_debug(method_t * method, xil_pool_t & pool)
    {
        static const char * dump  = __env("APRIL_DUMP_XIL");
        static const char * stats = __env("APRIL_XIL_STATS");
        static const char * check = __env("APRIL_VERIFY_XIL");

        if (dump == nullptr && stats == nullptr && check == nullptr)
            return;

        if (check != nullptr)
            __verify(method, pool);

        if (stats != nullptr)
            __count(pool, stats[0] == '2'? 2 : 1);

        if (dump != nullptr)
        {
            string_t name = _str(method);
            std::string narrow = __narrow_text(name);

            if (dump[0] != '*' || dump[1] != '\0')
            {
                if (narrow.find(dump) == std::string::npos)
                    return;
            }

            std::wcerr << L"\n== " << name << L"  (" << pool.size() << L" xils)\n";

            for (xil_t * xil : pool)
            {
                xil_index_t index = pool.index_of(xil);

                std::wstring line = __to_string(xil);
                if (command_of(xil) == xil_command_t::jmp && ((jmp_xil_t *)xil)->model() != xil_jmp_model_t::switch_)
                    line += L"  -> " + std::to_wstring((long long)index + (long long)((jmp_xil_t *)xil)->step());

                std::wcerr << L"  " << index << L"\t" << line << L"\n";
            }

            std::wcerr.flush();
        }
    }

    ////////// ////////// ////////// ////////// //////////
    // The passes

    namespace {

        // Whether the xil is a jump that always goes, to a label of the method.
        bool __is_goto(xil_t * xil)
        {
            return command_of(xil) == xil_command_t::jmp && ((jmp_xil_t *)xil)->model() == xil_jmp_model_t::none;
        }

        // Whether the xil is a jump to a label of the method: it always goes, or goes when a condition is true or false (not a
        // switch, and not the leave of a protected block).
        bool __is_local_jump(xil_t * xil)
        {
            if (command_of(xil) != xil_command_t::jmp)
                return false;

            xil_jmp_model_t model = ((jmp_xil_t *)xil)->model();
            return model == xil_jmp_model_t::none || model == xil_jmp_model_t::true_ || model == xil_jmp_model_t::false_;
        }

        ////////// ////////// ////////// ////////// //////////
        // The constants

        // Whether the type is an integer that the passes know.
        bool __is_int_type(xil_type_t type)
        {
            switch (type)
            {
                case xil_type_t::int8:   case xil_type_t::uint8:
                case xil_type_t::int16:  case xil_type_t::uint16:
                case xil_type_t::int32:  case xil_type_t::uint32:
                case xil_type_t::int64:  case xil_type_t::uint64:
                    return true;

                default:
                    return false;
            }
        }

        bool __is_unsigned_type(xil_type_t type)
        {
            return type == xil_type_t::uint8 || type == xil_type_t::uint16
                || type == xil_type_t::uint32 || type == xil_type_t::uint64;
        }

        // Whether the xil pushes a constant of an integer type.
        bool __is_int_constant(xil_t * xil)
        {
            if (command_of(xil) != xil_command_t::push)
                return false;

            push_xil_t * push = (push_xil_t *)xil;
            return push->stype() == xil_storage_type_t::constant && __is_int_type(push->dtype());
        }

        // The value of a constant of an integer type.
        __int128 __int_value(push_xil_t * push)
        {
            switch (push->dtype())
            {
                case xil_type_t::int8:   return push->get_extra<int8_t>();
                case xil_type_t::uint8:  return push->get_extra<uint8_t>();
                case xil_type_t::int16:  return push->get_extra<int16_t>();
                case xil_type_t::uint16: return push->get_extra<uint16_t>();
                case xil_type_t::int32:  return push->get_extra<int32_t>();
                case xil_type_t::uint32: return push->get_extra<uint32_t>();
                case xil_type_t::int64:  return push->get_extra<int64_t>();
                default:                 return push->get_extra<uint64_t>();
            }
        }

        // Makes a xil a constant of an integer type: the value is cut to the type, as the virtual machine does.
        void __set_int_constant(xil_t * xil, xil_type_t type, __int128 value)
        {
            auto * push = (xil_extra_t<push_xil_t> *)xil;
            push->set_dtype(type);

            switch (type)
            {
                case xil_type_t::int8:   push->set_extra((int8_t)value);   break;
                case xil_type_t::uint8:  push->set_extra((uint8_t)value);  break;
                case xil_type_t::int16:  push->set_extra((int16_t)value);  break;
                case xil_type_t::uint16: push->set_extra((uint16_t)value); break;
                case xil_type_t::int32:  push->set_extra((int32_t)value);  break;
                case xil_type_t::uint32: push->set_extra((uint32_t)value); break;
                case xil_type_t::int64:  push->set_extra((int64_t)value);  break;
                default:                 push->set_extra((uint64_t)value); break;
            }
        }

        // The xils that follow a xil, when no label is at them (a label at a xil: another way comes to it, with another stack).
        bool __joined(xil_pool_t & pool, const std::set<xil_index_t> & labels, size_t index, size_t count)
        {
            if (index + count > pool.size())
                return false;

            for (size_t k = index + 1; k < index + count; k++)
            {
                if (labels.count((xil_index_t)k) != 0)
                    return false;
            }

            return true;
        }

        // push constant, convert  =>  push constant of the new type.
        // push constant, push constant, add/sub/mul/div/mod  =>  push constant.
        // push constant (bool), jmp true/false  =>  jmp, or nothing.
        bool __fold_constants(statement_compile_context_t & sctx, xil_pool_t & pool)
        {
            std::set<xil_index_t> labels;
            sctx.jmp_manager.each_label([&](xil_t * label) { labels.insert(pool.index_of(label)); });

            std::set<xil_t *> removed;
            bool changed = false;

            for (size_t k = 0; k + 1 < pool.size(); k++)
            {
                xil_t * xil = pool[k];
                if (removed.count(xil) != 0 || command_of(xil) != xil_command_t::push)
                    continue;

                push_xil_t * push = (push_xil_t *)xil;
                if (push->stype() != xil_storage_type_t::constant)
                    continue;

                xil_t * next = pool[k + 1];

                // push constant (bool), jmp true / false
                if (push->dtype() == xil_type_t::bool_ && command_of(next) == xil_command_t::jmp && __joined(pool, labels, k, 2))
                {
                    jmp_xil_t * jmp = (jmp_xil_t *)next;
                    bool value = push->get_extra<uint8_t>() != 0;

                    if (jmp->model() == xil_jmp_model_t::true_ || jmp->model() == xil_jmp_model_t::false_)
                    {
                        bool goes = (jmp->model() == xil_jmp_model_t::true_) == value;

                        removed.insert(xil);
                        if (goes)
                            jmp->set_model(xil_jmp_model_t::none);
                        else
                        {
                            sctx.jmp_manager.forget(jmp);
                            removed.insert(next);
                        }

                        changed = true;
                        k++;
                        continue;
                    }
                }

                if (!__is_int_type(push->dtype()))
                    continue;

                // push constant, convert
                if (command_of(next) == xil_command_t::push && ((push_xil_t *)next)->stype() == xil_storage_type_t::convert
                        && ((push_xil_t *)next)->dtype() == push->dtype() && __joined(pool, labels, k, 2))
                {
                    xil_type_t to = ((push_xil_t *)next)->dtype2();
                    __int128 value = __int_value(push);
                    bool from_unsigned = __is_unsigned_type(push->dtype());

                    if (__is_int_type(to))
                    {
                        __set_int_constant(xil, to, value);
                    }
                    else if (to == xil_type_t::double_)
                    {
                        auto * target = (xil_extra_t<push_xil_t> *)xil;
                        target->set_dtype(to);
                        target->set_extra(from_unsigned? (double)(uint64_t)value : (double)(int64_t)value);
                    }
                    else if (to == xil_type_t::float_)
                    {
                        auto * target = (xil_extra_t<push_xil_t> *)xil;
                        target->set_dtype(to);
                        target->set_extra(from_unsigned? (float)(uint64_t)value : (float)(int64_t)value);
                    }
                    else
                    {
                        continue;
                    }

                    removed.insert(next);
                    changed = true;
                    k++;
                    continue;
                }

                // push constant, push constant, al
                if (k + 2 < pool.size() && __is_int_constant(next) && __joined(pool, labels, k, 3))
                {
                    xil_t * al = pool[k + 2];
                    push_xil_t * second = (push_xil_t *)next;

                    if (command_of(al) != xil_command_t::al || second->dtype() != push->dtype())
                        continue;

                    xil_type_t type = push->dtype();
                    al_xil_t * al_xil = (al_xil_t *)al;
                    if (al_xil->dtype1() != type || al_xil->dtype2() != type)
                        continue;

                    // The types that the virtual machine calculates as they are (the smaller ones are widened first).
                    if (type != xil_type_t::int32 && type != xil_type_t::uint32 && type != xil_type_t::int64 && type != xil_type_t::uint64)
                        continue;

                    __int128 a = __int_value(push), b = __int_value(second), result = 0;
                    switch (al_xil->cmd())
                    {
                        case xil_al_command_t::add: result = a + b; break;
                        case xil_al_command_t::sub: result = a - b; break;
                        case xil_al_command_t::mul: result = a * b; break;

                        case xil_al_command_t::div:
                        case xil_al_command_t::mod:
                            // Not by zero, and not the one that overflows (the smallest of a signed type by -1).
                            if (b == 0 || (!__is_unsigned_type(type) && b == -1))
                                continue;

                            result = al_xil->cmd() == xil_al_command_t::div? a / b : a % b;
                            break;

                        default:
                            continue;
                    }

                    __set_int_constant(xil, type, result);
                    removed.insert(next);
                    removed.insert(al);
                    changed = true;
                    k += 2;
                    continue;
                }
            }

            if (!changed)
                return false;

            pool.erase_if([&](xil_t * xil) { return removed.count(xil) != 0; });
            return true;
        }

        // A jump that goes to a jump that always goes is a jump to the place that it goes to: if a: goto b; b: goto c; then a goes to c.
        // It is true for the jumps that go when a condition is true or false too.
        bool __thread_jumps(statement_compile_context_t & sctx, xil_pool_t & pool)
        {
            jmp_manager_t & jumps = sctx.jmp_manager;
            bool changed = false;

            for (xil_t * xil : pool)
            {
                if (!__is_local_jump(xil))
                    continue;

                jmp_xil_t * jmp = (jmp_xil_t *)xil;

                local_label_t label;
                if (!jumps.label_of(jmp, &label))
                    continue;

                local_label_t final_label = label;

                for (int hops = 0; hops < 8; hops++)
                {
                    xil_t * target = jumps.label_xil(final_label);
                    if (target == nullptr)
                        break;

                    size_t index = pool.index_of(target);
                    if (index >= pool.size())
                        break;

                    xil_t * next = pool[index];
                    if (next == xil || !__is_goto(next))
                        break;

                    local_label_t next_label;
                    if (!jumps.label_of((jmp_xil_t *)next, &next_label) || next_label == final_label)
                        break;

                    final_label = next_label;
                }

                if (final_label != label)
                {
                    jumps.set_label(jmp, final_label);
                    changed = true;
                }
            }

            return changed;
        }

        // A jump that goes to the xil that follows it does nothing.
        bool __remove_jumps_to_next(statement_compile_context_t & sctx, xil_pool_t & pool)
        {
            jmp_manager_t & jumps = sctx.jmp_manager;
            std::set<xil_t *> useless;

            for (xil_t * xil : pool)
            {
                if (!__is_goto(xil))
                    continue;

                local_label_t label;
                if (!jumps.label_of((jmp_xil_t *)xil, &label))
                    continue;

                xil_t * target = jumps.label_xil(label);
                if (target != nullptr && pool.index_of(target) == pool.index_of(xil) + 1)
                    useless.insert(xil);
            }

            // The last xil stays: the labels of the end of a block go to a xil, not past the end.
            if (!pool.empty())
                useless.erase(pool[pool.size() - 1]);

            if (useless.empty())
                return false;

            for (xil_t * xil : useless)
                jumps.forget((jmp_xil_t *)xil);

            pool.erase_if([&](xil_t * xil) { return useless.count(xil) != 0; });
            return true;
        }

        // Whether no code goes on to the next xil: a jump that always goes, a return, a throw.
        bool __ends_flow(xil_t * xil)
        {
            switch (command_of(xil))
            {
                case xil_command_t::jmp:
                    return ((jmp_xil_t *)xil)->model() == xil_jmp_model_t::none;

                case xil_command_t::smp:
                    switch (sub_command_of<xil_smp_t>(xil))
                    {
                        case xil_smp_t::ret:
                        case xil_smp_t::throw_:
                        case xil_smp_t::rethrow:
                            return true;

                        default:
                            return false;
                    }

                default:
                    return false;
            }
        }

        // The code after a xil that ends the flow, up to the next place that something can go to, is never run. (Every label is a
        // place: the jumps, the switch tables and the protected blocks go to labels; the markers of the blocks are not removed.)
        bool __remove_unreachable(statement_compile_context_t & sctx, xilx_block_manager_t & blocks, xil_pool_t & pool)
        {
            // The places that something goes to: the labels of the jumps that are left, of the switch tables, of the blocks, and the
            // points of the names.
            std::set<xil_index_t> places;

            auto add_label = [&](local_label_t label) {
                if (xil_t * xil = sctx.jmp_manager.label_xil(label))
                    places.insert(pool.index_of(xil));
            };

            sctx.jmp_manager.each_jump_label(add_label);
            sctx.switch_manager.each_label(add_label);
            blocks.each_label(add_label);
            sctx.jmp_manager.each_named_point([&](xil_t * xil) { places.insert(pool.index_of(xil)); });

            std::set<xil_t *> dead;
            bool unreachable = false;

            for (xil_t * xil : pool)
            {
                if (places.count(pool.index_of(xil)) != 0)
                    unreachable = false;

                if (unreachable)
                {
                    // The marker of the end of a finally, and the jumps that are not plain ones, are kept.
                    if (command_of(xil) == xil_command_t::smp && sub_command_of<xil_smp_t>(xil) == xil_smp_t::end_finally)
                    {
                        unreachable = false;
                        continue;
                    }

                    if (command_of(xil) == xil_command_t::jmp && !__is_local_jump(xil))
                    {
                        unreachable = false;
                        continue;
                    }

                    dead.insert(xil);
                    continue;
                }

                if (__ends_flow(xil))
                    unreachable = true;
            }

            // The code that is at the end of the method is removed whole, or not at all: a label of a block that is at its end goes to a
            // xil, not past the end, and a xil that is left alone (a call that lost the values that it takes) is not code.
            if (!pool.empty() && dead.count(pool[pool.size() - 1]) != 0)
            {
                size_t first = pool.size();
                while (first > 0 && dead.count(pool[first - 1]) != 0)
                    first--;

                bool at_the_end = false;
                for (xil_index_t place : places)
                    at_the_end = at_the_end || place >= first;

                // The labels that nobody uses (the end of the statements) are at the end too: only the ones that are used count.
                if (at_the_end)
                {
                    for (size_t k = first; k < pool.size(); k++)
                        dead.erase(pool[k]);
                }
            }

            if (dead.empty())
                return false;

            for (xil_t * xil : dead)
            {
                if (command_of(xil) == xil_command_t::jmp)
                    sctx.jmp_manager.forget((jmp_xil_t *)xil);
            }

            pool.erase_if([&](xil_t * xil) { return dead.count(xil) != 0; });
            return true;
        }

    }   // anonymous namespace

    // Optimizes the xils of a method: the passes run until none of them changes them.
    bool xil_optimize_enabled(method_compile_controller_t * controller)
    {
        static const bool disabled = __env("APRIL_NO_XIL_OPT") != nullptr;
        static const bool forced = __env("APRIL_XIL_OPT") != nullptr;

        if (disabled)
            return false;

        return forced || (controller != nullptr && controller->optimize(xil_optimize_code));
    }

    void xil_optimize(statement_compile_context_t & sctx, xilx_block_manager_t & blocks, xil_pool_t & pool, bool requested)
    {
        static const bool disabled = __env("APRIL_NO_XIL_OPT") != nullptr;
        static const bool forced = __env("APRIL_XIL_OPT") != nullptr;

        if (disabled || !(requested || forced))
            return;

        for (int round = 0; round < 4; round++)
        {
            bool changed = false;

            changed |= __fold_constants(sctx, pool);
            changed |= __thread_jumps(sctx, pool);
            changed |= __remove_unreachable(sctx, blocks, pool);
            changed |= __remove_jumps_to_next(sctx, pool);

            if (!changed)
                break;
        }
    }

}   // namespace X_ROOT_NS::modules::core
