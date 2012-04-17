#!/usr/bin/env python

'''
Created by Samvel Khalatyan, Apr 05, 2012
Copyright 2011, All rights reserved
'''

from __future__ import print_function

import re
import sys

from array import array
from root.tfile import topen
from root.comparison import ComparisonCanvas
from root.style import tdr

def plot(label, data, all_jets, tagged_jets, channel):
    canvas = ComparisonCanvas(pads=1)

    xaxis = tagged_jets.GetXaxis()

    channel_title = re.sub("[-\(\),]", '_', label)

    tagged = None
    all_ = None
    for pdgid in data["partons"]:
        bin_ = xaxis.FindBin(pdgid)
        plot = tagged_jets.ProjectionY("{0}_tagged".format(channel_title), bin_, bin_).Clone()

        if not tagged:
            tagged = plot
        else:
            tagged.Add(plot)

        plot = all_jets.ProjectionY("{0}_all".format(channel_title), bin_, bin_).Clone()

        if not all_:
            all_ = plot
        else:
            all_.Add(plot)

    bins = array('d', (30, 40, 50, 60, 70, 80, 100, 120, 160, 210, 260, 320, 400, 500, 670))
    efficiency = tagged.Rebin(len(bins) - 1, "{0}_efficiency".format(channel_title), bins)
    efficiency.Divide(all_.Rebin(len(bins) - 1, "{0}_all_rebin".format(channel_title), bins))

    efficiency.GetXaxis().SetTitle("p_{T}^{jet} [GeV]")
    efficiency.GetYaxis().SetTitle(data["y-title"])
    efficiency.SetMaximum(data["range"][1])
    efficiency.SetMinimum(data["range"][0])

    xaxis = efficiency.GetXaxis()
    print(label)
    for bin_ in range(1, xaxis.GetNbins() + 1):
        #print(efficiency.GetBinContent(bin_), end=', ')
        print("{0:>2}: {1}".format(xaxis.GetBinUpEdge(bin_), efficiency.GetBinContent(bin_)))

    print()

    canvas.plot = efficiency

    efficiency.Draw("9 e1")

    canvas.canvas.Update()

    canvas.canvas.SaveAs("{0}_{1}.pdf".format(channel, channel_title))

    return canvas

def main():
    style = tdr()
    style.cd()

    for channel in sys.argv[1:]:
        with topen(channel + '/output_signal_p150_hlt.root') as _input:
            all_jets = _input.Get("parton_jets")
            if not all_jets:
                raise RuntimeError("failed to load all jets plot")

            tagged_jets = _input.Get("btagged_parton_jets")
            if not tagged_jets:
                raise RuntimeError("failed to load tagged jets plot")

            canvases = []
            for label, data in {"mistag(u,d,s)": {
                                    "partons": [1, 2, 3],
                                    "range": [0, 0.1],
                                    "y-title": "mistag rate (u,d,s)"
                                    },

                                  "mistag(u,d,s,g)": {
                                      "partons": [1, 2, 3, 21],
                                      "range": [0, 0.1],
                                      "y-title": "mistag rate (u,d,s,g)"
                                      },

                                  "b-tag": {
                                      "partons": [4, 5],
                                      "range": [0, 0.7],
                                      "y-title": "#epsilon_{b}"
                                      }
                                  }.items():
                canvases.append(plot(label, data, all_jets, tagged_jets, channel))


            raw_input("enter")

if "__main__" == __name__:
    main()
