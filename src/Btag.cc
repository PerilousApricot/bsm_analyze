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
        ("use-btag-sf",
         po::value<bool>()->implicit_value(true)->notifier(
             boost::bind(&BtagOptions::setUseBtagSF, this)),
         "Use b-tagging Scale Factors")

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
void BtagOptions::setUseBtagSF()
{
    if (delegate())
        delegate()->setUseBtagSF();
}

void BtagOptions::setSystematic(const BtagDelegate::Systematic &systematic)
{
    if (!delegate())
        return;

    delegate()->setSystematic(systematic);
}



// Btag Scale
//
const float BtagScale::ptmax[] = {
    40, 50, 60, 70, 80, 100, 120, 160, 210, 260, 320, 400, 500, 670
};

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

    uint32_t bin = 0;
    for(; 14 > bin; ++bin)
    {
        if (BtagScale::ptmax[bin] > jet_pt)
            break;
    }

    return BtagScale::errors[bin];
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
float BtagEfficiency::value(const float &discriminator) const
{
    return 1.04305075822 -
           1.03328577451 * discriminator +
           0.784721653518 * pow(discriminator, 2) +
           1.26161794785 * pow(discriminator, 3) -
           1.73338329789 * pow(discriminator, 4);
}



// Ctag efficiency
//
float CtagEfficiency::value(const float &discriminator) const
{
    return 0.780639301724 -
           1.66657942274 * discriminator +
           0.866697059943 * pow(discriminator, 2) +
           1.52798999269 * pow(discriminator, 3) -
           1.5734604211 * pow(discriminator, 4);
}



// Light Efficiency
//
float LightEfficiency::value(const float &jet_pt) const
{
    if (20 > jet_pt)
        return value(20);

    if (670 < jet_pt)
        return value(670);

    return 0.00315116 *
            (1 -
             0.00769281 * jet_pt +
             2.58066e-05 * pow(jet_pt, 2) -
             2.02149e-08 * pow(jet_pt, 3));
}



// Btag
//
Btag::Btag():
    _systematic(NONE),
    _use_sf(false)
{
    _generator.reset(new boost::mt19937());
}

Btag::Btag(const Btag &object):
    _systematic(object._systematic),
    _use_sf(object._use_sf)
{
    _generator.reset(new boost::mt19937());
}

float Btag::discriminator()
{
    return 0.898;
}

float Btag::btag_efficiency(const float &discriminator)
{
    return 1.04305075822 -
           1.03328577451 * discriminator +
           0.784721653518 * pow(discriminator, 2) +
           1.26161794785 * pow(discriminator, 3) -
           1.73338329789 * pow(discriminator, 4);
}

float Btag::btag_scale(const float &discriminator)
{
    return 1.04926963159 - 0.113472343605 * discriminator;
}

float Btag::mistag_efficiency(const float &jet_pt)
{
    return 0.00315116 * (1 -
                         0.00769281 * jet_pt +
                         2.58066e-05 * pow(jet_pt, 2) -
                         2.02149e-08 * pow(jet_pt, 3));
}

float Btag::mistag_scale(const float &jet_pt)
{
    return 0.948463 +
           0.00288102 * jet_pt -
           7.98091e-06 * pow(jet_pt, 2) +
           5.50157e-09 * pow(jet_pt, 3);
}

float Btag::mistag_scale_sigma_down(const float &jet_pt)
{
    return 0.899715 +
           0.00102278 * jet_pt -
           2.46335e-06 * pow(jet_pt, 2) +
           9.71143e-10 * pow(jet_pt, 3);
}

float Btag::mistag_scale_sigma_up(const float &jet_pt)
{
    return 0.997077 +
           0.00473953 * jet_pt -
           1.34985e-05 * pow(jet_pt, 2) +
           1.0032e-08 * pow(jet_pt, 3);
}

float Btag::btag_scale_with_systematic(const float &discriminator,
                                       const float &uncertainty)
{
    float scale = Btag::btag_scale(discriminator);
    
    if (_systematic)
        scale += _systematic * uncertainty;

    return scale;
}

float Btag::mistag_scale_with_systematic(const float &jet_pt)
{
    switch(_systematic)
    {
        case DOWN: return Btag::mistag_scale_sigma_down(jet_pt);
        case NONE: return Btag::mistag_scale(jet_pt);
        case UP: return Btag::mistag_scale_sigma_up(jet_pt);

        default: throw runtime_error("unsupported systematic type used");
    }
}

bool Btag::is_tagged(const CorrectedJet &jet)
{
    typedef ::google::protobuf::RepeatedPtrField<Jet::BTag> BTags;

    for(BTags::const_iterator btag = jet.jet->btag().begin();
            jet.jet->btag().end() != btag;
            ++btag)
    {
        if (Jet::BTag::CSV == btag->type())
        {
            const float discriminator = btag->discriminator();
            bool result = Btag::discriminator() < discriminator;

            if (_use_sf && jet.jet->has_gen_parton())
            {
                float scale = 0;
                float efficiency = 0;

                switch(abs(jet.jet->gen_parton().id()))
                {
                    case 5:
                        scale = btag_scale_with_systematic(discriminator, 0.04);
                        efficiency = Btag::btag_efficiency(discriminator);

                        break;

                    case 4:
                        scale = btag_scale_with_systematic(discriminator, 0.08);
                        efficiency = Btag::btag_efficiency(discriminator);

                        break;

                    case 3: // fall through
                    case 2: // fall through
                    case 1:
                        const float jet_pt = pt(*jet.corrected_p4);

                        scale = mistag_scale_with_systematic(jet_pt);
                        efficiency = Btag::mistag_efficiency(jet_pt);

                        break;
                }

                if (scale && efficiency)
                {
                    _generator->seed(static_cast<unsigned int>(
                                        sin(phi(*jet.corrected_p4) * 1000000)));

                    result = correct(result, scale, efficiency);
                }
            }

            return result;
        }
    }

    return false;
}

// BtagDelegate interface
//
void Btag::setUseBtagSF()
{
    _use_sf = true;
}

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



// Private
//
bool Btag::correct(const bool &is_tagged,
                   const float &scale,
                   const float &efficiency)
{
    if (1 == scale)
        return is_tagged;

    bool result = is_tagged;
    if (1 < scale)
    {
        if (!is_tagged)
        {
            if ((*_generator)() < (1 - scale) / (1 - scale / efficiency))
                result = true;
        }
    }
    else
    {
        if (is_tagged &&
            (*_generator)() > scale)

            result = false;
    }

    return result;
}
