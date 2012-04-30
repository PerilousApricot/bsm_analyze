#!/usr/bin/env python

'''
Created by Samvel Khalatyan, Mar 13, 2012
Copyright 2011, All rights reserved
'''

from __future__ import division, print_function

import math
import sys

import root.label
import root.style
import ROOT

import template.templates

from template import channel_type

from template.channel_template import MCChannelTemplate
from root.tfile import topen
from root.hist_decorator import calculate_efficiency
from util.timer import Timer
from util.arg import split_use_and_ban

class Comparator(template.templates.Templates):
    channels = set([
        "zprime_m1000_w10",
        "zprime_m2000_w20",
        "zprime_m3000_w30",

        "zprime_m1000_w100",
        "zprime_m2000_w200",
        "zprime_m3000_w300",

        "rsgluon_m1000",
        "rsgluon_m2000",
        "rsgluon_m3000",
    ])

    channel_names = template.templates.Templates.channel_names

    def __init__(self, options, args):
        options.plots = "mass"
        options.folders = "htop_1jet,htop_2jet,htop_3jet"

        template.templates.Templates.__init__(self, options, args)

    def _process(self):
        if not self.use_channels:
            raise RuntimeError("all channels are turned off")

        self._load_channels()

        canvases = self._plot()

        # Save canvases
        for obj in canvases:
            obj.SaveAs(self._canvas_template.format(
                obj.GetName()))

        if canvases and not self._batch_mode:
            raw_input('enter')

    @Timer(label = "[plot templates]", verbose = True)
    def _plot(self):
        canvases = []

        htop_1jet = self.loader.plots["/htop_1jet/mass"]
        htop_2jet = self.loader.plots["/htop_2jet/mass"]
        htop_3jet = self.loader.plots["/htop_3jet/mass"]

        # process Z'
        for channel_type, template in htop_1jet.items():
            if (not channel_type.startswith("zp") and
                not channel_type.startswith("rsg")):
                continue

            print(channel_type)

            h1 = template.hist
            h2 = htop_2jet[channel_type].hist
            h3 = htop_3jet[channel_type].hist

            for hist in h1, h2, h3:
                hist.SetFillStyle(0)
                hist.SetLineStyle(1)
                hist.Rebin(5)
                hist.GetYaxis().SetTitle("event yield / {0:.1f} GeV/c^".format(
                                         hist.GetBinWidth(1)) +
                                         "{2}")
                hist.GetXaxis().SetTitle("M_{htop} [GeV/c^{2}]")
                hist.SetNdivisions(5, "x")
                hist.SetNdivisions(3, "y")

            h1.SetLineColor(ROOT.kBlack)
            h2.SetLineColor(ROOT.kRed + 1)
            h3.SetLineColor(ROOT.kGreen + 1)

            canvas = ROOT.TCanvas()
            canvas.SetWindowSize(640, 640)
            canvas.SetName(channel_type)
            #canvas.Divide(2, 2)

            for pad in range(1, 2):
                pad = canvas.cd(pad)
                pad.SetRightMargin(5)
                pad.SetBottomMargin(0.15)

            for key, h in enumerate([h1, h2, h3], 1):
                print("{0} jets: {1:>4.1f}".format(key, h.Integral()))

            hists = [h.Clone() for h in (h1, h2, h3) if h.Integral()]
            
            is_drawn = False
            for h in hists:
                h.Scale(1 / h.Integral())
                if is_drawn:
                    h.Draw("hist 9 same")
                else:
                    h.Draw("hist 9")
                    is_drawn = True

            hists[0].SetMaximum(0.2)
            canvas.hists = hists

            legend = ROOT.TLegend(.65, .7, .85, .85)
            legend.SetHeader(channel_type)
            legend.SetMargin(0.12);
            legend.SetTextSize(0.03);
            legend.SetFillColor(10);
            legend.SetBorderSize(0);

            legend.AddEntry(h1, "htop 1 jets", "l")
            legend.AddEntry(h2, "htop 2 jets", "l")
            legend.AddEntry(h3, "htop 3+jets", "l")
            legend.Draw("9")

            canvas.legend = legend
            canvas.Update()
            
            canvases.append(canvas)

            print()

        return canvases

    def __str__(self):
        result = []

        result.append(["verbose", self._verbose])
        result.append(["batch mode", self._batch_mode])
        result.append(["input filename", self._input_filename])
        result.append(["channels", self.use_channels])

        return '\n'.join("{0:>15}: {1}".format(name, value)
                         for name, value in result)
