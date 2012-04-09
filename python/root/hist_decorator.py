'''
Created by Samvel Khalatyan, Feb 23, 2012
Copyright 2011, All rights reserved
'''

from __future__ import division,print_function

import ROOT

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

        h = wrapped(*parg, **karg)
        bins = h.GetXaxis().GetNbins()

        integral_error = ROOT.Double()
        integral = h.IntegralAndError(1, bins, integral_error)

        if integral:
            numerator = h.Clone()
            numerator.Reset()

            denominator = h.Clone()
            denominator.Reset()

            for bin_ in range(1, bins + 1):
                error = ROOT.Double()

                numerator.SetBinContent(bin_,
                                        h.IntegralAndError(1, bin_, error))
                numerator.SetBinError(bin_, error)

                denominator.SetBinContent(bin_, integral)
                denominator.SetBinError(bin_, integral_error)

            h = ROOT.TGraphAsymmErrors()
            h.Divide(numerator, denominator, "cl=0.683 b(1,1) mode e0")
        else:
            h = ROOT.TGraphAsymmErrors(h)

        return h

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
