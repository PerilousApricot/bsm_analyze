// Extract Trigger and associated filters
//
// Created by Samvel Khalatyan, Oct 07, 2011
// Copyright 2011, All rights reserved

#ifndef BSM_TRIGGER_WITH_FILTERS
#define BSM_TRIGGER_WITH_FILTERS

#include <stdint.h>
#include <map>
#include <string>
#include <sstream>

#include "interface/Analyzer.h"
#include "interface/TriggerAnalyzer.h"
#include "interface/bsm_fwd.h"

namespace bsm
{
    class TriggerWithFilterAnalyzer: public Analyzer,
        public TriggerDelegate
    {
        public:
            TriggerWithFilterAnalyzer();
            TriggerWithFilterAnalyzer(const TriggerWithFilterAnalyzer &);

            // Trigger Delegate interface
            //
            virtual void setTrigger(const Trigger &);

            // Analyzer interface
            //
            virtual void onFileOpen(const std::string &filename, const Input *);
            virtual void process(const Event *);

            // Object interface
            //
            virtual uint32_t id() const;

            virtual ObjectPtr clone() const;
            virtual void merge(const ObjectPtr &);

            virtual void print(std::ostream &) const;

        private:
            typedef std::map<Hash, std::string> HashTable;

            HashTable _trigger_map;
            HashTable _filter_map;

            std::ostringstream _out;

            Trigger _trigger;
    };
}

#endif
