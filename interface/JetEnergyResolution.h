// Jet Energy Resolution
//
// Doc: https://twiki.cern.ch/twiki/bin/view/CMS/JetResolution
//
// Created by Samvel Khalatyan, Apr 10, 2012
// Copyright 2012, All rights reserved

#ifndef BSM_JET_ENERGY_RESOLUTION
#define BSM_JET_ENERGY_RESOLUTION

#include <string>

#include "bsm_core/interface/Object.h"
#include "interface/bsm_fwd.h"
#include "interface/AppController.h"
#include "interface/DelegateManager.h"

namespace bsm
{
    class JetEnergyResolutionDelegate
    {
        public:
            enum Systematic
            {
                NONE = 0,
                DOWN,
                UP
            };

            ~JetEnergyResolutionDelegate() {}

            virtual void setSystematic(const Systematic &)
            {
            }
    };

    class JetEnergyResolutionOptions:
        public Options,
        public DelegateManager<JetEnergyResolutionDelegate>
    {
        public:
            JetEnergyResolutionOptions();

            // Options interface
            //
            virtual DescriptionPtr description() const;

        private:
            void setSystematic(const JetEnergyResolutionDelegate::Systematic &);

            DescriptionPtr _description;
    };

    class JetEnergyResolution: public core::Object,
                               public JetEnergyResolutionDelegate
    {
        public:
            JetEnergyResolution();
            JetEnergyResolution(const JetEnergyResolution &);

            // warning: the jet corrected_p4 will be changed
            //
            void correct(CorrectedJet &jet);
            
            // JetEnergyResolutionDelegate interface
            // 
            void setSystematic(const Systematic &);

            // Object interface
            //
            virtual uint32_t id() const;
            virtual ObjectPtr clone() const;
            virtual void print(std::ostream &) const;

        private:
            // Prevent copying
            //
            JetEnergyResolution &operator =(const JetEnergyResolution &);

            virtual float scale(const float &reco_eta);

            Systematic _systematic;
    };
}

#endif

#include <iosfwd>
