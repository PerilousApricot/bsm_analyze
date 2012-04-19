#!/usr/bin/env python

'''
Created by Samvel Khalatyan, Mar 12, 2012
Copyright 2011, All rights reserved
'''

from __future__ import print_function,division

import math
import sys

import root.label
import root.style
import ROOT

import template.templates

from template import channel_type

import template.compare as compare
from template.channel_template import MCChannelTemplate
from root.comparison import ComparisonCanvas
from root.tfile import topen
from root.hist_decorator import calculate_efficiency
from util.timer import Timer
from util.arg import split_use_and_ban

class Templates(template.templates.Templates):
    channel_names = {
            # Map channel type to channel name to be used in plot name

            "ttbar": "ttbar",
            "ttbar_powheg": "ttbar_powheg",
            "zjets": "zjets",
            "wjets": "wjets",
            "stop": "singletop",
            "qcd": "eleqcd",
            "data": "DATA",

            "zprime_m1000_w10": "zp1000",
            "zprime_m1500_w15": "zp1500",
            "zprime_m2000_w20": "zp2000",
            "zprime_m3000_w30": "zp3000",
            "zprime_m4000_w40": "zp4000",

            "ttbar_matching_plus": "ttbar",
            "ttbar_matching_minus": "ttbar",

            "ttbar_scaling_plus": "ttbar",
            "ttbar_scaling_minus": "ttbar",

            "wjets_matching_plus": "wjets",
            "wjets_matching_minus": "wjets",

            "wjets_scaling_plus": "wjets",
            "wjets_scaling_minus": "wjets",
    }

    def __init__(self, options, args):
        options.plots = "mttbar_after_htlep"
        options.folders = ""
        options.channels = "ttbar,ttbar_powheg"

        template.templates.Templates.__init__(
                self,
                options, args,
                disable_systematics=True)

    def _process(self):
        if not self.use_channels:
            raise RuntimeError("all channels are turned off")

        self._load_channels()
        self._run_fraction_fitter()
        self._apply_fractions()
        self._apply_scales()

        canvases = self._plot()

        # Save canvases
        for obj in canvases:
            obj.canvas.SaveAs(self._canvas_template.format(
                obj.canvas.GetName()))

        if canvases and not self._batch_mode:
            raw_input('enter')

    @Timer(label = "[plot templates]", verbose = True)
    def _plot(self):
        class Canvas: pass

        canvases = []

        '''
        loop over plots and draw them:

            1. all background channels stacked (MC channels + QCD)
            2. background error band (MC + QCD)
            3. data with errors
            4. ratio of data over background
        '''

        # loop over plots
        for plot_name, channels in self.loader.plots.items():
            ttbar = channels["ttbar"]
            ttbar_powheg = channels["ttbar_powheg"]

            for hist in ttbar.hist, ttbar_powheg.hist:
                hist.SetFillStyle(0)

            obj = Canvas()
            obj.canvas_obj = ComparisonCanvas()
            obj.canvas = obj.canvas_obj.canvas

            obj.canvas.cd(2)

            obj.ratio = compare.ratio(ttbar.hist,
                                      ttbar_powheg.hist,
                                      title = "#frac{MADGRAPH}{POWHEG}")
            obj.ratio.SetMinimum(0)
            obj.ratio.SetMaximum(2)

            obj.ratio.GetXaxis().SetTitle("")
            obj.ratio.Draw("9 e")

            obj.canvas.SetName("canvas_" + plot_name.replace("/", "_"))
            obj.canvas.cd(1)

            ttbar.hist.Draw('9 hist')
            ttbar_powheg.hist.Draw('9 hist same')

            legend = ROOT.TLegend(.67, .60, .89, .75)
            legend.SetMargin(0.12);
            legend.SetTextSize(0.03);
            legend.SetFillColor(10);
            legend.SetBorderSize(0);
            legend.SetHeader("t#bar{t} comparison")

            legend.AddEntry(ttbar.hist, "MADGRAPH", "l")
            legend.AddEntry(ttbar_powheg.hist, "POWHEG", "l")

            obj.legend = legend
            obj.legend.Draw('9')

            obj.canvas.Update()
            canvases.append(obj)

            print("Powheg/Madgraph: {0:.2f}".format(
                ttbar_powheg.hist.Integral() / ttbar.hist.Integral()))

        return canvases

    def __str__(self):
        '''
        Print Templates object configuraiton
        '''

        result = []

        return "{0}\n{1}".format(
                template.templates.Templates.__str__(self),
                '\n'.join(self._str_format.format(name, value)
                          for name, value in result))
