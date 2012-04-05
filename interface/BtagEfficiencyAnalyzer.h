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
#include "interface/SynchSelector.h"

namespace bsm
{
    namespace gen
    {
        struct MatchedPartons
        {
            typedef SynchSelector::GoodJets GoodJets;

            void fill(const GoodJets &good_jets);
            void match(const Event *);

            // More than one parton can be matched to the same jet. Skip such
            // jets in the b-tagging efficiency calculation. Therefore need to
            // know how many partons are matched to each jet
            //
            typedef std::vector<const GenParticle *> Partons;
            typedef std::map<const CorrectedJet *, Partons> MatchedJets;

            MatchedJets jets;

            private:
                void match(const GenParticle &);
        };
    }

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

            virtual void print(std::ostream &) const;

        private:
            typedef boost::shared_ptr<H2Proxy> H2ProxyPtr;

            boost::shared_ptr<SynchSelector> _synch_selector;

            H2ProxyPtr _parton_jets;
            H2ProxyPtr _btagged_parton_jets;

            boost::shared_ptr<Btag> _btag;
    };
}

#endif
