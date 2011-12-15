// Generate Template plots and comparison:
//
//  [MC]        hTlep, Mttbar in regions S1, S2, Signal
//  [Data]      hTlep, Mttbar in regions S1, S2
//  [MC/Data]   compare hTlep, Mttbar in S1 + S2
//
// Created by Samvel Khalatyan, Aug 29, 2011
// Copyright 2011, All rights reserved

#ifndef BSM_TEMPLATE_ANALYZER
#define BSM_TEMPLATE_ANALYZER

#include <iosfwd>

#include "bsm_stat/interface/bsm_stat_fwd.h"
#include "bsm_input/interface/bsm_input_fwd.h"
#include "interface/Analyzer.h"
#include "interface/AppController.h"
#include "interface/Cut.h"
#include "interface/Pileup.h"
#include "interface/TriggerAnalyzer.h"
#include "interface/bsm_fwd.h"

namespace bsm
{
    // Fix of the Wjets Scale Down sample with brocken branching fractions
    //
    class WDecay
    {
        public:
            enum Type
            {
                ELECTRON = 0,
                MUON,
                TAU,
                UNKNOWN
            };

            WDecay(const Type &type = UNKNOWN): _type(type)
            {
            }

            inline void setType(const Type &type)
            {
                if (UNKNOWN >= type)
                    _type =type;
            }

            inline Type type() const
            {
                return _type;
            }

            inline float correction() const
            {
                switch(type())
                {
                    case ELECTRON: return 3.01;
                    case MUON: // Fall through
                    case TAU: return 0.75;

                    default: return 0;
                }
            }

        private:
            Type _type;
    };

    class TemplatesDelegate 
    {
        public:
            virtual ~TemplatesDelegate()
            {
            }

            virtual void setWjetCorrection()
            {
            }
    };

    class TemplatesOptions : public Options
    {
        public:
            TemplatesOptions();

            void setDelegate(TemplatesDelegate *);
            TemplatesDelegate *delegate() const;

            // Options interface
            //
            virtual DescriptionPtr description() const;

        private:
            void setWjetCorrection();

            TemplatesDelegate *_delegate;

            DescriptionPtr _description;
    };

    class TemplateAnalyzer : public Analyzer,
        public CounterDelegate,
        public TriggerDelegate,
        public TemplatesDelegate
    {
        public:
            typedef boost::shared_ptr<stat::H1> H1Ptr;
            typedef boost::shared_ptr<stat::H2> H2Ptr;
            typedef boost::shared_ptr<P4Monitor> P4MonitorPtr;

            TemplateAnalyzer();
            TemplateAnalyzer(const TemplateAnalyzer &);

            // TemplatesDelegate interface
            //
            inline virtual void setWjetCorrection()
            {
                _apply_wjet_correction = true;
            }

            const H1Ptr npv() const;
            const H1Ptr npvWithPileup() const;
            const H1Ptr njets() const;
            const H1Ptr d0() const;
            const H1Ptr htlep() const;
            const H1Ptr htall() const;   
            const H1Ptr htlepAfterHtlep() const;
            const H1Ptr htlepBeforeHtlep() const;
            const H1Ptr mttbarBeforeHtlep() const;
            const H1Ptr mttbarAfterHtlep() const;
            const H2Ptr drVsPtrel() const;

            const H1Ptr ttbarPt() const;
            const H1Ptr wlepMt() const;
            const H1Ptr whadMt() const;
            const H1Ptr wlepMass() const;
            const H1Ptr whadMass() const;
            const H1Ptr met() const;
            const H1Ptr metNoWeight() const;

            const H2Ptr ljetMetDphivsMetBeforeTricut() const;
            const H2Ptr leptonMetDphivsMetBeforeTricut() const;
            const H2Ptr ljetMetDphivsMet() const;
            const H2Ptr leptonMetDphivsMet() const;

            const H2Ptr htopNjetvsM() const;
            const H2Ptr htopPtvsM() const;
            const H2Ptr htopPtvsNjets() const;
            const H2Ptr htopPtvsLtoppt() const;

            const P4MonitorPtr firstJet() const;
            const P4MonitorPtr secondJet() const;
            const P4MonitorPtr thirdJet() const;
            const P4MonitorPtr electron() const;
            const P4MonitorPtr electronBeforeTricut() const;

            const P4MonitorPtr ltop() const;
            const P4MonitorPtr htop() const;

            JetEnergyCorrectionDelegate *getJetEnergyCorrectionDelegate() const;
            SynchSelectorDelegate *getSynchSelectorDelegate() const;
            Cut2DSelectorDelegate *getCut2DSelectorDelegate() const;
            PileupDelegate *getPileupDelegate() const;

            // Counter Delegate interface
            //
            virtual void didCounterAdd(const Counter *);

            // Trigger Delegate inteface
            //
            virtual void setTrigger(const Trigger &);

            // Anlayzer interface
            //
            virtual void onFileOpen(const std::string &filename, const Input *);
            virtual void process(const Event *);

            // Object interface
            //
            virtual uint32_t id() const;
            virtual ObjectPtr clone() const;
            virtual void merge(const ObjectPtr &);
            virtual void print(std::ostream &) const;

        private:
            typedef boost::shared_ptr<H1Proxy> H1ProxyPtr;
            typedef boost::shared_ptr<H2Proxy> H2ProxyPtr;
            typedef ::google::protobuf::RepeatedPtrField<GenParticle>
                GenParticles;

            void fillDrVsPtrel();
            void fillHtlep();

            struct Mttbar
            {
                LorentzVector mttbar;
                LorentzVector wlep;
                LorentzVector whad;
                LorentzVector neutrino;
                LorentzVector ltop;
                LorentzVector htop;

                int htop_njets;
            };

            Mttbar mttbar() const;
            void monitorJets();

            float htlepValue() const;
            float htallValue() const;

            bool isGoodLepton() const;
            bool isBtagJet() const;

            WDecay eventDecay(const Event *) const;
            WDecay decayType(const GenParticle &) const;
            WDecay wdecayType(const GenParticle &) const;

            boost::shared_ptr<SynchSelector> _synch_selector;

            H1ProxyPtr _npv;
            H1ProxyPtr _npv_with_pileup;
            H1ProxyPtr _njets;
            H1ProxyPtr _d0;
            H1ProxyPtr _htlep;
            H1ProxyPtr _htall;
            H1ProxyPtr _htlep_after_htlep;
            H1ProxyPtr _htlep_before_htlep;
            H1ProxyPtr _mttbar_before_htlep;
            H1ProxyPtr _mttbar_after_htlep;
            H2ProxyPtr _dr_vs_ptrel;

            H1ProxyPtr _ttbar_pt;
            H1ProxyPtr _wlep_mt;
            H1ProxyPtr _whad_mt;
            H1ProxyPtr _wlep_mass;
            H1ProxyPtr _whad_mass;
            H1ProxyPtr _met;
            H1ProxyPtr _met_noweight;

            H2ProxyPtr _ljet_met_dphi_vs_met_before_tricut;
            H2ProxyPtr _lepton_met_dphi_vs_met_before_tricut;
            H2ProxyPtr _ljet_met_dphi_vs_met;
            H2ProxyPtr _lepton_met_dphi_vs_met;

            H2ProxyPtr _htop_njet_vs_m;
            H2ProxyPtr _htop_pt_vs_m;
            H2ProxyPtr _htop_pt_vs_njets;
            H2ProxyPtr _htop_pt_vs_ltop_pt;

            const Event *_event;

            Counter *_secondary_lepton_counter;
            Counter *_leading_jet_counter;
            Counter *_htlep_counter;

            typedef std::vector<uint64_t> Triggers;
            Triggers _triggers; // hashes of triggers to be passed

            boost::shared_ptr<Pileup> _pileup;
            bool _use_pileup;
            float _pileup_weight;

            bool _wjets_input;
            bool _apply_wjet_correction;
            float _wjets_weight;

            P4MonitorPtr _first_jet;
            P4MonitorPtr _second_jet;
            P4MonitorPtr _third_jet;
            P4MonitorPtr _electron;
            P4MonitorPtr _electron_before_tricut;

            P4MonitorPtr _ltop;
            P4MonitorPtr _htop;
    };
}

#endif
