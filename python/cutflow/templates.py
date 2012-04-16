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
    }

    def __init__(self, options, args):
        options.plots = "cutflow"
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

        self._print_yield()

    def _print_yield(self):
        if "/cutflow" not in self.loader.plots:
            raise RuntimeError("cutflow is not loaded")

        channels = self.loader.plots["/cutflow"]
        yields = {}
        for channel in ("zprime_m1000_w10",
                        "zprime_m1500_w15",
                        "zprime_m2000_w20",
                        "zprime_m3000_w30",
                        "stop",
                        "wb",
                        "wc",
                        "wlight",
                        "zjets",
                        "ttbar",
                        "qcd",
                        "mc",
                        "data"):
            if channel not in channels:
                raise RuntimeError('channel {0} is not loaded'.format(channel))

            hist = channels[channel].hist
            yields[channel] = (hist.GetBinContent(hist.GetXaxis().GetLast()),
                               hist.GetBinError(hist.GetXaxis().GetLast()))

        yields["background"] = (yields["mc"][0] + yields["qcd"][0],
                                math.sqrt(yields["mc"][1] ** 2 +
                                          yields["qcd"][1] ** 2))

        for channel in ("zprime_m1000_w10",
                        "zprime_m1500_w15",
                        "zprime_m2000_w20",
                        "zprime_m3000_w30",
                        None,
                        "stop",
                        "wb",
                        "wc",
                        "wlight",
                        "zjets",
                        "ttbar",
                        None,
                        "mc",
                        "qcd",
                        None,
                        "background",
                        "data"):
            if not channel:
                continue

            print('{ch}: {yield_[0]:.2f} + {yield_[1]:.2f}'.format(ch=channel, yield_=yields[channel]))

    def __str__(self):
        '''
        Print Templates object configuraiton
        '''

        result = []

        return "{0}\n{1}".format(
                template.templates.Templates.__str__(self),
                '\n'.join(self._str_format.format(name, value)
                          for name, value in result))
