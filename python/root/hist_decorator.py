'''
Created by Samvel Khalatyan, Feb 23, 2012
Copyright 2011, All rights reserved
'''

from __future__ import division,print_function

import ROOT

def calculate_efficiency(hist):
    '''
    bins = hist.GetXaxis().GetNbins()

    integral_error = ROOT.Double()
    integral = hist.IntegralAndError(1, bins, integral_error)

    if integral:
        numerator = hist.Clone()
        numerator.Reset()

        denominator = hist.Clone()
        denominator.Reset()

        for bin_ in range(1, bins + 1):
            error = ROOT.Double()

            numerator.SetBinContent(bin_,
                                    hist.IntegralAndError(1, bin_, error))
            numerator.SetBinError(bin_, error)

            denominator.SetBinContent(bin_, integral)
            denominator.SetBinError(bin_, integral_error)

        hist = ROOT.TGraphAsymmErrors()
        hist.Divide(numerator, denominator, "cl=0.683 b(1,1) mode e0")
    else:
        hist = ROOT.TGraphAsymmErrors(hist)
    '''

    hist.Scale(1 / hist.Integral())

    clone = hist.Clone()
    bins = hist.GetXaxis().GetNbins()
    for bin_ in range(1, bins + 1):
        error = ROOT.Double()

        hist.SetBinContent(bin_, clone.IntegralAndError(1, bin_, error))
        hist.SetBinError(bin_, error)

    hist.GetYaxis().SetTitle("#varepsilon")

    return hist

def efficiency(wrapped):
    '''
    Convert 1D histogram into efficiency plot:
        - normalize histogram
        - each bin content is equal to:

                Integral(1, bin)

          the error is appropriately set
    '''

    def integrate(*parg, **karg):
        ''' wrapper around function that is expected to return 1D hist '''

        return calculate_efficiency(wrapped(*parg, **karg))

    return integrate

if "__main__" == __name__:
    @efficiency
    def gaussian():
        h = ROOT.TH1F("test_gaus", "test_gaus", 40, -10, 10)
        h.FillRandom("gaus", 100)

        return h

    h = gaussian()
    h.Draw("ALP 9")

    raw_input("enter")
