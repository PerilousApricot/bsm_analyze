// b-tagging utils
//
// Created by Samvel Khalatyan, Mar 25, 2011
// Copyright 2011, All rights reserved

#include <stdexcept>

#include "bsm_core/interface/ID.h"
#include "bsm_input/interface/Algebra.h"
#include "bsm_input/interface/Jet.pb.h"

#include "interface/Btag.h"
#include "interface/CorrectedJet.h"

using namespace bsm;
using namespace std;

BtagOptions::BtagOptions()
{
    _description.reset(new po::options_description("Btag Options"));
    _description->add_options()
        ("btag-up",
         po::value<bool>()->implicit_value(true)->notifier(
             boost::bind(&BtagOptions::setSystematic, this, BtagDelegate::UP)),
         "Change btag SF one sigma up")

        ("btag-down",
         po::value<bool>()->implicit_value(true)->notifier(
             boost::bind(&BtagOptions::setSystematic, this, BtagDelegate::DOWN)),
         "Change btag SF one sigma down")
    ;
}

// Options interface
//
BtagOptions::DescriptionPtr BtagOptions::description() const
{
    return _description;
}

// Private
//
void BtagOptions::setSystematic(const BtagDelegate::Systematic &systematic)
{
    if (!delegate())
        return;

    delegate()->setSystematic(systematic);
}



// BtagFunction
//
const uint32_t BtagFunction::bins = 14;

const float BtagFunction::jet_pt_bins[] = {
    30, 40, 50, 60, 70, 80, 100, 120, 160, 210, 260, 320, 400, 500, 670
};

const uint32_t BtagFunction::find_bin(const float &jet_pt) const
{
    if (BtagFunction::jet_pt_bins[0] > jet_pt)
        return 0;

    if (BtagFunction::jet_pt_bins[BtagFunction::bins] < jet_pt)
        return BtagFunction::bins - 1;

    for(uint32_t bin = 1; BtagFunction::bins >= bin; ++bin)
    {
        if (BtagFunction::jet_pt_bins[bin] > jet_pt)
            return bin - 1;
    }

    throw runtime_error("failed to find b-tag bin");
}


// Btag Scale
//
const float BtagScale::errors[] = {
    0.0364717, 0.0362281, 0.0232876, 0.0249618, 0.0261482, 0.0290466,
    0.0300033, 0.0453252, 0.0685143, 0.0653621, 0.0712586, 0.094589,
    0.0777011, 0.0866563
};

float BtagScale::value(const float &jet_pt) const
{
    // underflow
    //
    if (30 > jet_pt)
        return value(30);

    // overflow
    //
    if (670 < jet_pt)
        return value(670);

    return 0.901615 *
           (1. + 0.552628 * jet_pt) /
           (1. + 0.547195 * jet_pt);
}

float BtagScale::error(const float &jet_pt) const
{
    if (30 > jet_pt)
        return 0.12;

    if (670 <= jet_pt)
        return 2 * error(669);

    return BtagScale::errors[find_bin(jet_pt)];
}



// Ctag scale
//
float CtagScale::error(const float &jet_pt) const
{
    return 2 * BtagScale::error(jet_pt);
}



// Light-tag scale
//
float LightScale::value(const float &jet_pt) const
{
    if (20 > jet_pt)
        return value(20);

    if (670 < jet_pt)
        return value(670);

    return 0.948463 + 
           0.00288102 * jet_pt -
           7.98091e-06 * pow(jet_pt, 2) + 
           5.50157e-09 * pow(jet_pt, 3);
}

float LightScale::error_plus(const float &jet_pt) const
{
    return LightScale::value_max(jet_pt) - value(jet_pt);
}

float LightScale::error_minus(const float &jet_pt) const
{
    return value(jet_pt) - LightScale::value_min(jet_pt);
}

// Private
//
float LightScale::value_max(const float &jet_pt)
{
    if (20 > jet_pt)
        return LightScale::value_max(20);

    if (670 < jet_pt)
        return LightScale::value_max(670);

    return 0.997077 +
           0.00473953 * jet_pt -
           1.34985e-05 * pow(jet_pt, 2) +
           1.0032e-08 * pow(jet_pt, 3);
}

float LightScale::value_min(const float &jet_pt)
{
    if (20 > jet_pt)
        return LightScale::value_min(20);

    if (670 < jet_pt)
        return LightScale::value_min(670);

    return 0.899715 +
           0.00102278 * jet_pt -
           2.46335e-06 * pow(jet_pt, 2) +
           9.71143e-10 * pow(jet_pt, 3);
}



// Btag Efficiency
//
const float BtagEfficiency::values[] = {
    0.0, 0.0, 0.45700194476, 0.481780083914, 0.485380425249, 0.515662731626,
    0.498871069179, 0.492350232848, 0.397570152294, 0.32058194111,
    0.271581953854, 0.224112593547, 0.11042330955, 0.123300043702
};

float BtagEfficiency::value(const float &jet_pt) const
{
    return BtagEfficiency::values[find_bin(jet_pt)];
}



// Light Efficiency
//
const float LightEfficiency::values[] = {
    0.0, 0.0, 0.00665025786822, 0.00661178343117, 0.00383612052866,
    0.0096833532719, 0.00611911636857, 0.00985837381435, 0.00888843910838,
    0.0152255871887, 0.00625901203913, 0.00862685315311, 0.00729571108855,
    0.01859141652
};

float LightEfficiency::value(const float &jet_pt) const
{
    return LightEfficiency::values[find_bin(jet_pt)];
}



// Btag
//
const float Btag::_discriminator = 0.898;

Btag::Btag():
    _systematic(NONE)
{
    _scale_btag.reset(new BtagScale());
    _eff_btag.reset(new BtagEfficiency());

    _scale_ctag.reset(new CtagScale());
    _eff_ctag.reset(new CtagEfficiency());

    _scale_light.reset(new LightScale());
    _eff_light.reset(new LightEfficiency());
}

Btag::Btag(const Btag &object):
    _systematic(object._systematic)
{
    _scale_btag.reset(new BtagScale());
    _eff_btag.reset(new BtagEfficiency());

    _scale_ctag.reset(new CtagScale());
    _eff_ctag.reset(new CtagEfficiency());

    _scale_light.reset(new LightScale());
    _eff_light.reset(new LightEfficiency());
}

Btag::Info Btag::is_tagged(const CorrectedJet &jet)
{
    typedef ::google::protobuf::RepeatedPtrField<Jet::BTag> BTags;

    for(BTags::const_iterator btag = jet.jet->btag().begin();
            jet.jet->btag().end() != btag;
            ++btag)
    {
        if (Jet::BTag::CSV == btag->type())
        {
            bool result = Btag::_discriminator < btag->discriminator();
            float scale = 1;

            if (jet.jet->has_gen_parton())
            {
                BtagFunctionPtr _scale_function;
                BtagFunctionPtr _eff_function;

                switch(abs(jet.jet->gen_parton().id()))
                {
                    case 5: // b-quark
                        _scale_function = _scale_btag;
                        _eff_function = _eff_btag;
                        break;

                    case 4: // c-quark
                        _scale_function = _scale_ctag;
                        _eff_function = _eff_ctag;
                        break;

                    case 3: // s-quark
                    case 2: // d-quark
                    case 1: // u-quark
                    case 21: // gluon
                        _scale_function = _scale_light;
                        _eff_function = _eff_light;
                        break;

                    default:
                        break;
                }

                if (_scale_function && _eff_function)
                {
                    const float jet_pt = pt(*jet.corrected_p4);

                    scale = result ?
                            _scale_function->value(jet_pt) :
                            (1 - _scale_function->value(jet_pt) *
                                    _eff_function->value(jet_pt)) /
                                (1 - _eff_function->value(jet_pt));
                }
            }

            return make_pair(result, scale);
        }
    }

    return make_pair(false, 1);
}

// BtagDelegate interface
//
void Btag::setSystematic(const Systematic &systematic)
{
    _systematic = systematic;
}

// Object interface
//
uint32_t Btag::id() const
{
    return core::ID<Btag>::get();
}

Btag::ObjectPtr Btag::clone() const
{
    return ObjectPtr(new Btag(*this));
}

void Btag::print(std::ostream &out) const
{
}
