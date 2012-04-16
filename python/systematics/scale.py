#!/usr/bin/env python

'''
Created by Samvel Khalatyan, Apr 12, 2012
Copyright 2011, All rights reserved
'''

from __future__ import division, print_function

import root.style
import template.compare as compare
from template.templates import Templates
from root.comparison import ComparisonCanvas
import root.label
from systematics.loader import SystematicLoader,ScalingSystematicLoader,MatchingSystematicLoader
from util.arg import split_use_and_ban
import ROOT

class Comparator(object):
    channels = set([
        "ttbar", "zjets", "wb", "wc", "wlight", "wjets", "stop",
        "zprime_m1000_w10",
        "zprime_m1500_w15",
        "zprime_m2000_w20",
        "zprime_m3000_w30"
    ])

    channel_names = Templates.channel_names

    def __init__(self, options, args):
        self._batch_mode = options.batch
        self._verbose = options.verbose

        self._input_filename = options.filename

        self.use_channels = []

        self._systematic = options.systematic
        if self._systematic:
            if self._systematic.endswith("+"):
                self._load_systematic = { "plus": True, "minus": False}

            elif self._systematic.endswith("-"):
                self._load_systematic = { "plus": False, "minus": True}

            else:
                self._load_systematic = None

            self._systematic = self._systematic.rstrip("+-")

        if not self._systematic:
            raise RuntimeError("systematic is not specified")

        if options.channels:
            use_channels, ban_channels = split_use_and_ban(set(
                channel.strip() for channel in options.channels.split(',')))

            # use only allowed channels or all if None specified
            channels = self.channels
            if use_channels:
                channels &= use_channels

            # remove banned channels
            if ban_channels:
                channels -= ban_channels

            self.use_channels = list(channels)
        else:
            self.use_channels = self.channels

        self.loader = None

    def run(self):
        # Apply TDR style to all plots
        style = root.style.tdr()
        style.cd()

        # print run configuration
        if self._verbose:
            print("{0:-<80}".format("-- Configuration "))
            print(self)
            print()

        self._load()
        self._get_scales()

    def _load(self):
        if self._verbose:
            print("{0:-<80}".format("-- Load Systematics "))

        loader = {
                "jes": SystematicLoader,
                "jer": SystematicLoader,
                "pileup": SystematicLoader,
                "btag": SystematicLoader,
                "mistag": SystematicLoader,
                "scale": ScalingSystematicLoader,
                "matching": MatchingSystematicLoader
                }.get(self._systematic)

        if not loader:
            raise RuntimeError("unsupported systematic type")

        self.loader = loader(self._input_filename,
                             self.use_channels,
                             self._systematic,
                             self._load_systematic)

        self.loader.load()

        if self._verbose:
            print(self.loader)
            print()

    def _get_scales(self):
        if not self.loader:
            raise RuntimeError("templates are not loaded")

        plot_name = "/mttbar_after_htlep"
        if plot_name not in self.loader.plots:
            raise RuntimeError("{0} is not loaded".format(plot_name))

        channels = self.loader.plots[plot_name]
        for channel, systematics in channels.items():
            max_y = 0
            for channel in (systematics.nominal,
                            systematics.plus,
                            systematics.minus):
                if not channel:
                    continue

                channel_max_y = channel.hist.GetBinContent(
                        channel.hist.GetMaximumBin())

                if channel_max_y > max_y:
                    max_y = channel_max_y

            if not systematics.nominal:
                raise RuntimeError("nominal plot {0} is not loaded".format(plot_name))

            nominal_yield = systematics.nominal.hist.Integral()

            if systematics.plus:
                plus_yield = systematics.plus.hist.Integral()
                print("+scale: {0:.2f}".format(plus_yield / nominal_yield))

            if systematics.minus:
                minus_yield = systematics.minus.hist.Integral()
                print("-scale: {0:.2f}".format(minus_yield / nominal_yield))

    def style(self, systematics):
        for channel in systematics.nominal, systematics.plus, systematics.minus:
            if not channel:
                continue

            plot = channel.hist

            plot.SetFillStyle(0)
            plot.SetFillColor(0)

            if channel == systematics.nominal:
                plot.SetLineColor(ROOT.kBlack)
                plot.SetLineStyle(1)
            else:
                plot.SetLineStyle(2)

                if channel == systematics.plus:
                    plot.SetLineColor(ROOT.kRed + 1)
                elif channel == systematics.minus:
                    plot.SetLineColor(ROOT.kGreen + 1)

    def __str__(self):
        result = []

        result.append(["verbose", self._verbose])
        result.append(["batch mode", self._batch_mode])
        result.append(["input filename", self._input_filename])
        result.append(["channels", self.use_channels])

        return '\n'.join("{0:>15}: {1}".format(name, value)
                         for name, value in result)
