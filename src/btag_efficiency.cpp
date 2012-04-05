// Measure b-tagging efficiency
//
// Created by Samvel Khalatyan, Apr 05, 2012
// Copyright 2011, All rights reserved

#include <iostream>
#include <stdexcept>

#include <boost/shared_ptr.hpp>

#include <TFile.h>
#include <TGaxis.h>
#include <TH1.h>
#include <TH2.h>
#include <TRint.h>

#include "bsm_stat/interface/Utility.h"
#include "interface/AppController.h"
#include "interface/Btag.h"
#include "interface/JetEnergyCorrections.h"
#include "interface/Pileup.h"
#include "interface/BtagEfficiencyAnalyzer.h"
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
        boost::shared_ptr<BtagEfficiencyAnalyzer> analyzer(new BtagEfficiencyAnalyzer());
        boost::shared_ptr<AppController> app(new AppController());

        boost::shared_ptr<JetEnergyCorrectionOptions> jec_options(new JetEnergyCorrectionOptions());
        boost::shared_ptr<SynchSelectorOptions> synch_selector_options(new SynchSelectorOptions());
        boost::shared_ptr<TriggerOptions> trigger_options(new TriggerOptions());
        boost::shared_ptr<PileupOptions> pileup_options(new PileupOptions());

        jec_options->setDelegate(analyzer->getJetEnergyCorrectionDelegate());
        synch_selector_options->setDelegate(analyzer->getSynchSelectorDelegate());
        trigger_options->setDelegate(analyzer->getTriggerDelegate());
        pileup_options->setDelegate(analyzer->getPileupDelegate());

        app->addOptions(*jec_options);
        app->addOptions(*synch_selector_options);
        app->addOptions(*trigger_options);
        app->addOptions(*pileup_options);

        app->setAnalyzer(analyzer);

        result = app->run(argc, argv);
        if (result && app->output())
        {
            typedef bsm::stat::TH1Ptr TH1Ptr;
            typedef bsm::stat::TH2Ptr TH2Ptr;

            int empty_argc = 1;
            char *empty_argv[] = { argv[0] };

            boost::shared_ptr<TRint>
                root(new TRint("app", &empty_argc, empty_argv));

            TGaxis::SetMaxDigits(3);

            TH1Ptr parton_jets = convert(*analyzer->parton_jets());
            parton_jets->SetName("parton_jets");
            parton_jets->Write();

            TH1Ptr btagged_parton_jets = convert(*analyzer->btagged_parton_jets());
            btagged_parton_jets->SetName("btagged_parton_jets");
            btagged_parton_jets->Write();
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
