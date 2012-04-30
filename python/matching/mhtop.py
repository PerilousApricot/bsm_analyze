#!/usr/bin/env python

'''
Created by Samvel Khalatyan, Apr 24, 2012
Copyright 2012, All rights reserved
'''

from __future__ import print_function

import sys
import template.main

def parser():
    parser = template.main.parser()

    parser.remove_option("--plots")
    parser.remove_option("--folders")

    parser.add_option(
            "--systematic",
            action = "store",
            help = ("Systematics type to be loaded: jes, pileup, "
                    "matching, scaling"))

    return parser

def main():
    try:
        options, args = parser().parse_args()

        import comparator
        
        app = comparator.Comparator(options, args)
        app.run()

    except RuntimeError as error:
        # print error traceback for debug
        import traceback

        traceback.print_tb(sys.exc_info()[2])

        print(error, file = sys.stderr)

        return 1

    else:
        return 0

if "__main__" == __name__:
    sys.exit(main())
