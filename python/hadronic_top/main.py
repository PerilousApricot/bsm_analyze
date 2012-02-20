#!/usr/bin/env python

'''
Created by Samvel Khalatyan, Feb 09, 2012
Copyright 2011, All rights reserved

Plot templates generated by Hadronic Top analyzer
'''

from __future__ import print_function

import sys

import root.style

from hadronic_top.template import HadronicTopTemplates as Templates

def usage(argv):
    return "usage: {0[0]} input.root".format(argv)

def main(argv = sys.argv):
    '''
    application entry point

    run executable without arguments to see usage
    '''
    
    try:
        if 2 > len(argv):
            raise Exception("input is not specified")

        style = root.style.tdr()
        style.SetOptStat(111111)
        style.cd()

        templates = Templates()
        #templates.use_folders = ["jet1", "jet2", "top", "jet1_parton" ]
        #templates.use_folders = ["top", "jet1_vs_jet2", "jet1", "jet2", "jet1_parton", "jet2_parton"]
        #templates.use_folders = ["ttbar_reco_delta", "ttbar_gen_delta"]
        #templates.use_folders = ["ttbar_reco", "ttbar_gen"]
        #templates.use_folders = ["top", "gen_top"]
        #templates.use_folders = ["jet1", "jet1_parton"]
        #templates.use_folders = ["jet2", "jet2_parton"]
        templates.use_folders = ["jet1_vs_jet2", "jet1_parton_vs_jet2_parton"]
        #templates.use_folders = ["ttbar_reco", "ttbar_gen", "ttbar_reco_delta", "ttbar_gen_delta"]
        templates.ban_plots = [ "px", "py", "pz", "phi", "ptrel"]

        templates.parton_rebin["mass"] = 1
        templates.parton_rebin["mt"] = 1

        templates.ttbar_rebin["eta"] = 25

        templates.load(argv[1])

        if not templates:
            raise Exception("failed to laod templates")

        canvases = templates.draw()
        raw_input("enter")

    except Exception as error:
        print(error, file = sys.stderr)
        print()
        print(usage(argv))

        return 0
    else:
        return 1

if "__main__" == __name__:
    sys.exit(main())
