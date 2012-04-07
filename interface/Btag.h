// b-tagging utils
//
// Created by Samvel Khalatyan, Mar 25, 2011
// Copyright 2011, All rights reserved

#ifndef BTAG_H
#define BTAG_H

#include "bsm_core/interface/Object.h"
#include "interface/bsm_fwd.h"
#include "interface/AppController.h"
#include "interface/DelegateManager.h"

namespace bsm
{
    class BtagDelegate
    {
        public:
            enum Systematic
            {
                DOWN = -1,
                NONE = 0,
                UP = 1
            };

            virtual ~BtagDelegate()
            {
            }

            virtual void setSystematic(const Systematic &)
            {
            }
    };

    class BtagOptions:
        public Options,
        public DelegateManager<BtagDelegate>
    {
        public:
            BtagOptions();

            // Options interface
            //
            virtual DescriptionPtr description() const;

        private:
            void setSystematic(const BtagDelegate::Systematic &);

            DescriptionPtr _description;
    };

    class BtagFunction
    {
        public:
            virtual ~BtagFunction()
            {
            };

            virtual float value(const float &x) const = 0;
            virtual float error_plus(const float &x) const = 0;
            virtual float error_minus(const float &x) const = 0;

        protected:
            const uint32_t find_bin(const float &jet_pt) const;

        private:
            static const uint32_t bins;
            static const float jet_pt_bins[];
    };

    class BtagScale: public BtagFunction
    {
        // CSVT operating point
        public:
            virtual float value(const float &jet_pt) const;
            virtual float error_plus(const float &jet_pt) const
            {
                return error(jet_pt);
            }

            virtual float error_minus(const float &jet_pt) const
            {
                return error(jet_pt);
            }

        protected:
            virtual float error(const float &jet_pt) const;

        private:
            static const float errors[];
    };

    class CtagScale: public BtagScale
    {
        protected:
            virtual float error(const float &jet_pt) const;
    };

    class LightScale: public BtagFunction
    {
        public:
            virtual float value(const float &jet_pt) const;
            virtual float error_plus(const float &jet_pt) const;
            virtual float error_minus(const float &jet_pt) const;

        private:
            static float value_max(const float &jet_pt);
            static float value_min(const float &jet_pt);
    };

    class BtagEfficiency: public BtagFunction
    {
        // Errors are not provided ... yet
        public:
            virtual float value(const float &jet_pt) const;
            virtual float error_plus(const float &jet_pt) const
            {
                return 0;
            }

            virtual float error_minus(const float &jet_pt) const
            {
                return 0;
            }

        private:
            static const float values[];
    };

    class CtagEfficiency: public BtagEfficiency
    {
    };

    class LightEfficiency: public BtagEfficiency
    {
        // Errors are not provided ... yet
        public:
            virtual float value(const float &jet_pt) const;

        private:
            static const float values[];
    };

    class Btag: public core::Object,
                public BtagDelegate
    {
        public:
            typedef std::pair<bool, const float> Info;

            Btag();
            Btag(const Btag &);

            Info is_tagged(const CorrectedJet &jet);

            // BtagDelegate interface
            //
            virtual void setSystematic(const Systematic &);

            // Object interface
            //
            virtual uint32_t id() const;
            virtual ObjectPtr clone() const;
            virtual void print(std::ostream &) const;

        private:
            // Prevent copying
            //
            Btag &operator =(const Btag &);

            Systematic _systematic;

            static const float _discriminator;

            typedef boost::shared_ptr<BtagFunction> BtagFunctionPtr;

            BtagFunctionPtr _scale_btag;
            BtagFunctionPtr _eff_btag;

            BtagFunctionPtr _scale_ctag;
            BtagFunctionPtr _eff_ctag;

            BtagFunctionPtr _scale_light;
            BtagFunctionPtr _eff_light;
    };
}

#endif
