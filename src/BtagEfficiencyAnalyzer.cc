// Measure b-tagging efficiency
//
// Created by Samvel Khalatyan, Apr 04, 2012
// Copyright 2011, All rights reserved

#include <boost/pointer_cast.hpp>

#include "bsm_core/interface/ID.h"
#include "bsm_input/interface/Algebra.h"
#include "bsm_input/interface/Event.pb.h"
#include "bsm_input/interface/GenParticle.pb.h"
#include "bsm_input/interface/Jet.pb.h"
#include "bsm_stat/interface/H2.h"
#include "interface/Btag.h"
#include "interface/BtagEfficiencyAnalyzer.h"
#include "interface/StatProxy.h"

using namespace std;

using namespace boost;
using namespace bsm;
using gen::MatchedPartons;

// Matched Jets
//
void MatchedPartons::fill(const GoodJets &good_jets)
{
    jets.clear();

    for(GoodJets::const_iterator jet = good_jets.begin();
            good_jets.end() != jet;
            ++jet)
    {
        jets[&*jet];
    }
}

void MatchedPartons::match(const Event *event)
{
    typedef ::google::protobuf::RepeatedPtrField<GenParticle> GenParticles;

    const GenParticles &particles = event->gen_particle();
    for(GenParticles::const_iterator particle = particles.begin();
            particles.end() != particle;
            ++particle)
    {
        if (3 != particle->status())
            continue;

        match(*particle);
    }
}

// Private
//
void MatchedPartons::match(const GenParticle &particle)
{
    // Process particle
    //
    const int32_t pdg_id = abs(particle.id());

    // Keep only quarks: u, d, c, s, b - and gluons, e.g. no t-quark
    //
    if ((0 < pdg_id && 7 > pdg_id) ||
        21 == pdg_id)
    {
        for(MatchedJets::iterator jet = jets.begin();
                jets.end() != jet;
                ++jet)
        {
            if (0.1 > dr(*jet->first->corrected_p4, 
                         particle.physics_object().p4()))
            {
                bool parton_is_found = false;
                for(MatchedPartons::Partons::const_iterator parton =
                            jet->second.begin();
                        jet->second.end() != parton;
                        ++parton)
                {
                    if (abs((*parton)->id()) == pdg_id &&
                        (*parton)->physics_object().p4() ==
                            particle.physics_object().p4())
                    {
                        parton_is_found = true;

                        break;
                    }
                }

                if (!parton_is_found)
                    jet->second.push_back(&particle);
                
                break;
            }
        }
    }

    // Take care of children
    //
    typedef ::google::protobuf::RepeatedPtrField<GenParticle> GenParticles;

    const GenParticles &children = particle.child();
    for(GenParticles::const_iterator child = children.begin();
            children.end() != child;
            ++child)
    {
        if (3 != child->status())
            continue;

        match(*child);
    }
}



// Btag Efficiency Analyzer
//
BtagEfficiencyAnalyzer::BtagEfficiencyAnalyzer()
{
    _synch_selector.reset(new SynchSelector());
    monitor(_synch_selector);

    _parton_jets.reset(new H2Proxy(25, 0, 25, 67, 0, 670));
    monitor(_parton_jets);

    _btagged_parton_jets.reset(new H2Proxy(25, 0, 25, 67, 0, 670));
    monitor(_btagged_parton_jets);

    _btag.reset(new Btag());
    monitor(_btag);
}

BtagEfficiencyAnalyzer::BtagEfficiencyAnalyzer(
        const BtagEfficiencyAnalyzer &object)
{
    _synch_selector = 
        dynamic_pointer_cast<SynchSelector>(object._synch_selector->clone());
    monitor(_synch_selector);

    _parton_jets = dynamic_pointer_cast<H2Proxy>(object._parton_jets->clone());
    monitor(_parton_jets);

    _btagged_parton_jets =
        dynamic_pointer_cast<H2Proxy>(object._btagged_parton_jets->clone());
    monitor(_btagged_parton_jets);
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
void BtagEfficiencyAnalyzer::onFileOpen(const string &filename, const Input *)
{
}

void BtagEfficiencyAnalyzer::process(const Event *event)
{
    if (!event->has_missing_energy())
        return;

    // Process only events, that pass the synch selector
    //
    if (_synch_selector->apply(event))
    {
        MatchedPartons partons;

        partons.fill(_synch_selector->goodJets());
        partons.match(event);

        // Use only those, that have one parton matched to one jet
        //
        for(MatchedPartons::MatchedJets::const_iterator jet =
                    partons.jets.begin();
                partons.jets.end() != jet;
                ++jet)
        {
            if (1 != jet->second.size())
                continue;

            parton_jets()->fill(abs(jet->second[0]->id()),
                                pt(*jet->first->corrected_p4));

            if (_btag->is_tagged(*jet->first))
                btagged_parton_jets()->fill(abs(jet->second[0]->id()),
                                            pt(*jet->first->corrected_p4));
        }
    }
}

uint32_t BtagEfficiencyAnalyzer::id() const
{
    return core::ID<BtagEfficiencyAnalyzer>::get();
}

void BtagEfficiencyAnalyzer::print(ostream &out) const
{
    out << *_synch_selector << endl;
}
