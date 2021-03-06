// Load and plot Scaling systematic
//
// Created by Samvel Khalatyan, Nov 18, 2011
// Copyright 2011, All rights reserved

#include <iostream>
#include <utility>

#include <TCanvas.h>
#include <TFile.h>
#include <TH1.h>
#include <TObject.h>
#include <TSystem.h>

#include "interface/ScalingSystematic.h"

using namespace std;

void ScalingSystematic::draw()
{
    plot(Input::TTJETS);
    plot(Input::WJETS);
}

void ScalingSystematic::load(Plots &plots,
        const string &suffix,
        const int &systematic)
{
    typedef pair<Input, Input> InputMap;
    typedef vector<InputMap> Inputs;

    Inputs inputs;
    switch(systematic)
    {
        case 1:
            inputs.push_back(make_pair(Input(Input::TTJETS_SCALE_UP),
                        Input(Input::TTJETS)));
            inputs.push_back(make_pair(Input(Input::WJETS_SCALE_UP),
                        Input(Input::WJETS)));
            break;

        case -1:
            inputs.push_back(make_pair(Input(Input::TTJETS_SCALE_DOWN),
                        Input(Input::TTJETS)));
            inputs.push_back(make_pair(Input(Input::WJETS_SCALE_DOWN),
                        Input(Input::WJETS)));
            break;

        default:
            inputs.push_back(make_pair(Input(Input::TTJETS),
                        Input(Input::TTJETS)));
            inputs.push_back(make_pair(Input(Input::WJETS),
                        Input(Input::WJETS)));
            break;
    };

    for(Inputs::const_iterator input = inputs.begin();
            inputs.end() != input;
            ++input)
    {
        TH1 *histogram = loadPlot(input->first, suffix, plots);
        if (!histogram)
            continue;

        style(histogram, systematic);

        // store plot
        //
        plots[input->second.type()] = histogram;
    }
}

void ScalingSystematic::plot(const Input::Type &type)
{
    if (_systematic_none.end() != _systematic_none.find(type)
            && _systematic_plus.end() != _systematic_plus.find(type)
            && _systematic_minus.end() != _systematic_minus.find(type))
    {
        Input input(type);
        TCanvas *canvas = Systematic::draw(input,
                _systematic_none[type],
                _systematic_plus[type],
                _systematic_minus[type]);

        canvas->SaveAs(("systematic_" + input.repr() + ".pdf").c_str());
    }
    else
    {
        cerr << "can not plot input: " << Input(type) << endl;
    }
}

void ScalingSystematic::style(TH1 *hist, const int &systematic)
{
    int color = 1;
    switch(systematic)
    {
        case 1:
            color = 2;
            break;

        case -1:
            color = 8;
            break;

        default:
            break;
    }

    hist->SetLineColor(color);
    hist->SetMarkerColor(color);
    hist->SetMarkerSize(0.5);
    hist->SetLineWidth(2);
}

TH1 *ScalingSystematic::loadPlot(const Input &input, const string &suffix, Plots &plots)
{
    FileStat_t buf;

    string filename = folder(input) + "/" + file_mask() + suffix + ".root";
    if (gSystem->GetPathInfo(filename.c_str(), buf))
    {
        cerr << "skip file: " << filename << endl;

        return 0;
    }

    TFile *file = TFile::Open(filename.c_str());
    if (!file
            || !file->IsOpen())
    {
        cerr << "failed to open: " << filename << endl;

        return 0;
    }

    // extract histogram
    //
    TObject *object = file->Get(plotname().c_str());
    if (!object)
    {
        cerr << "failed to extract plot: " << plotname()
            << " from: " << filename << endl;

        return 0;
    }

    TH1 *histogram = dynamic_cast<TH1 *>(object->Clone());
    if (!histogram)
    {
        cerr << "extracted plot: " << plotname()
            << " is not TH1" << endl;

        return 0;
    }

    scale(histogram, input);

    histogram->Rebin(100);
    
    return histogram;
}
