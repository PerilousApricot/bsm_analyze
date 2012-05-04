#!/usr/bin/env python

'''
Created by Samvel Khalatyan, Mar 12, 2012
Copyright 2011, All rights reserved
'''

from __future__ import print_function

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

class Templates(template.templates.Templates):
    channel_names = {
            # Map channel type to channel name to be used in plot name

            "ttbar": "ttbar",
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
        options.plots = "chi2"
        options.folders = ""

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

    def _create_obj(self, name):
        # container where all canvas related objects will be saved
        class Object: pass

        obj = Object()

        obj.canvas = ROOT.TCanvas()
        obj.legend = ROOT.TLegend(.3, .7, .5, .85)
        obj.legend.SetMargin(0.12);
        obj.legend.SetTextSize(0.03);
        obj.legend.SetFillColor(10);
        obj.legend.SetBorderSize(0);

        obj.canvas.SetName(name)
        obj.canvas.SetWindowSize(640, 640)

        pad = obj.canvas.cd(1)
        pad.SetRightMargin(5)
        pad.SetBottomMargin(0.15)

        obj.hist = []

        obj.labels = [
                root.label.CMSSimulationLabel(text_size=0.033),
                root.label.LuminosityLabel(1000)]
        obj.labels[1].label = "e+jets"

        return obj

    @Timer(label="[plot templates]", verbose=True)
    def _plot(self):
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
            # process Z'
            obj = None
            for channel_type, template in channels.items():
                if not channel_type.startswith("zp"):
                    continue

                hist = calculate_efficiency(template.hist)
                hist.SetFillStyle(0)
                hist.SetMaximum(1.2)
                hist.SetMinimum(0)
                hist.GetXaxis().SetRangeUser(0, 20)

                if not obj:
                    obj = self._create_obj("canvas_signal_chi2")
                    hist.Draw('hist 9')
                else:
                    hist.Draw("hist 9 same")

                obj.legend.AddEntry(hist, channel_type, "l")

                obj.hist.append(hist)

            if obj:
                obj.legend.Draw('9')

                for label in obj.labels:
                    label.draw()

                canvases.append(obj)

            # take care of background
            obj = None
            for channel_type, template in channels.items():
                if (channel_type.startswith("zp") or
                    channel_type not in set(channels.keys()) -
                                        set(["data", "mc"])):
                    continue

                hist = calculate_efficiency(template.hist)
                hist.SetFillStyle(0)
                hist.SetMaximum(1.2)
                hist.SetMinimum(0)
                hist.GetXaxis().SetRangeUser(0, 20)

                if not obj:
                    obj = self._create_obj("canvas_background_chi2")
                    hist.Draw('hist 9')
                else:
                    hist.Draw("hist 9 same")

                obj.legend.AddEntry(hist, channel_type, "l")

                obj.hist.append(hist)

            if obj:
                obj.legend.Draw('9')

                for label in obj.labels:
                    label.draw()

                canvases.append(obj)

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

def sqrt_hist(hist):
    for bin_ in range(1, hist.GetXaxis().GetNbins() + 1):
        hist.SetBinContent(bin_, math.sqrt(hist.GetBinContent(bin_)))

    hist.GetYaxis().SetTitle("#sqrt{" + hist.GetYaxis().GetTitle() + "}")

def signal_over_background(signal, background, title=None):
    ratio = signal.Clone()
    ratio.Divide(background)

    ratio.GetYaxis().SetTitleSize(0.035)
    ratio.GetYaxis().SetTitleOffset(2.5)
    if title:
        ratio.GetYaxis().SetTitle(title)
    else:
        ratio.GetYaxis().SetTitle("#frac{" +
                                  signal.GetYaxis().GetTitle() +
                                  "}{" + background.GetYaxis().GetTitle() + 
                                  "}")

    return ratio

class TemplatesDataVsBG(Templates):
    def __init__(self, options, args):
        Templates.__init__(self, options, args)

    def _create_obj(self, name):
        # container where all canvas related objects will be saved
        class Object: pass

        obj = Object()

        obj.canvas = ROOT.TCanvas()
        obj.legend = ROOT.TLegend(.5, .3, .8, .35)
        obj.legend.SetMargin(0.12);
        obj.legend.SetTextSize(0.03);
        obj.legend.SetFillColor(10);
        obj.legend.SetBorderSize(0);

        obj.canvas.SetName(name)
        obj.canvas.SetWindowSize(1024, 640)
        obj.canvas.Divide(2)

        for pad_id in range(1, 3):
            pad = obj.canvas.cd(pad_id)
            pad.SetRightMargin(5)
            pad.SetBottomMargin(0.15)

        obj.canvas.cd(1)

        obj.hist = []

        return obj

    @Timer(label = "[plot templates]", verbose = True)
    def _plot(self):
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
            background = None
            if "mc" in channels:
                background = channels["mc"].hist.Clone()

            if "qcd" in channels:
                if not background:
                    background = channels["qcd"].hist.Clone()
                else:
                    background.Add(channels["qcd"].hist)

            if not background:
                print("background is not avilable for", plot_name,
                      file=sys.stderr)

                continue

            background = calculate_efficiency(background)
            obj = self._create_obj("canvas_bg")
            obj.hist.append(background)
            background.Draw('hist 9')
            obj.legend.AddEntry(background, "background", "l")
            obj.legend.Draw('9')
            canvases.append(obj)

            bg_sqrt = background.Clone()
            sqrt_hist(bg_sqrt)
            obj = self._create_obj("canvas_sqrt_bg")
            obj.hist.append(bg_sqrt)
            bg_sqrt.Draw('hist 9')
            obj.legend.AddEntry(bg_sqrt, "sqrt(background)", "l")
            obj.legend.Draw('9')

            canvases.append(obj)

            # process Z'
            obj = None
            for channel_type, template in channels.items():
                if not channel_type.startswith("zp"):
                    continue

                hist = calculate_efficiency(template.hist)
                hist.SetFillStyle(0)

                obj = self._create_obj("canvas_chi2_{0}".format(channel_type))

                '''
                if '1000' in channel_type:
                    hist.Print('all')
                    background.Print('all')
                '''

                ratio = signal_over_background(hist, background,
                                               title="#frac{s}{b}")
                ratio.SetMaximum(5)
                ratio.SetMinimum(0.5)
                obj.hist.append(ratio)
                obj.legend.AddEntry(hist, channel_type, "l")

                ratio.Draw('hist 9')
                obj.legend.Draw('9')

                obj.canvas.cd(2)
                ratio = signal_over_background(hist, bg_sqrt,
                                               title="#frac{s}{#sqrt{b}}")
                obj.hist.append(ratio)

                ratio.SetMaximum(1.5)
                ratio.SetMinimum(0.5)
                ratio.Draw('hist 9')
                obj.legend.Draw('9')

                canvases.append(obj)

        return canvases
