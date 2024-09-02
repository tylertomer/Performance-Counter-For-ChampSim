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

class performance_counter : public EventListener {
    std::unordered_map<std::string, uint64_t> cache_access;
    std::unordered_map<std::string, uint64_t> cache_misses;
    std::unordered_map<std::string, uint64_t> cache_stores;
    std::unordered_map<std::string, uint64_t> cache_loads;
    std::unordered_map<std::string, uint64_t> cache_store_misses;
    std::unordered_map<std::string, uint64_t> cache_load_misses;
    bool in_warmup = true;

    long cycles =0;
    long print_cycles = 10000;
    long num_cycles = 0;
    
    public:
     void process_event(event eventType, void* data) {
        if (eventType == event::BEGIN_PHASE) {
            BEGIN_PHASE_data* b_data = static_cast<BEGIN_PHASE_data *>(data);
            in_warmup = b_data->is_warmup;
            return;
        }
        if (in_warmup) {
            return;
        }
        num_cycles++;
        cycles++;
        if(eventType ==  event::PRE_CYCLE){
            if(cycles >= print_cycles){
                cycles = 0;
                fmt::print("lifetime Cycles: {}\n", num_cycles);
                fmt::print("Overall Cache Stats: \n");
                fmt::print("LLC_accesses:{} LLC_misses: {}\n", cache_access["LLC"], cache_misses["LLC"]);
                fmt::print("L1I_accesses:{} L1I_misses: {}\n", cache_access["L1I"], cache_misses["L1I"]);
                fmt::print("L1D_accesses:{} L1D_misses: {}\n", cache_access["L1D"], cache_misses["L1D"]);
                fmt::print("L2C_accesses:{} L2C_misses: {}\n", cache_access["L2C"], cache_misses["L2C"]);
                fmt::print("STLB_accesses:{} STLB_misses: {}\n", cache_access["STLB"], cache_misses["STLB"]);
                fmt::print("iTLB_accesses:{} iTLB_misses: {}\n", cache_access["iTLB"], cache_misses["iTLB"]);
                fmt::print("dTLB_accesses:{} STLB_misses: {}\n", cache_access["dTLB"], cache_misses["dTLB"]);
                fmt::print("Cache Store Stats: \n");
                fmt::print("LLC_stores:{} LLC_store_misses: {}\n", cache_stores["LLC"], cache_store_misses["LLC"]);
                fmt::print("L1I_stores:{} L1I_store_misses: {}\n", cache_stores["L1I"], cache_store_misses["L1I"]);
                fmt::print("L1D_stores:{} L1D_store_misses: {}\n", cache_stores["L1D"], cache_store_misses["L1D"]);
                fmt::print("L2C_stores:{} L2C_store_misses: {}\n", cache_stores["L2C"], cache_store_misses["L2C"]);
                fmt::print("STLB_stores:{} STLB_store_misses: {}\n", cache_stores["STLB"], cache_store_misses["STLB"]);
                fmt::print("iTLB_stores:{} iTLB_store_misses: {}\n", cache_stores["iTLB"], cache_store_misses["iTLB"]);
                fmt::print("dTLB_stores:{} STLB_store_misses: {}\n", cache_stores["dTLB"], cache_store_misses["dTLB"]);
                fmt::print("Cache Load Stats: \n");
                fmt::print("LLC_loads:{} LLC_load_misses: {}\n", cache_loads["LLC"], cache_load_misses["LLC"]);
                fmt::print("L1I_loads:{} L1I_load_misses: {}\n", cache_loads["L1I"], cache_load_misses["L1I"]);
                fmt::print("L1D_loads:{} L1D_load_misses: {}\n", cache_loads["L1D"], cache_load_misses["L1D"]);
                fmt::print("L2C_loads:{} L2C_load_misses: {}\n", cache_loads["L2C"], cache_load_misses["L2C"]);
                fmt::print("STLB_loads:{} STLB_load_misses: {}\n", cache_loads["STLB"], cache_load_misses["STLB"]);
                fmt::print("iTLB_loads:{} iTLB_load_misses: {}\n", cache_loads["iTLB"], cache_load_misses["iTLB"]);
                fmt::print("dTLB_loads:{} STLB_load_misses: {}\n", cache_loads["dTLB"], cache_load_misses["dTLB"]);
            }
        }

        if (eventType == event::CACHE_TRY_HIT){
            CACHE_TRY_HIT_data* c_data = static_cast<CACHE_TRY_HIT_data*>(data);
            if(c_data->NAME == "LLC"){
                cache_access["LLC"]++;
                if (!c_data->hit){
                    cache_misses["LLC"]++;
                }
                if(c_data->type == "WRITE"){
                    cache_stores["LLC"]++;
                }
                else if(c_data-> == "LOAD"){
                    cache_loads["LLC"]++;
                }
            }
            else if(c_data->NAME == "L1I"){
                cache_access["L1I"]++;
                if(!c_data->hit){
                    cache_misses["L1I"]++;
                }
                if(c_data->type == "WRITE"){
                    cache_stores["L1I"]++;
                }
                else if(c_data-> == "LOAD"){
                    cache_loads["L1I"]++;
                }
            }
            else if(c_data->NAME == "L1D"){
                cache_access["L1D"]++;
                if(!c_data->hit){
                    cache_misses["L1D"]++;
                }
                if(c_data->type == "WRITE"){
                    cache_stores["L1D"]++;
                }
                else if(c_data-> == "LOAD"){
                    cache_loads["L1D"]++;
                }
            }
            else if(c_data->NAME == "L2C"){
                cache_access["L2C"]++;
                if(!c_data->hit){
                    cache_misses["L2C"]++;
                }
                if(c_data->type == "WRITE"){
                    cache_stores["L2C"]++;
                }
                else if(c_data-> == "LOAD"){
                    cache_loads["L2C"]++;
                }
            }
            else if(c_data->NAME == "STLB"){
                cache_access["STLB"]++;
                if(!c_data->hit){
                    cache_misses["STLB"]++;
                }
                if(c_data->type == "WRITE"){
                    cache_stores["L2C"]++;
                }
                else if(c_data-> == "LOAD"){
                    cache_loads["L2C"]++;
                }
            }
            else if(c_data->NAME == "iTLB"){
                cache_access["iTLB"]++;
                if(!c_data->hit){
                    cache_misses["iTLB"]++;
                }
                if(c_data->type == "WRITE"){
                    cache_stores["iTLB"]++;
                }
                else if(c_data-> == "LOAD"){
                    cache_loads["iTLB"]++;
                }
            }
            else if(c_data->NAME == "dTLB"){
                cache_access["dTLB"]++;
                if(!c_data->hit){
                    cache_misses["dTLB"]++;
                }
                if(c_data->type == "WRITE"){
                    cache_stores["dTLB"]++;
                }
                else if(c_data-> == "LOAD"){
                    cache_loads["dTLB"]++;
                }
            }

        }

        if(eventType == CACHE_HANDLE_MISSES_data){
            if(c_data->type == "WRITE"){
                if(c_data->NAME == "LLC"){
                    cache_store_misses["LLC"]++;
                }
                else if(c_data->NAME == "L1I"){
                    cache_store_misses["L1I"]++;
                }
                else if(c_data->NAME == "L1D"){
                    cache_store_misses["L1D"]++;
                }
                else if(c_data->NAME == "L2C"){
                    cache_store_misses["L2C"]++;
                }
                else if(c_data->NAME == "STLB"){
                    cache_store_misses["STLB"]++;
                }
                else if(c_data->NAME == "iTLB"){
                    cache_store_misses["iTLB"]++;
                }
                else if(c_data->NAME == "dTLB"){
                    cache_store_misses["dTLB"]++;
                }
            }
            else if(c_data-> == "LOAD"){
                if(c_data->NAME == "LLC"){
                    cache_load_misses["LLC"]++;
                }
                else if(c_data->NAME == "L1I"){
                    cache_load_misses["L1I"]++;
                }
                else if(c_data->NAME == "L1D"){
                    cache_load_misses["L1D"]++;
                }
                else if(c_data->NAME == "L2C"){
                    cache_load_misses["L2C"]++;
                }
                else if(c_data->NAME == "STLB"){
                    cache_load_misses["STLB"]++;
                }
                else if(c_data->NAME == "iTLB"){
                    cache_load_misses["iTLB"]++;
                }
                else if(c_data->NAME == "dTLB"){
                    cache_load_misses["dTLB"]++;
                }
            }
        }

    }

    
};

#endif

#ifdef SET_ASIDE_CHAMPSIM_MODULE
#undef SET_ASIDE_CHAMPSIM_MODULE
#define CHAMPSIM_MODULE
#endif