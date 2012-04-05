// Measure b-tagging efficiency
//
// Created by Samvel Khalatyan, Apr 04, 2012
// Copyright 2011, All rights reserved

#include "bsm_core/interface/ID.h"
#include "bsm_input/interface/Algebra.h"
#include "bsm_input/interface/Event.pb.h"
#include "bsm_input/interface/GenParticle.pb.h"
#include "bsm_input/interface/Jet.pb.h"
#include "bsm_stat/interface/H2.h"
#include "interface/BtagEfficiencyAnalyzer.h"
#include "interface/StatProxy.h"

using namespace bsm;
using namespace std;
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
    const uint32_t pdg_id = abs(particle.id());

    // Keep only quark for the moment
    //
    if (0 < pdg_id && 7 > pdg_id)
    {
        for(MatchedJets::iterator jet = jets.begin();
                jets.end() != jet;
                ++jet)
        {
            if (0.3 > dr(*jet->first->corrected_p4, 
                         particle.physics_object().p4()))
            {
                jet->second.push_back(&particle);
                
                break;
            }
        }
    }

    // Take care of its children
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
}

BtagEfficiencyAnalyzer::BtagEfficiencyAnalyzer(const BtagEfficiencyAnalyzer &)
{
}

const stat::H2Ptr BtagEfficiencyAnalyzer::parton_jets() const
{
    return _parton_jets->histogram();
}

const stat::H2Ptr BtagEfficiencyAnalyzer::btagged_parton_jets() const
{
    return _btagged_parton_jets->histogram();
}

void BtagEfficiencyAnalyzer::onFileOpen(const string &filename, const Input *)
{
}

void BtagEfficiencyAnalyzer::process(const Event *)
{
}

uint32_t BtagEfficiencyAnalyzer::id() const
{
    return core::ID<BtagEfficiencyAnalyzer>::get();
}

void BtagEfficiencyAnalyzer::print(ostream &out) const
{
}
