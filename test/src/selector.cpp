// Test BSM Selectors
//
// Open inputs, apply selectors and monitor selected objects
//
// Created by Samvel Khalatyan, Apr 22, 2011
// Copyright 2011, All rights reserved

#include <iostream>

#include <boost/shared_ptr.hpp>

#include <TCanvas.h>
#include <TH1.h>
#include <TRint.h>

#include "bsm_input/interface/Reader.h"
#include "bsm_input/interface/Event.pb.h"
#include "bsm_stat/interface/H1.h"
#include "bsm_stat/interface/Utility.h"
#include "interface/Monitor.h"
#include "interface/MonitorCanvas.h"
#include "interface/Selector.h"

using namespace std;
using namespace boost;
using namespace bsm;

shared_ptr<ElectronsMonitor> all_electron;
shared_ptr<ElectronsMonitor> selected_electron;

shared_ptr<MuonsMonitor> all_muon;
shared_ptr<MuonsMonitor> selected_muon;

typedef ElectronsMonitor::Electrons Electrons;
typedef MuonsMonitor::Muons Muons;

void plot();

int main(int argc, char *argv[])
try
{
    if (2 > argc)
    {
        cerr << "Usage: " << argv[0] << " input.pb" << endl;

        return 0;
    }

    GOOGLE_PROTOBUF_VERIFY_VERSION;

    all_electron.reset(new ElectronsMonitor());
    selected_electron.reset(new ElectronsMonitor());

    all_muon.reset(new MuonsMonitor());
    selected_muon.reset(new MuonsMonitor());

    {
        shared_ptr<ElectronSelector> el_selector(new ElectronSelector());
        shared_ptr<ElectronSelector> test_el_selector(new ElectronSelector());

        shared_ptr<MuonSelector> mu_selector(new MuonSelector());
        shared_ptr<MuonSelector> test_mu_selector(new MuonSelector());

        for(int i = 1; argc > i; ++i)
        {
            shared_ptr<ElectronSelector> per_file_el_selector(new ElectronSelector());
            shared_ptr<MuonSelector> per_file_mu_selector(new MuonSelector());

            shared_ptr<Reader> reader(new Reader(argv[i]));
            reader->open();

            if (!reader->isOpen())
                continue;

            uint32_t events_read = 0;
            for(shared_ptr<Event> event(new Event());
                    reader->read(event);
                    ++events_read)
            {
                if (!event->primary_vertex().size())
                    continue;

                const PrimaryVertex &pv = *event->primary_vertex().begin();

                if (event->electron().size())
                {
                    all_electron->fill(event->electron());

                    Electrons selected_electrons;
                    for(Electrons::const_iterator electron = event->electron().begin();
                            event->electron().end() != electron;
                            ++electron)
                    {
                        per_file_el_selector->apply(*electron, pv);
                        if (el_selector->apply(*electron, pv))
                            *selected_electrons.Add() = *electron;
                    }

                    if (!selected_electrons.size())
                        continue;

                    selected_electron->fill(selected_electrons);
                }

                if (event->muon().size())
                {
                    all_muon->fill(event->muon());

                    Muons selected_muons;
                    for(Muons::const_iterator muon = event->muon().begin();
                            event->muon().end() != muon;
                            ++muon)
                    {
                        per_file_mu_selector->apply(*muon, pv);
                        if (mu_selector->apply(*muon, pv))
                            *selected_muons.Add() = *muon;
                    }

                    if (!selected_muons.size())
                        continue;

                    selected_muon->fill(selected_muons);
                }

                event->Clear();
            }

            test_el_selector->merge(per_file_el_selector);
            test_mu_selector->merge(per_file_mu_selector);

            cout << "Events Read: " << events_read << endl;
        }

        cout << "Original Electron Selector" << endl;
        cout << *el_selector << endl;

        cout << "Test Electron Selector" << endl;
        cout << *test_el_selector << endl;

        cout << "Original Muon Selector" << endl;
        cout << *mu_selector << endl;

        cout << "Test Muon Selector" << endl;
        cout << *test_mu_selector << endl;
    }

    cout << "All PF Electrons" << endl;
    cout << *all_electron << endl;
    cout << endl;
    cout << "Selected PF Electrons" << endl;
    cout << *selected_electron << endl;

    cout << "All PF Muons" << endl;
    cout << *all_muon << endl;
    cout << endl;
    cout << "Selected PF Muons" << endl;
    cout << *selected_muon << endl;

    plot();

    // Clean Up any memory allocated by libprotobuf
    //
    google::protobuf::ShutdownProtobufLibrary();

    return 0;
}
catch(...)
{
    // Clean Up any memory allocated by libprotobuf
    //
    google::protobuf::ShutdownProtobufLibrary();

    cerr << "Unknown error" << endl;

    return 1;
}

void plot()
{
    // Cheat ROOT with empty args
    //
    int empty_argc = 1;
    char *empty_argv[] = { "root" };
    shared_ptr<TRint> app(new TRint("app", &empty_argc, empty_argv));

    shared_ptr<ElectronCanvas> all_electron_canvas(new ElectronCanvas("All PF Electrons"));
    all_electron_canvas->draw(*all_electron);

    shared_ptr<ElectronCanvas> selected_pf_electron_canvas(new ElectronCanvas("Selected PF Electrons"));
    selected_pf_electron_canvas->draw(*selected_electron);

    shared_ptr<MuonCanvas> all_muon_canvas(new MuonCanvas("All PF Muons"));
    all_muon_canvas->draw(*all_muon);

    shared_ptr<MuonCanvas> selected_pf_muon_canvas(new MuonCanvas("Selected PF Muons"));
    selected_pf_muon_canvas->draw(*selected_muon);

    app->Run();
}
