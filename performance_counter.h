#ifdef CHAMPSIM_MODULE
#define SET_ASIDE_CHAMPSIM_MODULE
#undef CHAMPSIM_MODULE
#endif

#ifndef PERFORMANCE_COUNTER_H
#define PERFORMANCE_COUNTER_H

#include "event_listener.h"
#include "util/algorithm.h"

#include <fmt/chrono.h>
#include <fmt/core.h>
#include <fmt/ranges.h>

#include <iostream>
#include <cstdint>
#include <unordered_map>

class performance_counter : public EventListener
{
    std::unordered_map<std::string, uint64_t> cache_access;
    std::unordered_map<std::string, uint64_t> cache_misses;
    std::unordered_map<std::string, uint64_t> cache_stores;
    std::unordered_map<std::string, uint64_t> cache_loads;
    std::unordered_map<std::string, uint64_t> cache_store_misses;
    std::unordered_map<std::string, uint64_t> cache_load_misses;
    std::unordered_map<std::string, uint64_t> num_branches_type;
    std::unordered_map<std::string, uint64_t> num_misses_branch_type;
    std::unordered_map<std::string, uint64_t> retired_instr_type_count;
    bool in_warmup = true;

    long cycles = 0;
    long print_cycles = 10000;
    long num_cycles = 0;
    long num_branches = 0;

public:
    void process_event(event eventType, void *data)
    {
        if (eventType == event::BEGIN_PHASE)
        {
            BEGIN_PHASE_data *b_data = static_cast<BEGIN_PHASE_data *>(data);
            in_warmup = b_data->is_warmup;
            return;
        }
        if (in_warmup)
        {
            return;
        }
        if (eventType == event::PRE_CYCLE)
        {
            cycles++;
            num_cycles++;
            if (cycles >= print_cycles)
            {
                cycles = 0;
                fmt::print("Performance Check\n");
                fmt::print("Lifetime Cycles: {}\n", num_cycles);
                fmt::print("Overall Cache Stats:\n");
                for (const auto &[cache, count] : cache_access)
                {
                    auto miss_it = cache_misses.find(cache);
                    if (miss_it != cache_misses.end())
                    {
                        fmt::print("{} accesses: {} {} misses: {}\n", cache, count, cache, miss_it->second);
                    }
                    else
                    {
                        fmt::print("{} accesses: {} (No miss data available)\n", cache, count);
                    }
                }
                fmt::print("Branch Stats:\n");
                for (const auto &[branch, count] : num_branches_type)
                {
                    auto miss_it = num_misses_branch_type.find(branch);
                    if (miss_it != num_misses_branch_type.end())
                    {
                        fmt::print("{} instructions: {} {} misses: {}\n", branch, count, branch, miss_it->second);
                    }
                    else
                    {
                        fmt::print("{}: {} (No miss data available)\n", branch, count);
                    }
                }
                fmt::print("Retire Instructions:\n");
                for (const auto &[instruction, count] : retired_instr_type_count)
                {
                    fmt::print("{}: {}\n", instruction, count);
                }
            }
        }

        if (eventType == event::CACHE_TRY_HIT)
        {
            CACHE_TRY_HIT_data *c_data = static_cast<CACHE_TRY_HIT_data *>(data);
            cache_access[c_data->NAME]++;
            if (!c_data->hit)
            {
                cache_misses[c_data->NAME]++;
            }
            if (c_data->type == access_type::WRITE)
            {
                cache_stores[c_data->NAME]++;
                if (!c_data->hit)
                {
                    cache_store_misses[c_data->NAME]++;
                }
            }
            else if (c_data->type == access_type::LOAD)
            {
                cache_loads[c_data->NAME]++;
                if (!c_data->hit)
                {
                    cache_load_misses[c_data->NAME]++;
                }
            }
        }

        if (eventType == event::BRANCH)
        {
            num_branches++;
            BRANCH_data *b_data = static_cast<BRANCH_data *>(data);
            num_branches_type[branch_type_to_string(b_data->instr->branch)]++;
            if (b_data->instr->branch_mispredicted)
            {
                num_misses_branch_type[branch_type_to_string(b_data->instr->branch)]++;
            }
        }

        if (eventType == event::RETIRE)
        {
            RETIRE_data *r_data = static_cast<RETIRE_data *>(data);
            for (auto it = r_data->begin; it != r_data->end; ++it)
            {
                const auto &instr = *it;
                if (instr.is_branch)
                {
                    retired_instr_type_count["Branches"]++;
                }
                else if (!instr.source_registers.empty() && !instr.destination_registers.empty() && instr.source_memory.empty() && instr.destination_memory.empty())
                {
                    retired_instr_type_count["ALU"]++;
                }
                else if (!instr.destination_memory.empty() && !instr.source_registers.empty() && instr.source_memory.empty() && instr.destination_registers.empty())
                {
                    retired_instr_type_count["Stores"]++;
                }
                else if (!instr.source_memory.empty() && !instr.destination_registers.empty() && instr.destination_memory.empty())
                {
                    retired_instr_type_count["Loads"]++;
                }
                else
                {
                    retired_instr_type_count["Other"]++;
                }
            }
        }
    }

    std::string branch_type_to_string(branch_type type)
    {
        switch (type)
        {
        case BRANCH_DIRECT_JUMP:
            return "BRANCH_DIRECT_JUMP";
        case BRANCH_INDIRECT:
            return "BRANCH_INDIRECT";
        case BRANCH_CONDITIONAL:
            return "BRANCH_CONDITIONAL";
        case BRANCH_DIRECT_CALL:
            return "BRANCH_DIRECT_CALL";
        case BRANCH_INDIRECT_CALL:
            return "BRANCH_INDIRECT_CALL";
        case BRANCH_RETURN:
            return "BRANCH_RETURN";
        case BRANCH_OTHER:
            return "BRANCH_OTHER";
        default:
            return "error";
        }
    }
};

#endif

#ifdef SET_ASIDE_CHAMPSIM_MODULE
#undef SET_ASIDE_CHAMPSIM_MODULE
#define CHAMPSIM_MODULE
#endif
