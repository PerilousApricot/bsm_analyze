#!/usr/bin/env python

'''
Created by Samvel Khalatyan, Mar 12, 2012
Copyright 2011, All rights reserved
'''

import template.templates

from template import channel_type

from root.tfile import topen
from util.timer import Timer
from util.arg import split_use_and_ban

class Templates(template.templates.Templates):
    channel_names = {
            # Map channel type to channel name to be used in plot name

            "ttbar": "ttbar",
            "zjets": "zjets",
            "wb": "wb",
            "wc": "wc",
            "wlight": "wlight",
            "stop": "singletop",
            "qcd": "eleqcd",
            "data": "DATA",

            # narrow resonances
            "zprime_m1000_w10": "zp1000",
            "zprime_m1500_w15": "zp1500",
            "zprime_m2000_w20": "zp2000",
            "zprime_m3000_w30": "zp3000",
            "zprime_m4000_w40": "zp4000",

            # wide resonances
            "zprime_m1000_w100": "zp1000wide",
            "zprime_m1500_w150": "zp1500wide",
            "zprime_m2000_w200": "zp2000wide",
            "zprime_m3000_w300": "zp3000wide",
            "zprime_m4000_w400": "zp4000wide",

            # rsgluon
            "rsgluon_m1000": "rsg1000",
            "rsgluon_m1500": "rsg1500",
            "rsgluon_m2000": "rsg2000",
            "rsgluon_m3000": "rsg3000",
            "rsgluon_m4000": "rsg4000",

            "ttbar_matching_plus": "ttbar",
            "ttbar_matching_minus": "ttbar",

            "ttbar_scale_plus": "ttbar",
            "ttbar_scale_minus": "ttbar",
    }

    def __init__(self, options, args):
        template.templates.Templates.__init__(
                self,
                options, args,
                disable_systematics=False if (options.systematic and
                                              ("scale" in options.systematic or
                                               "matching" in options.systematic))
                                          else True)

        if options.systematic:
            self.suffix = "__{0}__{1}".format(
                    options.systematic[:-1],
                    "plus" if options.systematic.endswith("+") else "minus")
        else:
            self.suffix = None

        self.theta_prefix = options.thetaprefix
        self.output_filename = options.output

        if options.savechannels:
            use_channels, ban_channels = split_use_and_ban(set(
                channel.strip() for channel in options.savechannels.split(',')))

            # use only allowed channels or all if None specified
            channels = set(channel_type.ChannelType.channel_types.keys())
            if use_channels:
                channels &= self._expand_channels(use_channels)

            # remove banned channels
            if ban_channels:
                channels -= self._expand_channels(ban_channels)

            self.save_channels = list(channels)
        else:
            self.save_channels = []

    def _process(self):
        if not self.use_channels:
            raise RuntimeError("all channels are turned off")

        self._load_channels()
        self._run_fraction_fitter()
        self._apply_fractions()
        self._apply_scales()

        self._save_templates()

    @Timer(label="[save templates]", verbose=True)
    def _save_templates(self):
        '''
        Save loaded channels in output ROOT file
        
        Output file will be updated. All channels will be saved or only those,
        that are specified with --savechannels option.

        The histogram naming convention is as follows:

            [analysis]_[plot]__[channel][__[systematics]]

        where
        
            analysis    analysis channel: mu, el
            plot        plot name, e.g. mttbar
            channel     channel name: ttbar, zjets, wjets, singletop, etc.
            systematics source of the systematic error, e.g. jes__plus

        '''

        with topen(self.output_filename, "update"):
            for plot_name, channels in self.loader.plots.items():
                plot_name = plot_name[1:].replace('/', '_')

                # format string has different format with(-out) systematics
                format_string = str(self.theta_prefix) + "_{plot}__{channel}"
                if self.suffix:
                    format_string += self.suffix

                # save only those channels that are supported or specified by user
                for channel_type, channel in channels.items():
                    if (channel_type not in self.channel_names or
                            (self.save_channels and
                             channel_type not in self.save_channels)):

                        continue

                    name = format_string.format(
                            plot="mttbar" if "mttbar_after_htlep" == plot_name else plot_name,
                            channel=self.channel_names[channel_type])

                    hist = channel.hist.Clone(name)
                    hist.SetTitle(channel.hist.GetTitle())

                    hist.Write(name)

    def __str__(self):
        '''
        Print Templates object configuraiton
        '''

        result = []

        result.append(["systematic", self.suffix if self.suffix else ""])
        result.append(["output filename", self.output_filename])
        result.append(["save channels", self.save_channels])
        result.append(["theta prefix", self.theta_prefix])

        return "{0}\n{1}".format(
                template.templates.Templates.__str__(self),
                '\n'.join(self._str_format.format(name, value)
                          for name, value in result))
