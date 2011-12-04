// Template histogram
//
// Created by Samvel Khalatyan, Dec 2, 2011
// Copyright 2011, All rights reserved

#ifndef BSM_TEMPLATE
#define BSM_TEMPLATE

#include <string>
#include <iosfwd>

class Template
{
    public:
        enum Type
        {
            MET = 0,
            HTLEP,
            NPV,
            NJET,
            TTBAR_MASS,
            TTBAR_PT,
            WLEP_MT,
            WLEP_MASS,
            JET1_PT,
            JET1_ETA,
            JET2_PT,
            JET2_ETA,
            JET3_PT,
            JET3_ETA,
            ELECTRON_PT,
            ELECTRON_ETA,
            LTOP_PT,
            LTOP_ETA,
            LTOP_MASS,
            LTOP_MT,
            HTOP_PT,
            HTOP_ETA,
            HTOP_MASS,
            HTOP_MT
        };

        Template(const Type &type): _type(type)
        {
        }

        inline Type type() const
        {
            return _type;
        }

        operator std::string() const;

        std::string repr() const;

        Template &operator ++();
        Template &operator --();

        std::string unit() const;

    private:
        Type _type;
};

bool operator <(const Template &, const Template &);
bool operator >(const Template &, const Template &);

bool operator <=(const Template &, const Template &);
bool operator >=(const Template &, const Template &);

bool operator ==(const Template &, const Template &);
bool operator !=(const Template &, const Template &);

std::ostream &operator <<(std::ostream &out, const Template &c);

#endif