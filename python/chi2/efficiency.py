#!/usr/bin/env python

'''
Created by Samvel Khalatyan, Mar 12, 2012
Copyright 2011, All rights reserved
'''

from __future__ import print_function

import sys
import template.main

def parser():
    parser = template.main.parser()

    parser.remove_option("--ratio")
    parser.remove_option("--plots")
    parser.remove_option("--folders")

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
