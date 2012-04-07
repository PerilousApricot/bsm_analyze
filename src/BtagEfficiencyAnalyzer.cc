// Measure b-tagging efficiency
//
// Created by Samvel Khalatyan, Apr 04, 2012
// Copyright 2011, All rights reserved

#include <boost/pointer_cast.hpp>

#include "bsm_core/interface/ID.h"
#include "bsm_input/interface/Algebra.h"
#include "bsm_input/interface/Event.pb.h"
#include "bsm_input/interface/GenParticle.pb.h"
#include "bsm_input/interface/Input.pb.h"
#include "bsm_input/interface/Jet.pb.h"
#include "bsm_stat/interface/H2.h"
#include "interface/Btag.h"
#include "interface/BtagEfficiencyAnalyzer.h"
#include "interface/Pileup.h"
#include "interface/StatProxy.h"
#include "interface/SynchSelector.h"

using namespace std;

using namespace boost;
using namespace bsm;

// Btag Efficiency Analyzer
//
BtagEfficiencyAnalyzer::BtagEfficiencyAnalyzer():
    _use_pileup(false),
    _pileup_weight(1)
{
    _synch_selector.reset(new SynchSelector());
    monitor(_synch_selector);

    _parton_jets.reset(new H2Proxy(25, 0, 25, 67, 0, 670));
    monitor(_parton_jets);

    _btagged_parton_jets.reset(new H2Proxy(25, 0, 25, 67, 0, 670));
    monitor(_btagged_parton_jets);

    _btag.reset(new Btag());
    monitor(_btag);

    _pileup.reset(new Pileup());
    monitor(_pileup);
}

BtagEfficiencyAnalyzer::BtagEfficiencyAnalyzer(
        const BtagEfficiencyAnalyzer &object):
    _use_pileup(false),
    _pileup_weight(1)
{
    _synch_selector = 
        dynamic_pointer_cast<SynchSelector>(object._synch_selector->clone());
    monitor(_synch_selector);

    _parton_jets = dynamic_pointer_cast<H2Proxy>(object._parton_jets->clone());
    monitor(_parton_jets);

    _btagged_parton_jets =
        dynamic_pointer_cast<H2Proxy>(object._btagged_parton_jets->clone());
    monitor(_btagged_parton_jets);

    _btag.reset(new Btag());
    monitor(_btag);

    _pileup =
        dynamic_pointer_cast<Pileup>(object._pileup->clone());
    monitor(_pileup);
}

// Delegates
//
bsm::JetEnergyCorrectionDelegate
    *BtagEfficiencyAnalyzer::getJetEnergyCorrectionDelegate() const
{
    return _synch_selector.get();
}

bsm::SynchSelectorDelegate
    *BtagEfficiencyAnalyzer::getSynchSelectorDelegate() const
{
    return _synch_selector.get();
}

bsm::TriggerDelegate *BtagEfficiencyAnalyzer::getTriggerDelegate() const
{
    return _synch_selector.get();
}

bsm::PileupDelegate *BtagEfficiencyAnalyzer::getPileupDelegate() const
{
    return _pileup.get();
}

// Accessors
//
const stat::H2Ptr BtagEfficiencyAnalyzer::parton_jets() const
{
    return _parton_jets->histogram();
}

const stat::H2Ptr BtagEfficiencyAnalyzer::btagged_parton_jets() const
{
    return _btagged_parton_jets->histogram();
}

// Processing
//
void BtagEfficiencyAnalyzer::onFileOpen(const string &filename,
                                        const Input *input)
{
    if (input->has_type())
        _use_pileup = (Input::DATA != input->type());
    else
        _use_pileup = false;
}

void BtagEfficiencyAnalyzer::process(const Event *event)
{
    if (!event->has_missing_energy())
        return;

    // Process only events, that pass the synch selector
    //
    if (_synch_selector->apply(event))
    {
        _pileup_weight = _use_pileup ? 0 : 1;
        if (_use_pileup)
            _pileup_weight = _pileup->scale(event);

        typedef SynchSelector::GoodJets GoodJets;
        for(GoodJets::const_iterator jet = _synch_selector->goodJets().begin();
                _synch_selector->goodJets().end() != jet;
                ++jet)
        {
            if (jet->jet->has_gen_parton())
            {
                const uint32_t pdg_id = abs(jet->jet->gen_parton().id());
                if ((0 < pdg_id && 6 > pdg_id) || 21 == pdg_id)
                {
                    parton_jets()->fill(pdg_id,
                                        pt(*jet->corrected_p4),
                                        _pileup_weight);

                    if (_btag->is_tagged(*jet).first)
                        btagged_parton_jets()->fill(pdg_id,
                                                    pt(*jet->corrected_p4),
                                                    _pileup_weight);
                }
            }
        }
    }
}

uint32_t BtagEfficiencyAnalyzer::id() const
{
    return core::ID<BtagEfficiencyAnalyzer>::get();
}

BtagEfficiencyAnalyzer::ObjectPtr BtagEfficiencyAnalyzer::clone() const
{
    return ObjectPtr(new BtagEfficiencyAnalyzer(*this));
}
void BtagEfficiencyAnalyzer::print(ostream &out) const
{
    out << *_synch_selector << endl;
}
