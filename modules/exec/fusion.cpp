
#include "fusion.h"
#include "exec_profile.h"
#include <lib.h>

#include <functional>
#include <mutex>
#include <unordered_map>

namespace X_ROOT_NS::modules::exec {

    using namespace core;

    ////////// ////////// ////////// ////////// //////////
    // The mode

    fusion_mode_t fusion_mode()
    {
        static const fusion_mode_t mode = []() {
            const char * value = std::getenv("APRIL_SUPER");
            if (value == nullptr || *value == '\0' || std::string(value) == "auto")
                return fusion_mode_t::automatic;

            if (std::string(value) == "eager")
                return fusion_mode_t::eager;

            return fusion_mode_t::off;
        }();

        return mode;
    }

    int fusion_threshold()
    {
        static const int threshold = []() {
            const char * value = std::getenv("APRIL_SUPER_THRESHOLD");
            int n = value != nullptr? std::atoi(value) : 0;
            return n > 0? n : 50;
        }();

        return threshold;
    }

    ////////// ////////// ////////// ////////// //////////
    // The matcher

    namespace
    {
        enum class __cls_t : uint8_t
        {
            other, push_local, push_argument, push_const, op, cmp, pop_local, jump_cond, jump, push_this, push_field, convert, pop_field,
        };

        struct __item_t
        {
            __cls_t     cls = __cls_t::other;
            xil_type_t  d1 = xil_type_t::empty, d2 = xil_type_t::empty;
        };

        bool __is_num(xil_type_t type)
        {
            switch (type)
            {
                case xil_type_t::int32: case xil_type_t::uint32:
                case xil_type_t::int64: case xil_type_t::uint64:
                case xil_type_t::float_: case xil_type_t::double_:
                    return true;

                default:
                    return false;
            }
        }

        bool __is_float(xil_type_t type)
        {
            return type == xil_type_t::float_ || type == xil_type_t::double_;
        }

        // The types of the two operands of an operation that a command can have: the same type, or an Int64 and an Int32.
        bool __is_pair(xil_type_t d1, xil_type_t d2)
        {
            return __is_num(d1) && (d1 == d2 || (d1 == xil_type_t::int64 && d2 == xil_type_t::int32));
        }

        __item_t __classify(const xil_base_t * xil)
        {
            __item_t item;

            switch (command_of(xil))
            {
                case xil_command_t::push: {
                    const push_xil_t * push = (const push_xil_t *)xil;
                    switch (push->stype())
                    {
                        case xil_storage_type_t::local:
                            if (__is_num(push->dtype()))
                                item.cls = __cls_t::push_local, item.d1 = item.d2 = push->dtype();
                            break;

                        case xil_storage_type_t::argument:
                            if (push->get_identity() == XIL_CALLING_BOTTOM_IDENTITY)
                                break;

                            if (__is_num(push->dtype()))
                                item.cls = __cls_t::push_argument, item.d1 = item.d2 = push->dtype();
                            else if (push->dtype() == xil_type_t::object)
                                item.cls = __cls_t::push_this, item.d1 = item.d2 = push->dtype();
                            break;

                        case xil_storage_type_t::constant:
                            if (__is_num(push->dtype()))
                                item.cls = __cls_t::push_const, item.d1 = item.d2 = push->dtype();
                            break;

                        case xil_storage_type_t::field:
                            if (__is_num(push->dtype()))
                                item.cls = __cls_t::push_field, item.d1 = item.d2 = push->dtype();
                            break;

                        case xil_storage_type_t::convert:
                            if (__is_num(push->dtype()) && __is_num(push->dtype2()))
                                item.cls = __cls_t::convert, item.d1 = push->dtype(), item.d2 = push->dtype2();
                            break;

                        default:
                            break;
                    }
                }   break;

                case xil_command_t::al: {
                    const al_xil_t * al = (const al_xil_t *)xil;
                    if (!__is_pair(al->dtype1(), al->dtype2()))
                        break;

                    switch (al->cmd())
                    {
                        case xil_al_command_t::add:
                        case xil_al_command_t::sub:
                        case xil_al_command_t::mul:
                        case xil_al_command_t::div:
                            item.cls = __cls_t::op, item.d1 = al->dtype1(), item.d2 = al->dtype2();
                            break;

                        case xil_al_command_t::mod:
                            // The remainder of numbers that are not integers is not an operator of the language.
                            if (!__is_float(al->dtype1()))
                                item.cls = __cls_t::op, item.d1 = al->dtype1(), item.d2 = al->dtype2();
                            break;

                        default:
                            break;
                    }
                }   break;

                case xil_command_t::bit: {
                    const bit_xil_t * bit = (const bit_xil_t *)xil;
                    if (!__is_pair(bit->dtype1(), bit->dtype2()) || __is_float(bit->dtype1()))
                        break;

                    switch (bit->cmd())
                    {
                        case xil_bit_command_t::bit_and:
                        case xil_bit_command_t::bit_or:
                        case xil_bit_command_t::bit_xor:
                        case xil_bit_command_t::left_shift:
                        case xil_bit_command_t::right_shift:
                            item.cls = __cls_t::op, item.d1 = bit->dtype1(), item.d2 = bit->dtype2();
                            break;

                        default:
                            break;
                    }
                }   break;

                case xil_command_t::cmp: {
                    const cmp_xil_t * cmp = (const cmp_xil_t *)xil;
                    if (__is_pair(cmp->dtype1(), cmp->dtype2()))
                        item.cls = __cls_t::cmp, item.d1 = cmp->dtype1(), item.d2 = cmp->dtype2();
                }   break;

                case xil_command_t::pop: {
                    const pop_xil_t * pop = (const pop_xil_t *)xil;
                    if (pop->stype() == xil_storage_type_t::local && __is_num(pop->dtype()))
                        item.cls = __cls_t::pop_local, item.d1 = item.d2 = pop->dtype();
                    else if (pop->stype() == xil_storage_type_t::field && __is_num(pop->dtype()))
                        item.cls = __cls_t::pop_field, item.d1 = item.d2 = pop->dtype();
                }   break;

                case xil_command_t::jmp: {
                    const jmp_xil_t * jmp = (const jmp_xil_t *)xil;
                    if (jmp->model() == xil_jmp_model_t::true_ || jmp->model() == xil_jmp_model_t::false_)
                        item.cls = __cls_t::jump_cond;
                    else if (jmp->model() == xil_jmp_model_t::none)
                        item.cls = __cls_t::jump;
                }   break;

                default:
                    break;
            }

            return item;
        }

        bool __is_operand(__cls_t cls)
        {
            return cls == __cls_t::push_local || cls == __cls_t::push_argument || cls == __cls_t::push_const;
        }

        fusion_operand_t __operand_kind(__cls_t cls)
        {
            return cls == __cls_t::push_local? fusion_operand_t::local
                 : cls == __cls_t::push_argument? fusion_operand_t::argument : fusion_operand_t::constant;
        }
    }

    // Finds the longest superinstruction at the beginning of the xils.
    bool fusion_match(const xil_base_t * const * xils, size_t available, size_t max_length, fusion_shape_t * shape)
    {
        size_t limit = std::min(available, max_length);
        if (limit < 2)
            return false;

        __item_t items[fusion_max_length];
        size_t count = std::min<size_t>(limit, fusion_max_length);
        for (size_t k = 0; k < count; k++)
            items[k] = __classify(xils[k]);

        auto is_op = [&](size_t k) { return k < count && (items[k].cls == __cls_t::op || items[k].cls == __cls_t::cmp); };
        auto operand = [&](size_t k) { return k < count && __is_operand(items[k].cls); };

        // [x] [y] OP|CMP [tail]: the operands are checked with the types of the operation.
        // tail: 0 none (the result is pushed), 1 pop local, 2 jump when true / false, 3 pop local and a jump that always goes.
        auto binary = [&](bool has_left, bool has_right, size_t total, uint8_t tail) -> bool {
            size_t k = 0;
            size_t left_k = (size_t)-1, right_k = (size_t)-1, op_k;

            if (has_left)
                left_k = k++;

            if (has_right)
                right_k = k++;

            op_k = k++;

            if (total > count || !is_op(op_k))
                return false;

            const __item_t & op = items[op_k];
            if ((has_left && (!operand(left_k) || items[left_k].d1 != op.d1))
                    || (has_right && (!operand(right_k) || items[right_k].d1 != op.d2)))
                return false;

            if (tail == 1 || tail == 3)
            {
                // Stored in a local: only the result of an operation, of the type of its first operand.
                if (op.cls != __cls_t::op || items[k].cls != __cls_t::pop_local || items[k].d1 != op.d1)
                    return false;

                if (tail == 3 && items[k + 1].cls != __cls_t::jump)
                    return false;
            }
            else if (tail == 2)
            {
                if (op.cls != __cls_t::cmp || items[k].cls != __cls_t::jump_cond)
                    return false;
            }

            shape->form = fusion_form_t::binary;
            shape->length = (uint8_t)total;
            shape->left = has_left? __operand_kind(items[left_k].cls) : fusion_operand_t::stack;
            shape->right = has_right? __operand_kind(items[right_k].cls) : fusion_operand_t::stack;
            shape->left_index = has_left? (int8_t)left_k : (int8_t)-1;
            shape->right_index = has_right? (int8_t)right_k : (int8_t)-1;
            shape->op_index = (int8_t)op_k;
            shape->tail_index = tail != 0? (int8_t)k : (int8_t)-1;
            shape->jump_index = tail == 3? (int8_t)(k + 1) : (int8_t)-1;
            shape->tail = tail;
            return true;
        };

        if (limit >= 5)
        {
            if (binary(true, true, 5, 3))
                return true;
        }

        if (limit >= 4)
        {
            if (binary(true, true, 4, 1) || binary(true, true, 4, 2) || binary(false, true, 4, 3))
                return true;
        }

        if (limit >= 3)
        {
            if (binary(true, true, 3, 0) || binary(false, true, 3, 1) || binary(false, true, 3, 2) || binary(false, false, 3, 3))
                return true;

            // push x, push this, pop field: this.Field = x
            if (operand(0) && items[1].cls == __cls_t::push_this && items[2].cls == __cls_t::pop_field && items[0].d1 == items[2].d1)
            {
                shape->form = fusion_form_t::store_field;
                shape->length = 3;
                shape->left = __operand_kind(items[0].cls);
                shape->left_index = 0;
                shape->right_index = 1;
                shape->op_index = 2;
                return true;
            }
        }

        if (limit >= 2)
        {
            if (binary(false, false, 2, 1) || binary(false, false, 2, 2) || binary(false, true, 2, 0))
                return true;

            // push x, push y
            if (operand(0) && operand(1))
            {
                shape->form = fusion_form_t::push2;
                shape->length = 2;
                return true;
            }

            // push x, pop local
            if (operand(0) && items[1].cls == __cls_t::pop_local && items[0].d1 == items[1].d1)
            {
                shape->form = fusion_form_t::copy;
                shape->length = 2;
                return true;
            }

            // push x, convert
            if (operand(0) && items[1].cls == __cls_t::convert && items[0].d1 == items[1].d1)
            {
                shape->form = fusion_form_t::convert;
                shape->length = 2;
                shape->left = __operand_kind(items[0].cls);
                shape->left_index = 0;
                shape->op_index = 1;
                return true;
            }

            // push this, push field
            if (items[0].cls == __cls_t::push_this && items[1].cls == __cls_t::push_field)
            {
                shape->form = fusion_form_t::this_field;
                shape->length = 2;
                return true;
            }

            // push this, pop field: this.Field = (the value that is in the stack)
            if (items[0].cls == __cls_t::push_this && items[1].cls == __cls_t::pop_field)
            {
                shape->form = fusion_form_t::store_field;
                shape->length = 2;
                shape->left = fusion_operand_t::stack;
                shape->left_index = -1;
                shape->right_index = 0;
                shape->op_index = 1;
                return true;
            }
        }

        return false;
    }

    ////////// ////////// ////////// ////////// //////////
    // The plan

    namespace
    {
        struct __key_t
        {
            const void * first;
            size_t       count;

            bool operator == (const __key_t & other) const { return first == other.first && count == other.count; }
        };

        struct __key_hash_t
        {
            size_t operator () (const __key_t & key) const
            {
                return std::hash<const void *>()(key.first) ^ (key.count * 0x9E3779B97F4A7C15ULL);
            }
        };

        std::mutex                                                          __lock;
        std::unordered_map<__key_t, const fusion_plan_t *, __key_hash_t>    __plans;
    }

    // Makes the plan of the xils of a method.
    const fusion_plan_t * fusion_plan_of(const std::vector<const xil_base_t *> & xils, exec_switch_manager_t & switches,
                const al::svector_t<method_xil_block_t, 10> & blocks, bool * has_loop)
    {
        __key_t key { xils.empty()? nullptr : (const void *)xils[0], (size_t)xils.size() };

        {
            std::lock_guard<std::mutex> guard(__lock);
            auto it = __plans.find(key);
            if (it != __plans.end())
            {
                *has_loop = it->second->has_loop;
                return it->second;
            }
        }

        fusion_plan_t * plan = new fusion_plan_t();
        std::vector<uint8_t> cuts = exec_cut_points(xils, switches, blocks, &plan->has_loop);
        plan->has_blocks = blocks.size() > 0;

        size_t count = xils.size();
        plan->index_map.assign(count + 1, 0);

        uint32_t command_index = 0;
        for (size_t k = 0; k < count;)
        {
            // The command cannot have a place that something goes to in it, but its first xil.
            size_t max_length = fusion_max_length;
            for (size_t j = 1; j < max_length && k + j <= count; j++)
            {
                if (cuts[k + j])
                {
                    max_length = j;
                    break;
                }
            }

            fusion_shape_t shape;
            if (max_length >= 2 && fusion_match(xils.data() + k, count - k, max_length, &shape))
            {
                plan->groups.push_back({ (uint32_t)k, shape.length });

                for (size_t j = 0; j < shape.length; j++)
                    plan->index_map[k + j] = command_index;

                k += shape.length;
            }
            else
            {
                plan->index_map[k] = command_index;
                k++;
            }

            command_index++;
        }

        plan->index_map[count] = command_index;

        {
            std::lock_guard<std::mutex> guard(__lock);
            auto it = __plans.find(key);
            if (it != __plans.end())
            {
                delete plan;
                plan = const_cast<fusion_plan_t *>(it->second);
            }
            else
            {
                __plans[key] = plan;
            }
        }

        *has_loop = plan->has_loop;
        return plan;
    }

}   // namespace X_ROOT_NS::modules::exec
