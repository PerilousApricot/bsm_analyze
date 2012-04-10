// b-tagging utils
//
// Created by Samvel Khalatyan, Mar 25, 2011
// Copyright 2011, All rights reserved

#ifndef BTAG_H
#define BTAG_H

#include <vector>

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
                NONE = 0,
                DOWN,
                UP
            };

            virtual ~BtagDelegate()
            {
            }

            virtual void setBtagSystematic(const Systematic &)
            {
            }

            virtual void setMistagSystematic(const Systematic &)
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
            void setBtagSystematic(const BtagDelegate::Systematic &);
            void setMistagSystematic(const BtagDelegate::Systematic &);

            DescriptionPtr _description;
    };

    class BtagFunction
    {
        public:
            BtagFunction();

            virtual ~BtagFunction()
            {
            };

            virtual float value(const float &x) const = 0;
            virtual float value_plus(const float &x) const = 0;
            virtual float value_minus(const float &x) const = 0;

        protected:
            const uint32_t find_bin(const float &jet_pt) const;

        private:
            std::vector<float> _bins;
    };

    class BtagScale: public BtagFunction
    {
        // CSVT operating point
        public:
            BtagScale();

            virtual float value(const float &jet_pt) const;
            virtual float value_plus(const float &jet_pt) const
            {
                return value(jet_pt) + error(jet_pt);
            }

            virtual float value_minus(const float &jet_pt) const
            {
                const float value_ = value(jet_pt) - error(jet_pt);

                return value_ > 0 ? value_ : 0;
            }

        protected:
            virtual float error(const float &jet_pt) const;

        private:
            std::vector<float> _errors;
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
            virtual float value_plus(const float &jet_pt) const;
            virtual float value_minus(const float &jet_pt) const;

        private:
            float value_max(const float &jet_pt) const;
            float value_min(const float &jet_pt) const;
    };

    class BtagEfficiency: public BtagFunction
    {
        // Errors are not provided ... yet
        public:
            BtagEfficiency();

            virtual float value(const float &jet_pt) const;
            virtual float value_plus(const float &jet_pt) const
            {
                return value(jet_pt);
            }

            virtual float value_minus(const float &jet_pt) const
            {
                return value(jet_pt);
            }

        private:
            std::vector<float> _values;
    };

    class CtagEfficiency: public BtagEfficiency
    {
    };

    class LightEfficiency: public BtagEfficiency
    {
        // Errors are not provided ... yet
        public:
            LightEfficiency();

            virtual float value(const float &jet_pt) const;

        private:
            std::vector<float> _values;
    };

    class LightEfficiencyData: public BtagEfficiency
    {
        // Errors are not provided ... yet
        public:
            virtual float value(const float &jet_pt) const;
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
            virtual void setBtagSystematic(const Systematic &);
            virtual void setMistagSystematic(const Systematic &);

            // Object interface
            //
            virtual uint32_t id() const;
            virtual ObjectPtr clone() const;
            virtual void print(std::ostream &) const;

        private:
            typedef boost::shared_ptr<BtagFunction> BtagFunctionPtr;

            // Prevent copying
            //
            Btag &operator =(const Btag &);

            float scale(const bool &is_tagged,
                        const float &jet_pt,
                        const BtagFunctionPtr &sf,
                        const BtagFunctionPtr &eff,
                        const Systematic &sytematic);

            float scale_data(const bool &is_tagged,
                             const float &jet_pt,
                             const BtagFunctionPtr &sf,
                             const BtagFunctionPtr &eff,
                             const Systematic &sytematic);

            Systematic _btag_systematic;
            Systematic _mistag_systematic;

            const float _discriminator;

            BtagFunctionPtr _scale_btag;
            BtagFunctionPtr _eff_btag;

            BtagFunctionPtr _scale_ctag;
            BtagFunctionPtr _eff_ctag;

            BtagFunctionPtr _scale_light;
            BtagFunctionPtr _eff_light;
    };
}

#endif
