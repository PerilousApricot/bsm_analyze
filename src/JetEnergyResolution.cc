// Jet Energy Resolution
//
// Doc: https://twiki.cern.ch/twiki/bin/view/CMS/JetResolution
//
// Created by Samvel Khalatyan, Apr 10, 2012
// Copyright 2012, All rights reserved

#include <algorithm>

#include "bsm_core/interface/ID.h"
#include "bsm_input/interface/Algebra.h"
#include "bsm_input/interface/Jet.pb.h"
#include "interface/CorrectedJet.h"

#include "interface/JetEnergyResolution.h"

using namespace bsm;
using namespace std;

JetEnergyResolutionOptions::JetEnergyResolutionOptions()
{
    _description.reset(new po::options_description("JetEnergyResolution Options"));
    _description->add_options()
        ("jer-up",
         po::value<bool>()->implicit_value(true)->notifier(
             boost::bind(&JetEnergyResolutionOptions::setSystematic, this,
                         JetEnergyResolutionDelegate::UP)),
         "Use Jet Energy Resolution systematic up")

        ("jer-down",
         po::value<bool>()->implicit_value(true)->notifier(
             boost::bind(&JetEnergyResolutionOptions::setSystematic, this,
                         JetEnergyResolutionDelegate::DOWN)),
         "Use Jet Energy Resolution systematic down")
    ;
}

// Options interface
//
JetEnergyResolutionOptions::DescriptionPtr 
    JetEnergyResolutionOptions::description() const
{
    return _description;
}

// Private
//
void JetEnergyResolutionOptions::setSystematic(
        const JetEnergyResolutionDelegate::Systematic &systematic)
{
    if (!delegate())
        return;

    delegate()->setSystematic(systematic);
}



JetEnergyResolution::JetEnergyResolution():
    _systematic(NONE)
{
}

JetEnergyResolution::JetEnergyResolution(const JetEnergyResolution &obj):
    _systematic(obj._systematic)
{
}

void JetEnergyResolution::correct(CorrectedJet &jet)
{
    // Skip jets that do not have matched partons
    //
    if (!jet.jet->has_gen_parton())
        return;

    const float gen_pt = pt(jet.jet->gen_parton().physics_object().p4());
    const float reco_pt = pt(*jet.corrected_p4);

    const float pt_scale = max(0.f, 1 + scale(eta(*jet.corrected_p4)) * 
                                        (reco_pt - gen_pt) / reco_pt);

    *jet.corrected_p4 *= pt_scale;
}

// JetEnergyResolutionDelegate interface
// 
void JetEnergyResolution::setSystematic(const Systematic &systematic)
{
    _systematic = systematic;
}

// Object interface
//
uint32_t JetEnergyResolution::id() const
{
    return core::ID<JetEnergyResolution>::get();
}

JetEnergyResolution::ObjectPtr JetEnergyResolution::clone() const
{
    return ObjectPtr(new JetEnergyResolution(*this));
}

void JetEnergyResolution::print(std::ostream &) const
{
}

// Private
//
float JetEnergyResolution::scale(const float &reco_eta)
{
    switch(_systematic)
    {
        case NONE:
            if (0.5 > reco_eta)
                return 0.052;

            if (1.1 > reco_eta)
                return 0.057;

            if (1.7 > reco_eta)
                return 0.096;

            if (2.3 > reco_eta)
                return 0.134;

            return 0.288;

        case UP:
            if (0.5 > reco_eta)
                return 0.115;

            if (1.1 > reco_eta)
                return 0.114;

            if (1.7 > reco_eta)
                return 0.161;

            if (2.3 > reco_eta)
                return 0.228;

            return 0.488;

        case DOWN:
            if (0.5 > reco_eta)
                return -0.01;

            if (1.1 > reco_eta)
                return 0.;

            if (1.7 > reco_eta)
                return 0.032;

            if (2.3 > reco_eta)
                return 0.042;

            return 0.089;

        default:
            throw runtime_error("unsupported jet energy resolution systematic");
    }
}
