#!/usr/bin/env python

'''
Created by Samvel Khalatyan, Feb 21, 2012
Copyright 2011, All rights reserved
'''

from __future__ import print_function

import sys
from optparse import OptionParser

def parser():
    parser = OptionParser(usage = "usage: %prog [options]")

    parser.add_option(
            "-b", "--batch",
            action = "store_true", default = False,
            help = "Run application in batch mode")

    parser.add_option(
            "-v", "--verbose",
            action = "store_true", default = False,
            help = "Print additional info")

    parser.add_option(
            "--filename",
            action = "store", default = "output_signal_p150_hlt.root",
            help = "input filename")

    parser.add_option(
            "--scales",
            action = "store", 
            help = "scales filename")

    parser.add_option(
            "--fractions",
            action = "store",
            help = ("Use specified fractions insted of dynamic "
                    "TFractionFitter results"))

    parser.add_option(
            "--ratio",
            action = "store", default = "",
            help = "Use channels ratio in comparison")

    parser.add_option(
            "--notff",
            action = "store_true", default = False,
            help = "Do NOT use TFraction Fitter")

    parser.add_option(
            "--plots",
            action = "store",
            help = "Load only comma separated plots")

    parser.add_option(
            "--folders",
            action = "store",
            help = ("Look for plots in comma separated folders in "
                    "ROOT file"))

    parser.add_option(
            "--channels",
            action = "store",
            help = "Load only comma separated channels")

    parser.add_option(
            "--suffix",
            action="store",
            help="Suffix for canvases to be stored")

    parser.add_option(
            "--btag",
            action="store",
            help="btag channel: 0btag or 1btag")

    parser.add_option(
            "--log",
            action="store_true", default=False,
            help = "Generate plots in Log scale")

    parser.add_option(
            "--label",
            action="store",
            help = "Add label to the plots, e.g.: channel name")

    return parser

def main():
    try:
        options, args = parser().parse_args()

        # import templates only here otherwise PyROOT inhercepts --help option
        import templates

        app = templates.Templates(options, args)
        app.run()

    except Exception as error:
        # print Exception traceback for debug
        import traceback

        traceback.print_tb(sys.exc_info()[2])

        print(error, file = sys.stderr)

        return 1

    else:
        return 0

if "__main__" == __name__:
    sys.exit(main())
