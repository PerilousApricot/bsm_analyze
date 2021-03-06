#!/usr/bin/env python

'''
Created by Samvel Khalatyan, Feb 23, 2012
Copyright 2011, All rights reserved
'''

from base_style import Style,BaseStyle
import ROOT

class ChannelStyle(BaseStyle):
    '''
    Specify channel styles for plots. These are merged input styles in some
    cases, for example: stop_s, stop_t, stop_tw, etc. are merged into stop
    (single top)
    '''

    # List all supported channel styles: keys are supported styles, values
    # represent style objects to be applied to histograms.
    channel_styles = {
            "ttbar": Style(ROOT.kRed + 1),
            "ttbar_powheg": Style(ROOT.kRed + 2),
            "zjets": Style(ROOT.kBlue + 1),
            "wb": Style(ROOT.kGreen + 3),
            "wc": Style(ROOT.kGreen + 2),
            "wlight": Style(ROOT.kGreen + 1),
            "wjets": Style(ROOT.kGreen + 1),
            "stop": Style(ROOT.kMagenta + 1),

            "data": Style(fill_style = 0, marker_size = 1, marker_style = 20),

            "qcd": Style(ROOT.kYellow + 1),

            # Narrow resonances
            "zprime_m1000_w10": Style(line_style = 1, fill_style = 0),
            "zprime_m1500_w15": Style(line_style = 2, fill_style = 0),
            "zprime_m2000_w20": Style(line_style = 3, fill_style = 0),
            "zprime_m3000_w30": Style(line_style = 7, fill_style = 0),
            #"zprime_m4000_w40": Style(line_style = 9, fill_style = 0),

            # Wide resonances
            "zprime_m1000_w100": Style(line_style = 1, fill_style = 0),
            "zprime_m1500_w150": Style(line_style = 2, fill_style = 0),
            "zprime_m2000_w200": Style(line_style = 3, fill_style = 0),
            "zprime_m3000_w300": Style(line_style = 7, fill_style = 0),
            #"zprime_m4000_w400": Style(line_style = 9, fill_style = 0),

            # RSGluon
            "rsgluon_m1000": Style(line_style = 1, fill_style = 0),
            "rsgluon_m1500": Style(line_style = 2, fill_style = 0),
            "rsgluon_m2000": Style(line_style = 3, fill_style = 0),
            "rsgluon_m2500": Style(line_style = 3, fill_style = 0),
            "rsgluon_m3000": Style(line_style = 7, fill_style = 0),

            # Systematic channels
            "ttbar_scale_plus": Style(ROOT.kRed + 1),
            "ttbar_scale_minus": Style(ROOT.kRed + 1),

            "ttbar_matching_plus": Style(ROOT.kRed + 1),
            "ttbar_matching_minus": Style(ROOT.kRed + 1),

            "wjets_scale_plus": Style(ROOT.kRed + 1),
            "wjets_scale_minus": Style(ROOT.kRed + 1),

            "wjets_matching_plus": Style(ROOT.kRed + 1),
            "wjets_matching_minus": Style(ROOT.kRed + 1) }

    def __init__(self, channel_style):
        '''
        Initialize channel with specific style. Style can not be changed.
        '''

        BaseStyle.__init__(self, channel_style, "channel_style")

    @property
    def channel_style(self):
        '''
        Access style object
        '''

        return self.channel_styles[self.style]

    def __contains__(self, value):
        '''
        Check if specific style is allowed
        '''

        return (value in self.channel_styles or
                BaseStyle.__contains__(self, value))



if "__main__" == __name__:
    import unittest

    class TestChannelStyle(unittest.TestCase):
        def test_ttbar(self):
            input_style = ChannelStyle("ttbar")
            self.assertEqual(input_style.style, "ttbar")

        def test_zjets(self):
            input_style = ChannelStyle("zjets")
            self.assertEqual(input_style.style, "zjets")

        def test_wjets(self):
            input_style = ChannelStyle("wjets")
            self.assertEqual(input_style.style, "wjets")

        def test_stop(self):
            input_style = ChannelStyle("stop")
            self.assertEqual(input_style.style, "stop")

        def test_data(self):
            input_style = ChannelStyle("data")
            self.assertEqual(input_style.style, "data")

        def test_zprime_m1000_w10(self):
            input_style = ChannelStyle("zprime_m1000_w10")
            self.assertEqual(input_style.style, "zprime_m1000_w10")

        def test_zprime_m1500_w15(self):
            input_style = ChannelStyle("zprime_m1500_w15")
            self.assertEqual(input_style.style, "zprime_m1500_w15")

        def test_zprime_m2000_w20(self):
            input_style = ChannelStyle("zprime_m2000_w20")
            self.assertEqual(input_style.style, "zprime_m2000_w20")

        def test_zprime_m3000_w30(self):
            input_style = ChannelStyle("zprime_m3000_w30")
            self.assertEqual(input_style.style, "zprime_m3000_w30")

        def test_zprime_m4000_w40(self):
            input_style = ChannelStyle("zprime_m4000_w40")
            self.assertEqual(input_style.style, "zprime_m4000_w40")

        def test_qcd(self):
            self.assertRaises(AttributeError, ChannelStyle, "qcd")

    unittest.main()
