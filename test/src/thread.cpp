// Test Threads
//
// Create N threads on M files
//
// Created by Samvel Khalatyan, May 04, 2011
// Copyright 2011, All rights reserved

#include <iostream>

#include <boost/shared_ptr.hpp>

#include "bsm_input/interface/Reader.h"

#include "interface/Thread.h"

using namespace std;

using boost::shared_ptr;

using bsm::core::AnalyzerThread;
using bsm::core::Files;
using bsm::core::ThreadController;

void run(const Files &input_files);

int main(int argc, char *argv[])
try
{
    if (3 > argc)
    {
        cerr << "Usage: " << argv[0] << " threads input.pb" << endl;
        cerr << endl;
        cerr << " threads is the maximum number of threads to use. 0 - auto"
            << endl;

        return 0;
    }

    GOOGLE_PROTOBUF_VERIFY_VERSION;

    {
        Files input_files;
        for(int i = 2; argc > i; ++i)
            input_files.push_back(argv[i]);

        run(input_files);
    }

    // Clean Up any memory allocated by libprotobuf
    //
    google::protobuf::ShutdownProtobufLibrary();

    return 0;
 }
catch(...)
{
    // Clean Up any memory allocated by libprotobuf
    //
    google::protobuf::ShutdownProtobufLibrary();

    cerr << "Unknown error" << endl;

    return 1;
}

void run(const Files &input_files)
try
{
    shared_ptr<ThreadController> controller(new ThreadController());

    for(Files::const_iterator input = input_files.begin();
            input_files.end() != input;
            ++input)
    {
        cout << " [+] " << *input << endl;
    }

    controller->process(input_files);
}
catch(...)
{
    cerr << "Unknown error" << endl;
}
