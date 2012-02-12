// Study hadronic top
//
// Created by Samvel Khalatyan, Feb 09, 2011
// Copyright 2011, All rights reserved

#include <iostream>
#include <stdexcept>

#include <boost/shared_ptr.hpp>

#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TRint.h>

#include "bsm_stat/interface/Utility.h"
#include "interface/Algorithm.h"
#include "interface/AppController.h"
#include "interface/JetEnergyCorrections.h"
#include "interface/MonitorCanvas.h"
#include "interface/Pileup.h"
#include "interface/HadronicTopAnalyzer.h"
#include "interface/TriggerAnalyzer.h"
#include "interface/SynchSelector.h"

using namespace std;
using namespace boost;
using namespace bsm;

int main(int argc, char *argv[])
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    bool result = false;
    try
    {
        boost::shared_ptr<HadronicTopAnalyzer> analyzer(new HadronicTopAnalyzer());
        boost::shared_ptr<AppController> app(new AppController());

        boost::shared_ptr<JetEnergyCorrectionOptions> jec_options(new JetEnergyCorrectionOptions());
        boost::shared_ptr<SynchSelectorOptions> synch_selector_options(new SynchSelectorOptions());
        boost::shared_ptr<TriggerOptions> trigger_options(new TriggerOptions());
        boost::shared_ptr<PileupOptions> pileup_options(new PileupOptions());
        boost::shared_ptr<HadronicTopOptions> htop_options(new HadronicTopOptions());

        jec_options->setDelegate(analyzer->getJetEnergyCorrectionDelegate());
        synch_selector_options->setDelegate(analyzer->getSynchSelectorDelegate());
        trigger_options->setDelegate(analyzer->getTriggerDelegate());
        pileup_options->setDelegate(analyzer->getPileupDelegate());
        htop_options->setDelegate(analyzer.get());

        app->addOptions(*jec_options);
        app->addOptions(*synch_selector_options);
        app->addOptions(*trigger_options);
        app->addOptions(*pileup_options);
        app->addOptions(*htop_options);

        app->setAnalyzer(analyzer);

        result = app->run(argc, argv);
        if (result && app->output())
        {
            typedef bsm::stat::TH1Ptr TH1Ptr;
            typedef bsm::stat::TH2Ptr TH2Ptr;

            int empty_argc = 3;
            char *empty_argv[] = { argv[0], "-b", "-q" };

            boost::shared_ptr<TRint>
                root(new TRint("app", &empty_argc, empty_argv));

            TH1Ptr njets = convert(*analyzer->njets());
            njets->SetName("njets");
            njets->Write();

            TH2Ptr njets_vs_mass = convert(*analyzer->njets_vs_mass());
            njets_vs_mass->SetName("njets_vs_mass");
            njets_vs_mass->Write();

            TH2Ptr pt_vs_mass = convert(*analyzer->pt_vs_mass());
            pt_vs_mass->SetName("pt_vs_mass");
            pt_vs_mass->Write();

            TH2Ptr njets_vs_pt = convert(*analyzer->njets_vs_pt());
            njets_vs_pt->SetName("njets_vs_pt");
            njets_vs_pt->Write();

            shared_ptr<P4Canvas> top(new P4Canvas("top", "top"));
            top->write(*analyzer->top(), app->output().get());

            shared_ptr<P4Canvas> jet1(new P4Canvas("jet1", "jet1"));
            jet1->write(*analyzer->jet1(), app->output().get());

            shared_ptr<P4Canvas> jet2(new P4Canvas("jet2", "jet2"));
            jet2->write(*analyzer->jet2(), app->output().get());

            shared_ptr<P4Canvas> jet3(new P4Canvas("jet3", "jet3"));
            jet3->write(*analyzer->jet3(), app->output().get());

            shared_ptr<P4Canvas> jet4(new P4Canvas("jet4", "jet4"));
            jet4->write(*analyzer->jet4(), app->output().get());

            shared_ptr<GenParticleCanvas> jet1_parton(new GenParticleCanvas("jet1_parton", "jet1_parton"));
            jet1_parton->write(*analyzer->jet1_parton(), app->output().get());

            shared_ptr<DeltaCanvas> jet1_vs_jet2(new DeltaCanvas("jet1_vs_jet2"));
            jet1_vs_jet2->write(*analyzer->jet1_vs_jet2(), app->output().get());

            shared_ptr<DeltaCanvas> jet1_vs_jet3(new DeltaCanvas("jet1_vs_jet3"));
            jet1_vs_jet3->write(*analyzer->jet1_vs_jet3(), app->output().get());

            shared_ptr<DeltaCanvas> jet1_vs_jet4(new DeltaCanvas("jet1_vs_jet4"));
            jet1_vs_jet4->write(*analyzer->jet1_vs_jet4(), app->output().get());

            shared_ptr<DeltaCanvas> jet2_vs_jet3(new DeltaCanvas("jet2_vs_jet3"));
            jet2_vs_jet3->write(*analyzer->jet2_vs_jet3(), app->output().get());

            shared_ptr<DeltaCanvas> jet2_vs_jet4(new DeltaCanvas("jet2_vs_jet4"));
            jet2_vs_jet4->write(*analyzer->jet2_vs_jet4(), app->output().get());

            shared_ptr<DeltaCanvas> jet3_vs_jet4(new DeltaCanvas("jet3_vs_jet4"));
            jet3_vs_jet4->write(*analyzer->jet3_vs_jet4(), app->output().get());
        }
    }
    catch(const std::exception &error)
    {
        cerr << error.what() << endl;

        result = false;
    }
    catch(...)
    {
        cerr << "Unknown error" << endl;

        result = false;
    }

    // Clean Up any memory allocated by libprotobuf
    //
    google::protobuf::ShutdownProtobufLibrary();

    return result
        ? 0
        : 1;
}