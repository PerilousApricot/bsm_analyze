// Synchronization exercise selector that is used to select "good"
// events with muons, electrons, jetc, etc.
//
// Created by Samvel Khalatyan, Aug 01, 2011
// Copyright 2011, All rights reserved

#include <functional>

#include <boost/algorithm/string.hpp>
#include <boost/pointer_cast.hpp>

#include "bsm_input/interface/Algebra.h"
#include "bsm_input/interface/Electron.pb.h"
#include "bsm_input/interface/Event.pb.h"
#include "bsm_input/interface/Isolation.pb.h"
#include "bsm_input/interface/Muon.pb.h"
#include "bsm_input/interface/PrimaryVertex.pb.h"
#include "bsm_input/interface/Physics.pb.h"
#include "interface/Btag.h"
#include "interface/Cut.h"
#include "interface/JetEnergyResolution.h"
#include "interface/SynchSelector.h"
#include "interface/Cut2DSelector.h"
#include "interface/Utility.h"

using namespace std;
using namespace boost;
using namespace bsm;

// Synch Selector Options
//
SynchSelectorOptions::SynchSelectorOptions()
{
    _description.reset(new po::options_description("Synchronization Selector Options"));
    _description->add_options()
        ("lepton-mode",
         po::value<string>()->notifier(
             boost::bind(&SynchSelectorOptions::setLeptonMode, this, _1)),
         "synchronization selector lepton mode: muon, electron")

        ("cut-mode",
         po::value<string>()->notifier(
             boost::bind(&SynchSelectorOptions::setCutMode, this, _1)),
         "synchroniation selector cut mode: 2dcut, isolation")

        ("leading-jet",
         po::value<float>()->notifier(
             boost::bind(&SynchSelectorOptions::setLeadingJetPt, this, _1)),
         "leading jet pT cut")

        ("jet-pt",
         po::value<float>()->notifier(
             boost::bind(&SynchSelectorOptions::setJetPt, this, _1)),
         "cut all jet pT")

        ("max-btags",
         po::value<float>()->notifier(
             boost::bind(&SynchSelectorOptions::setMaxBtag, this, _1)),
         "maximum number of b-tagged jets in event")

        ("min-btags",
         po::value<float>()->notifier(
             boost::bind(&SynchSelectorOptions::setMinBtag, this, _1)),
         "minimum number of b-tagged jets in event")

        ("electron-pt",
         po::value<float>()->notifier(
             boost::bind(&SynchSelectorOptions::setElectronPt, this, _1)),
         "electron pT cut")

         ("qcd-template",
          po::value<bool>()->implicit_value(false)->notifier(
             boost::bind(&SynchSelectorOptions::setQCDTemplate, this, _1)),
         "derived a qcd template")   

        ("ltop-pt",
         po::value<float>()->notifier(
             boost::bind(&SynchSelectorOptions::setLtopPt, this, _1)),
         "set min value of ltop pt")

        ("chi2",
         po::value<float>()->notifier(
             boost::bind(&SynchSelectorOptions::setChi2Discriminator, this, _1)),
         "set max chi2 disriminator")

        ("invert-chi2",
         po::value<bool>()->implicit_value(false)->notifier(
             boost::bind(&SynchSelectorOptions::setInvertChi2, this, _1)),
         "invert chi2 cut")

        ("wflavor",
         po::value<string>()->notifier(
             boost::bind(&SynchSelectorOptions::setWflavor, this, _1)),
         "select W+flavor events: wbx, wcx, wlight")
    ;
}

// Options interface
//
SynchSelectorOptions::DescriptionPtr SynchSelectorOptions::description() const
{
    return _description;
}

// Privates
//
void SynchSelectorOptions::setLeptonMode(std::string mode)
{
    if (!delegate())
        return;

    to_lower(mode);

    if ("electron" == mode)
        delegate()->setLeptonMode(SynchSelectorDelegate::ELECTRON);
    else if ("muon" == mode)
        delegate()->setLeptonMode(SynchSelectorDelegate::MUON);
    else
        cerr << "unsupported synchronization selector lepton mode" << endl;
}

void SynchSelectorOptions::setCutMode(std::string mode)
{
    if (!delegate())
        return;

    to_lower(mode);

    if ("2dcut" == mode)
        delegate()->setCutMode(SynchSelectorDelegate::CUT_2D);
    else if ("isolation" == mode)
        delegate()->setCutMode(SynchSelectorDelegate::ISOLATION);
    else
        cerr << "unsupported synchronization selector cut mode" << endl;
}

void SynchSelectorOptions::setLeadingJetPt(const float &value)
{
    if (!delegate())
        return;

    if (0 > value)
    {
        cerr << "only positive values of leading jet pT are accepted" << endl;

        return;
    }

    delegate()->setLeadingJetPt(value);
}

void SynchSelectorOptions::setJetPt(const float &value)
{
    if (!delegate())
        return;

    if (0 > value)
    {
        cerr << "only positive values of jet pT are accepted" << endl;

        return;
    }

    delegate()->setJetPt(value);
}

void SynchSelectorOptions::setMaxBtag(const float &value)
{
    if (!delegate())
        return;

    delegate()->setMaxBtag(value);
}

void SynchSelectorOptions::setMinBtag(const float &value)
{
    if (!delegate())
        return;

    delegate()->setMinBtag(value);
}

void SynchSelectorOptions::setElectronPt(const float & value)
{
    if (!delegate()) return;    

    delegate()->setElectronPt(value);
}

void SynchSelectorOptions::setQCDTemplate(const bool &value)
{
    if (!delegate())
        return;

    delegate()->setQCDTemplate(value);
}

void SynchSelectorOptions::setLtopPt(const float &value)
{
    if (!delegate())
        return;

    if (0 > value)
        throw runtime_error("negative ltop pt is not allowed");

    delegate()->setLtopPt(value);
}

void SynchSelectorOptions::setChi2Discriminator(const float &value)
{
    if (!delegate())
        return;

    if (0 > value)
        throw runtime_error("negative chi2 cut is not allowed");

    delegate()->setChi2Discriminator(value);
}

void SynchSelectorOptions::setInvertChi2(const bool &value)
{
    if (!delegate())
        return;

    delegate()->setInvertChi2(value);
}

void SynchSelectorOptions::setWflavor(std::string mode)
{
    if (!delegate())
        return;

    to_lower(mode);

    if ("wjets" == mode)
        delegate()->setWflavor(SynchSelectorDelegate::WJETS);
    else if ("wbx" == mode)
        delegate()->setWflavor(SynchSelectorDelegate::WBX);
    else if ("wcx" == mode)
        delegate()->setWflavor(SynchSelectorDelegate::WCX);
    else if ("wlight" == mode)
        delegate()->setWflavor(SynchSelectorDelegate::WLIGHT);
    else
        cerr << "unsupported synchronization selector W+flavor mode" << endl;
}



// Synchronization Exercise Selector
//
SynchSelector::SynchSelector():
    _lepton_mode(ELECTRON),
    _cut_mode(CUT_2D),
    _qcd_template(false)
{
    // Cutflow table
    //
    _cutflow.reset(new MultiplicityCutflow(SELECTIONS - 1));
    monitor(_cutflow);

    // Selectors
    //
    _primary_vertex_selector.reset(new PrimaryVertexSelector());
    _primary_vertex_selector->cut(PrimaryVertexSelector::RHO)->setValue(2.0);
    monitor(_primary_vertex_selector);

    _electron_selector.reset(new ElectronSelector());
    _electron_selector->cut(ElectronSelector::PT)->setValue(70);
    _electron_selector->cut(ElectronSelector::PRIMARY_VERTEX)->disable();
    monitor(_electron_selector);

    _muon_selector.reset(new MuonSelector());
    _muon_selector->cut(MuonSelector::PT)->setValue(35);
    monitor(_muon_selector);

    // Nice jets have pT > 25
    //
    _nice_jet_selector.reset(new JetSelector());
    _nice_jet_selector->cut(JetSelector::PT)->setValue(25);
    monitor(_nice_jet_selector);

    // Good jets have pT > 50
    //
    _good_jet_selector.reset(new JetSelector());
    monitor(_good_jet_selector);

    // 2D-Cut
    //
    _cut2d_selector.reset(new Cut2DSelector());
    monitor(_cut2d_selector);

    // Jet Energy Corrections
    //
    _jec.reset(new DeltaRJetEnergyCorrections());
    monitor(_jec);

    // Jet Energy Resoltuion
    //
    _jer.reset(new JetEnergyResolution());
    monitor(_jer);

    // Cuts
    //
    _cut.reset(new Comparator<logical_and<bool> >(true));
    monitor(_cut);

    _wflavor.reset(new Comparator<equal_to<uint32_t> >(
                static_cast<uint32_t>(WJETS)));
    _wflavor->disable();
    monitor(_wflavor);

    _leading_jet.reset(new Comparator<>(150));
    monitor(_leading_jet);

    // Do not cut on max number of b-tagged jets by default
    //
    _max_btag.reset(new Comparator<less<float> >(3));
    _max_btag->disable();
    monitor(_max_btag);

    // Do not cut on min number of b-tagged jets by default
    //
    _min_btag.reset(new Comparator<std::greater_equal<float> >(1));
    _min_btag->disable();
    monitor(_min_btag);

    _htlep.reset(new Comparator<>(150));
    monitor(_htlep);

    // The cut value does not matter as it is not used
    //
    _tricut.reset(new Comparator<>(0));
    monitor(_tricut);

    _met.reset(new Comparator<>(50));
    monitor(_met);

    _reconstruction.reset(new Comparator<logical_and<bool> >(true));
    monitor(_reconstruction);

    _ltop.reset(new Comparator<>(100));
    monitor(_ltop);

    _chi2.reset(new Comparator<less<float> >(15));
    _chi2->disable();
    monitor(_chi2);

    _btag.reset(new Btag());
    monitor(_btag);
}

SynchSelector::SynchSelector(const SynchSelector &object):
    _lepton_mode(object._lepton_mode),
    _cut_mode(object._cut_mode),
    _qcd_template(object._qcd_template),
    _triggers(object._triggers.begin(), object._triggers.end())
{
    // Cutflow Table
    //
    _cutflow = 
        dynamic_pointer_cast<MultiplicityCutflow>(object._cutflow->clone());
    monitor(_cutflow);

    // Selectors
    //
    _primary_vertex_selector = 
        dynamic_pointer_cast<PrimaryVertexSelector>(object._primary_vertex_selector->clone());
    monitor(_primary_vertex_selector);

    _electron_selector = 
        dynamic_pointer_cast<ElectronSelector>(object._electron_selector->clone());
    monitor(_electron_selector);

    _muon_selector = 
        dynamic_pointer_cast<MuonSelector>(object._muon_selector->clone());
    monitor(_muon_selector);

    _nice_jet_selector = 
        dynamic_pointer_cast<JetSelector>(object._nice_jet_selector->clone());
    monitor(_nice_jet_selector);

    _good_jet_selector = 
        dynamic_pointer_cast<JetSelector>(object._good_jet_selector->clone());
    monitor(_good_jet_selector);

    // 2D-Cut
    //
    _cut2d_selector = 
        dynamic_pointer_cast<Cut2DSelector>(object._cut2d_selector->clone());
    monitor(_cut2d_selector);

    // Jet Energy Corrections
    //
    _jec = dynamic_pointer_cast<JetEnergyCorrections>(object._jec->clone());
    monitor(_jec);

    // Jet Energy Resolution
    //
    _jer = dynamic_pointer_cast<JetEnergyResolution>(object._jer->clone());
    monitor(_jer);

    // cuts
    //
    _cut = dynamic_pointer_cast<Cut>(object.cut()->clone());
    monitor(_cut);

    _wflavor = dynamic_pointer_cast<Cut>(object.wflavor()->clone());
    monitor(_wflavor);

    _leading_jet = dynamic_pointer_cast<Cut>(object.leadingJet()->clone());
    monitor(_leading_jet);

    _max_btag = dynamic_pointer_cast<Cut>(object.maxBtag()->clone());
    monitor(_max_btag);

    _min_btag = dynamic_pointer_cast<Cut>(object.minBtag()->clone());
    monitor(_min_btag);

    _htlep = dynamic_pointer_cast<Cut>(object.htlep()->clone());
    monitor(_htlep);

    _tricut = dynamic_pointer_cast<Cut>(object.tricut()->clone());
    monitor(_tricut);

    _met = dynamic_pointer_cast<Cut>(object.met()->clone());
    monitor(_met);

    _reconstruction =
        dynamic_pointer_cast<Cut>(object.reconstruction()->clone());
    monitor(_reconstruction);

    _ltop = dynamic_pointer_cast<Cut>(object.ltop()->clone());
    monitor(_ltop);

    _chi2 = dynamic_pointer_cast<Cut>(object.chi2()->clone());
    monitor(_chi2);

    _btag = dynamic_pointer_cast<Btag>(object._btag->clone());
    monitor(_btag);
}

SynchSelector::~SynchSelector()
{
}

SynchSelector::CutPtr SynchSelector::cut() const
{
    return _cut;
}

SynchSelector::CutPtr SynchSelector::wflavor() const
{
    return _wflavor;
}

SynchSelector::CutPtr SynchSelector::leadingJet() const
{
    return _leading_jet;
}

SynchSelector::CutPtr SynchSelector::maxBtag() const
{
    return _max_btag;
}

SynchSelector::CutPtr SynchSelector::minBtag() const
{
    return _min_btag;
}

SynchSelector::CutPtr SynchSelector::htlep() const
{
    return _htlep;
}

SynchSelector::CutPtr SynchSelector::tricut() const
{
    return _tricut;
}

SynchSelector::CutPtr SynchSelector::met() const
{
    return _met;
}

SynchSelector::CutPtr SynchSelector::reconstruction() const
{
    return _reconstruction;
}

SynchSelector::CutPtr SynchSelector::ltop() const
{
    return _ltop;
}

SynchSelector::CutPtr SynchSelector::chi2() const
{
    return _chi2;
}

SynchSelector::Btags SynchSelector::countBtaggedJets()
{
    if (!_btags.is_valid())
    {
        uint32_t btags = 0;
        float scale = 1;
        for(GoodJets::const_iterator jet = _good_jets.begin();
                _good_jets.end() != jet;
                ++jet)
        {
            Btag::Info info = _btag->is_tagged(*jet);
            if (info.first)
                ++btags;

            scale *= info.second;
        }

        _btags.set(make_pair(btags, scale));
    }

    return _btags.get();
}

bool SynchSelector::apply(const Event *event)
{
    invalidate_cache();

    _cutflow->apply(PRESELECTION);

    _good_primary_vertices.clear();
    _good_electrons.clear();
    _good_muons.clear();
    _nice_jets.clear();
    _good_jets.clear();
    _good_met.reset();
    _closest_jet = _nice_jets.end();

    // QCD template
    if (qcdTemplate())
    {
        tricut()->invert();

        return triggers(event)
            && primaryVertices(event)
            && jets(event)
            && lepton()
            && secondElectronVeto()
            && secondMuonVeto()
            && isolationAnd2DCut()
            && leadingJetCut()
            && splitWflavor()
            && maxBtags()
            && minBtags()
            && htlepCut(event)
            && missingEnergy(event)
            && triangularCut(event);
    }

    // Nominal
    return triggers(event)
        && primaryVertices(event)
        && jets(event)
        && lepton()
        && secondElectronVeto()
        && secondMuonVeto()
        && isolationAnd2DCut()
        && leadingJetCut()
        && splitWflavor()
        && maxBtags()
        && minBtags()
        && htlepCut(event)
        && triangularCut(event)
        && missingEnergy(event);
}

SynchSelector::CutflowPtr SynchSelector::cutflow() const
{
    return _cutflow;
}

const SynchSelector::GoodPrimaryVertices
    &SynchSelector::goodPrimaryVertices() const
{
    return _good_primary_vertices;
}

const SynchSelector::GoodElectrons &SynchSelector::goodElectrons() const
{
    return _good_electrons;
}

const SynchSelector::GoodMuons &SynchSelector::goodMuons() const
{
    return _good_muons;
}

const SynchSelector::GoodJets &SynchSelector::niceJets() const
{
    return _nice_jets;
}

const SynchSelector::GoodJets &SynchSelector::goodJets() const
{
    return _good_jets;
}

const SynchSelector::GoodMET &SynchSelector::goodMET() const
{
    return _good_met;
}

SynchSelector::GoodJets::const_iterator SynchSelector::closestJet() const
{
    return _closest_jet;
}

SynchSelector::LeptonMode SynchSelector::leptonMode() const
{
    return _lepton_mode;
}

SynchSelector::CutMode SynchSelector::cutMode() const
{
    return _cut_mode;
}

bool SynchSelector::qcdTemplate() const
{
    return _qcd_template;
}

bsm::Cut2DSelectorDelegate *SynchSelector::getCut2DSelectorDelegate() const
{
    return _cut2d_selector.get();
}

bsm::BtagDelegate *SynchSelector::getBtagDelegate() const
{
    return _btag.get();
}

bsm::JetEnergyResolutionDelegate *SynchSelector::getJERDelegate() const
{
    return _jer.get();
}

// Synch Selector Delegate interface
//
void SynchSelector::setLeptonMode(const LeptonMode &lepton_mode)
{
    _lepton_mode = lepton_mode;
}

void SynchSelector::setCutMode(const CutMode &cut_mode)
{
    _cut_mode = cut_mode;
}

void SynchSelector::setLeadingJetPt(const float &value)
{
    _leading_jet->setValue(value);
}

void SynchSelector::setJetPt(const float &value)
{
    _good_jet_selector->cut(JetSelector::PT)->setValue(value);
}

void SynchSelector::setMaxBtag(const float &value)
{
    _max_btag->setValue(value);
    _max_btag->enable();
}

void SynchSelector::setMinBtag(const float &value)
{
    _min_btag->setValue(value);
    _min_btag->enable();
}

void SynchSelector::setElectronPt(const float &value)
{
    _electron_selector->cut(ElectronSelector::PT)->setValue(value);
}

void SynchSelector::setQCDTemplate(const bool &value)
{
    _qcd_template = value;
}

void SynchSelector::setLtopPt(const float &value)
{
    ltop()->setValue(value);
}

void SynchSelector::setChi2Discriminator(const float &value)
{
    chi2()->setValue(value);
    chi2()->enable();
}

void SynchSelector::setInvertChi2(const bool &do_invert)
{
    if (do_invert)
        chi2()->invert();
    else
        chi2()->noinvert();
}

void SynchSelector::setWflavor(const Wflavor &flavor)
{
    wflavor()->setValue(static_cast<uint32_t>(flavor));
    wflavor()->enable();
}

// Jet Energy Correction Delegate interface
//
void SynchSelector::setCorrection(const Level &level,
        const string &file_name)
{
    _jec->setCorrection(level, file_name);
}

void SynchSelector::setSystematic(const Systematic &systematic,
        const std::string &filename)
{
    _jec->setSystematic(systematic, filename);
}

void SynchSelector::setChildCorrection()
{
    // there is not guarantee that --child-corrrection argument is used before
    // any level of the jet energy corrections file is specified. Therefore,
    // files should be reloaded with ne object.
    //
    shared_ptr<JetEnergyCorrections> jec(new ChildJetEnergyCorrections());
    jec->setCorrectionFiles(_jec->correctionFiles());

    // Activate new Jet Energy Corrections
    //
    stopMonitor(_jec);
    _jec = jec;
    monitor(_jec);
}

// Trigger Delegate interface
//
void SynchSelector::setTrigger(const Trigger &trigger)
{
    _triggers.push_back(trigger.hash());
}

// Selector interface
//
void SynchSelector::enable()
{
}

void SynchSelector::disable()
{
}

// Object inteface
//
uint32_t SynchSelector::id() const
{
    return core::ID<SynchSelector>::get();
}

SynchSelector::ObjectPtr SynchSelector::clone() const
{
    return ObjectPtr(new SynchSelector(*this));
}

void SynchSelector::print(std::ostream &out) const
{
    _cutflow->cut(PRESELECTION)->setName("Pre-Selection");

    string wflavor_;
    switch(Wflavor(wflavor()->value()))
    {
        case WJETS:
            wflavor_ = "W+jet";
            break;

        case WBX:
            wflavor_ = "W+bX";
            break;

        case WCX:
            wflavor_ = "W+cX";
            break;

        case WLIGHT:
            wflavor_ = "W+light";
            break;

        default:
            wflavor_ = "unsupported";
            break;
    }
    _cutflow->cut(WFLAVOR)->setName(wflavor_);
    _cutflow->cut(TRIGGER)->setName("Trigger");
    _cutflow->cut(SCRAPING)->setName("Scraping Veto");
    _cutflow->cut(HBHENOISE)->setName("HBHE Noise");
    _cutflow->cut(PRIMARY_VERTEX)->setName("Good Primary Vertex");
    _cutflow->cut(JET)->setName("2 Good Jets");

    ostringstream lepton;
    lepton << _lepton_mode;

    _cutflow->cut(LEPTON)->setName(string("Good ") + lepton.str());
    _cutflow->cut(VETO_SECOND_ELECTRON)->setName("Veto 2nd electron");
    _cutflow->cut(VETO_SECOND_MUON)->setName("Veto 2nd muon");

    lepton << " " << _cut_mode;
    _cutflow->cut(CUT_LEPTON)->setName(lepton.str());

    _cutflow->cut(LEADING_JET)->setName("Leading Jet");

    ostringstream max_btag;
    max_btag << "btagged jets < " << maxBtag()->value();
    _cutflow->cut(MAX_BTAG)->setName(max_btag.str());

    ostringstream min_btag;
    min_btag << "btagged jets >= " << minBtag()->value();
    _cutflow->cut(MIN_BTAG)->setName(min_btag.str());

    _cutflow->cut(HTLEP)->setName("hTlep");
    _cutflow->cut(TRICUT)->setName("tri-cut");
    _cutflow->cut(MET)->setName("MET");
    _cutflow->cut(RECONSTRUCTION)->setName("reconstruction");
    _cutflow->cut(LTOP)->setName("pt(ltop)");
    _cutflow->cut(CHI2)->setName("Chi2");

    out << "Cutflow [" << _lepton_mode << ": " << _cut_mode << "]" << endl;
    out << *_cutflow << endl;
    out << endl;
}

bool SynchSelector::reconstruction(const bool &value)
{
    if (reconstruction()->isDisabled())
        return true;

    return reconstruction()->apply(value)
        && (_cutflow->apply(RECONSTRUCTION), true);
}

bool SynchSelector::ltop(const float &value)
{
    if (ltop()->isDisabled())
        return true;

    return ltop()->apply(value)
        && (_cutflow->apply(LTOP), true);
}

bool SynchSelector::chi2(const float &value)
{
    if (chi2()->isDisabled())
        return true;

    return chi2()->apply(value)
        && (_cutflow->apply(CHI2), true);
}

// Private
//
bool SynchSelector::splitWflavor(const Event *event)
{
    if (wflavor()->isDisabled())
        return true;

    if (WJETS == wflavor()->value())
        return wflavor()->apply(static_cast<uint32_t>(WJETS)) &&
               (_cutflow->apply(WFLAVOR), true);

    // It is assumed that Wjets sample has W->l+nu (leptonic decay) and
    // all jets are additional generated objects
    //
    //  Wbx     if at least one b-quark is found among jets
    //  Wcx     if there is no b-quark and at least one c-quark is found
    //  Wlight  otherwise
    //
    typedef ::google::protobuf::RepeatedPtrField<Jet> Jets;

    bool wcx = false;
    for(Jets::const_iterator jet = event->jet().begin();
            event->jet().end() != jet;
            ++jet)
    {
        if (!jet->has_gen_parton())
            continue;

        switch(abs(jet->gen_parton().id()))
        {
            case 5: // Wbx
                return wflavor()->apply(static_cast<uint32_t>(WBX)) &&
                       (_cutflow->apply(WFLAVOR), true);

            case 4:
                wcx = true;
                break;
        }
    }

    return wflavor()->apply(static_cast<uint32_t>(wcx ? WCX : WLIGHT)) &&
           (_cutflow->apply(WFLAVOR), true);
}

bool SynchSelector::splitWflavor()
{
    if (wflavor()->isDisabled())
        return true;

    if (WJETS == wflavor()->value())
        return wflavor()->apply(static_cast<uint32_t>(WJETS)) &&
               (_cutflow->apply(WFLAVOR), true);

    // It is assumed that Wjets sample has W->l+nu (leptonic decay) and
    // all jets are additional generated objects
    //
    //  Wbx     if at least one b-quark is found among jets
    //  Wcx     if there is no b-quark and at least one c-quark is found
    //  Wlight  otherwise
    //
    typedef ::google::protobuf::RepeatedPtrField<Jet> Jets;

    bool wcx = false;
    for(GoodJets::const_iterator jet = goodJets().begin();
            goodJets().end() != jet;
            ++jet)
    {
        if (!jet->jet->has_gen_parton())
            continue;

        switch(abs(jet->jet->gen_parton().id()))
        {
            case 5: // Wbx
                return wflavor()->apply(static_cast<uint32_t>(WBX)) &&
                       (_cutflow->apply(WFLAVOR), true);

            case 4:
                wcx = true;
                break;
        }
    }

    return wflavor()->apply(static_cast<uint32_t>(wcx ? WCX : WLIGHT)) &&
           (_cutflow->apply(WFLAVOR), true);
}

bool SynchSelector::triggers(const Event *event)
{
    bool result = _triggers.empty();

    if (!result
            && event->hlt().trigger().size())
    {
        // OR triggers
        //
        typedef ::google::protobuf::RepeatedPtrField<Trigger> PBTriggers;
        for(Triggers::const_iterator trigger = _triggers.begin();
                _triggers.end() != trigger
                    && !result;
                ++trigger)
        {
            for(PBTriggers::const_iterator hlt = event->hlt().trigger().begin();
                    event->hlt().trigger().end() != hlt;
                    ++hlt)
            {
                if (hlt->hash() == *trigger)
                {
                    if (hlt->pass())
                        result = true;

                    break;
                }
            }
        }
    }

    return result
        && (_cutflow->apply(TRIGGER), true);
}

bool SynchSelector::primaryVertices(const Event *event)
{
    selectGoodPrimaryVertices(event);

    return !goodPrimaryVertices().empty()
        && (_cutflow->apply(PRIMARY_VERTEX), true);
}

bool SynchSelector::jets(const Event *event)
{
    selectGoodElectrons(event);
    selectGoodMuons(event);

    // Correct all jets
    //
    typedef ::google::protobuf::RepeatedPtrField<Jet> Jets;

    LockSelectorEventCounterOnUpdate lock_nice_jets(*_nice_jet_selector);
    LockSelectorEventCounterOnUpdate lock_good_jets(*_good_jet_selector);
    const LorentzVector *met = &(event->missing_energy().p4());
    for(Jets::const_iterator jet = event->jet().begin();
            event->jet().end() != jet;
            ++jet)
    {
        CorrectedJet correction = _jec->correctJet(&*jet,
                event,
                _good_electrons,
                _good_muons,
                met);

        // Skip jet if energy corrections failed
        //
        if (!correction.corrected_p4)
            continue;

        _jer->correct(correction);

        met = correction.corrected_met.get();
        _good_met = correction.corrected_met;

        // Original jet in the event can not be modified and Jet Selector can
        // only be applied to jet: therefore copy jet, set corrected p4 and
        // apply selector
        //
        Jet corrected_jet;
        corrected_jet.CopyFrom(*jet);
        corrected_jet.mutable_physics_object()->mutable_p4()->CopyFrom(
                *correction.corrected_p4);

        if (!_nice_jet_selector->apply(corrected_jet))
            continue;

        // Store original jet and corrected p4
        //
        _nice_jets.push_back(correction);

        if (!_good_jet_selector->apply(corrected_jet))
            continue;

        _good_jets.push_back(correction);
    }

    // Sort jets by pT
    //
    sort(_nice_jets.begin(), _nice_jets.end(), CorrectedPtGreater());
    sort(_good_jets.begin(), _good_jets.end(), CorrectedPtGreater());

    return 1 < _good_jets.size()
        && (_cutflow->apply(JET), true);
}

bool SynchSelector::lepton()
{
    return (ELECTRON == _lepton_mode
        ? !_good_electrons.empty()
        : !_good_muons.empty())

        && (_cutflow->apply(LEPTON), true);
}

bool SynchSelector::secondElectronVeto()
{
    return (ELECTRON == _lepton_mode
            ? 1 == _good_electrons.size()
            : _good_electrons.empty())
        && (_cutflow->apply(VETO_SECOND_ELECTRON), true);
}

bool SynchSelector::secondMuonVeto()
{
    return (ELECTRON == _lepton_mode
            ? _good_muons.empty()
            : 1 == _good_muons.size())
        && (_cutflow->apply(VETO_SECOND_MUON), true);
}

bool SynchSelector::isolationAnd2DCut()
{
    if (_cut->isDisabled())
        return true;

    const LorentzVector *lepton_p4 = 0;
    const PFIsolation *lepton_isolation = 0;

    if (ELECTRON == _lepton_mode)
    {
        const Electron *electron = *_good_electrons.begin();

        lepton_p4 = &(electron->physics_object().p4());

        if (electron->has_pf_isolation())
            lepton_isolation = &(electron->pf_isolation());
    }
    else
    {
        const Muon *muon = *_good_muons.begin();

        lepton_p4 = &(muon->physics_object().p4());

        if (muon->has_pf_isolation())
            lepton_isolation = &(muon->pf_isolation());
    }

    bool result = false;
    if (CUT_2D == _cut_mode)
        result = cut2D(lepton_p4);
    else
    {
        if (lepton_isolation)
            result = isolation(lepton_p4, lepton_isolation);
    }

    return _cut->apply(result)
        && (_cutflow->apply(CUT_LEPTON), true);
}

bool SynchSelector::leadingJetCut()
{
    if (leadingJet()->isDisabled())
        return true;

    float max_pt = 0;
    for(GoodJets::const_iterator jet = _good_jets.begin();
            _good_jets.end() != jet;
            ++jet)
    {
        const float jet_pt = pt(*jet->corrected_p4);
        if (jet_pt > max_pt)
            max_pt = jet_pt;
    }

    return leadingJet()->apply(max_pt)
        && (_cutflow->apply(LEADING_JET), true);
}

bool SynchSelector::maxBtags()
{
    if (maxBtag()->isDisabled())
        return true;

    return maxBtag()->apply(countBtaggedJets().first)
        && (_cutflow->apply(MAX_BTAG), true);
}

bool SynchSelector::minBtags()
{
    if (minBtag()->isDisabled())
        return true;

    return minBtag()->apply(countBtaggedJets().first)
        && (_cutflow->apply(MIN_BTAG), true);
}

bool SynchSelector::htlepCut(const Event *event)
{
    if (htlep()->isDisabled())
        return true;

    const LorentzVector &lepton_p4 = (ELECTRON == _lepton_mode 
        ? (*_good_electrons.begin())->physics_object().p4()
        : (*_good_muons.begin())->physics_object().p4());

    return goodMET()
        && htlep()->apply(pt(*goodMET()) + pt(lepton_p4))
        && (_cutflow->apply(HTLEP), true);
}

bool SynchSelector::triangularCut(const Event *event)
{
    if (tricut()->isDisabled())
        return true;

    if (!goodMET())
        return false;

    const LorentzVector &met = *goodMET();

    const float met_pt = pt(met);

    const float dphi_el_met =
        fabs(dphi(goodElectrons()[0]->physics_object().p4(), met));

    const float dphi_ljet_met =
        fabs(dphi(*goodJets()[0].corrected_p4, met));

    const float slope = 1.5 / 75;

    bool pass = dphi_el_met < (slope * met_pt + 1.5)
        && dphi_el_met > (-slope * met_pt + 1.5)
        && dphi_ljet_met < (slope * met_pt + 1.5)
        && dphi_ljet_met > (-slope * met_pt + 1.5)
        && (_cutflow->apply(TRICUT), true);
   
    return tricut()->isInverted() ? !pass : pass;
}

bool SynchSelector::missingEnergy(const Event *event)
{
    if (met()->isDisabled())
        return true;

    return goodMET()
        && met()->apply(pt(*goodMET()))
        && (_cutflow->apply(MET), true);
}

bool SynchSelector::cut2D(const LorentzVector *lepton_p4)
{
    if (_nice_jets.empty())
        return true;

    GoodJets::const_iterator closest_jet = _nice_jets.end();
    float deltar_min = 999999;

    for(GoodJets::const_iterator jet = _nice_jets.begin();
            _nice_jets.end() != jet;
            ++jet)
    {
        const float deltar = dr(*lepton_p4, *jet->corrected_p4);
        if (deltar < deltar_min)
        {
            deltar_min = deltar;
            closest_jet = jet;
        }
    }

    _closest_jet = closest_jet;

    if (_nice_jets.end() == closest_jet)
        return true;

    return _cut2d_selector->apply(*lepton_p4, *closest_jet->corrected_p4);
}

bool SynchSelector::isolation(const LorentzVector *p4, const PFIsolation *isolation)
{
    return 0.5 < (isolation->charged_hadron()
            + isolation->neutral_hadron()
            + isolation->photon())
        / pt(*p4);
}

void SynchSelector::invalidate_cache()
{
    _btags.invalidate();
}

void SynchSelector::selectGoodPrimaryVertices(const Event *event)
{
    typedef ::google::protobuf::RepeatedPtrField<PrimaryVertex> PrimaryVertices;

    LockSelectorEventCounterOnUpdate lock(*_primary_vertex_selector);
    for(PrimaryVertices::const_iterator pv = event->primary_vertex().begin();
            event->primary_vertex().end() != pv;
            ++pv)
    {
        if (_primary_vertex_selector->apply(*pv))
            _good_primary_vertices.push_back(&*pv);
    }
}

void SynchSelector::selectGoodElectrons(const Event *event)
{
    typedef ::google::protobuf::RepeatedPtrField<Electron> Electrons;

    const PrimaryVertex &pv = *event->primary_vertex().begin();

    LockSelectorEventCounterOnUpdate lock(*_electron_selector);
    for(Electrons::const_iterator electron = event->electron().begin();
            event->electron().end() != electron;
            ++electron)
    {
        if (!_electron_selector->apply(*electron, pv))
            continue;

        typedef ::google::protobuf::RepeatedPtrField<Electron::ElectronID>
            ElectronIDs;

        bool is_good_lepton = false;
        for(ElectronIDs::const_iterator id = electron->id().begin();
                electron->id().end() != id;
                ++id)
        {
            if (Electron::HyperTight1 != id->name())
                continue;

            if (qcdTemplate())
            {
                if (!id->identification() || !id->conversion_rejection())
                    is_good_lepton = true;
            } 
            else
            {
                if (id->identification() && id->conversion_rejection())
                    is_good_lepton = true;
            }

            break;
        }

        if (is_good_lepton)
            _good_electrons.push_back(&*electron);
    }
}

void SynchSelector::selectGoodMuons(const Event *event)
{
    typedef ::google::protobuf::RepeatedPtrField<Muon> Muons;

    const PrimaryVertex &pv = *event->primary_vertex().begin();

    LockSelectorEventCounterOnUpdate lock(*_muon_selector);
    for(Muons::const_iterator muon = event->muon().begin();
            event->muon().end() != muon;
            ++muon)
    {
        if (_muon_selector->apply(*muon, pv))
            _good_muons.push_back(&*muon);
    }
}



// Helpers
//
std::ostream &bsm::operator <<(std::ostream &out,
        const SynchSelectorDelegate::LeptonMode &lepton_mode)
{
    switch(lepton_mode)
    {
        case SynchSelector::ELECTRON:   out << "Electron";
                                        break;

        case SynchSelector::MUON:       out << "Muon";
                                        break;

        default:                        out << "unknown";
                                        break;
    }

    return out;
}

std::ostream &bsm::operator <<(std::ostream &out,
        const SynchSelectorDelegate::CutMode &cut_mode)
{
    switch(cut_mode)
    {
        case SynchSelector::CUT_2D:     out << "2D Cut";
                                        break;

        case SynchSelector::ISOLATION:  out << "Isolation";
                                        break;

        default:                        out << "unknown";
                                        break;
    }

    return out;
}
