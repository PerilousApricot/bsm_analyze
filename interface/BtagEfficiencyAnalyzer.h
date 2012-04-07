// Measure b-tagging efficiency
//
// Created by Samvel Khalatyan, Apr 04, 2012
// Copyright 2011, All rights reserved

#ifndef BTAG_EFFICIENCY_ANALYZER
#define BTAG_EFFICIENCY_ANALYZER

#include <boost/shared_ptr.hpp>

#include "bsm_stat/interface/bsm_stat_fwd.h"
#include "interface/bsm_fwd.h"
#include "interface/Analyzer.h"

namespace bsm
{
    class BtagEfficiencyAnalyzer: public Analyzer
    {
        public:
            BtagEfficiencyAnalyzer();
            BtagEfficiencyAnalyzer(const BtagEfficiencyAnalyzer &);

            // Delegates
            //
            JetEnergyCorrectionDelegate *getJetEnergyCorrectionDelegate() const;
            SynchSelectorDelegate *getSynchSelectorDelegate() const;
            TriggerDelegate *getTriggerDelegate() const;
            PileupDelegate *getPileupDelegate() const;

            // Accessors
            //
            const stat::H2Ptr parton_jets() const;
            const stat::H2Ptr btagged_parton_jets() const;

            // Analyzer interface
            //
            virtual void onFileOpen(const std::string &filename, const Input *);
            virtual void process(const Event *);

            // Object interface
            //
            virtual uint32_t id() const;
            virtual ObjectPtr clone() const;
            virtual void print(std::ostream &) const;

        private:
            typedef boost::shared_ptr<H2Proxy> H2ProxyPtr;

            boost::shared_ptr<SynchSelector> _synch_selector;

            H2ProxyPtr _parton_jets;
            H2ProxyPtr _btagged_parton_jets;

            boost::shared_ptr<Btag> _btag;
            boost::shared_ptr<Pileup> _pileup;

            bool _use_pileup;
            float _pileup_weight;
    };
}

#endif
