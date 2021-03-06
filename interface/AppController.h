// Application Controller
//
// Apply user-specified analyzer to given input file(s) in single- or
// multi-threads mode.
//
// Created by Samvel Khalatyan, Jul 31, 2011
// Copyright 2011, All rights reserved

#ifndef BSM_APP_CONTROLLER
#define BSM_APP_CONTROLLER

#include <string>
#include <vector>

#include <boost/program_options.hpp>
#include <boost/shared_ptr.hpp>

#include "bsm_core/interface/bsm_core_fwd.h"
#include "bsm_input/interface/bsm_input_fwd.h"
#include "bsm_input/interface/Reader.h"
#include "interface/bsm_fwd.h"

namespace po = boost::program_options;

class TFile;

namespace bsm
{
    class Options
    {
        public:
            typedef boost::shared_ptr<po::options_description> DescriptionPtr;

            virtual ~Options() {}

            virtual DescriptionPtr description() const = 0;
    };

    class AppController: public ReaderDelegate
    {
        public:
            typedef std::vector<std::string> Inputs;
            typedef boost::shared_ptr<Analyzer> AnalyzerPtr;
            typedef boost::shared_ptr<TFile> TFilePtr;

            AppController();
            ~AppController();

            void setAnalyzer(const AnalyzerPtr &,
                    const bool &is_reader_delegate = false);

            bool isAnalyzerReaderDelegate() const;

            void addOptions(const Options &);

            void addInputs(const Inputs &);

            bool isInteractive() const;
            TFilePtr output() const;

            bool run(int &argc, char *argv[]);

            // disable multi-thread option
            //
            void disableMutlithread();

            // Reader Delegate interface
            //
            virtual void fileWillOpen(const Reader *);
            virtual void fileDidOpen(const Reader *);

            virtual void fileWillClose(const Reader *);
            virtual void fileDidClose(const Reader *);

        private:
            // Prevent Copying
            //
            AppController(const AppController &);
            AppController &operator =(const AppController &);

            typedef Options::DescriptionPtr DescriptionPtr;

            enum RunMode
            {
                SINGLE_THREAD = 0,
                MULTI_THREAD
            };

            void setDebugFile(const std::string &);

            void setNumberOfThreads(const uint32_t &);

            void setInteractive(const bool &);
            void setOutput(const std::string &);

            void processSingleThread();
            void processMultiThread();

            RunMode _run_mode;

            DescriptionPtr _generic_options;
            DescriptionPtr _hidden_options;

            std::vector<DescriptionPtr> _custom_options;

            AnalyzerPtr _analyzer;

            Inputs _input_files;

            bool _disable_multithread;
            uint32_t _number_of_threads;

            boost::shared_ptr<core::Debug> _debug;

            bool _interactive;

            std::string _output_filename;
            TFilePtr _output;

            ReaderDelegate *_reader_delegate;
    };
}

#endif
