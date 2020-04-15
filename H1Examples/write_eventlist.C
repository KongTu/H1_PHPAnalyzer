///////////////////////////////////////////////////////
// Write Standard Eventlist on file
//
// Author     : Ursula Berthon (berthon@in2p3.fr)
// Created    : 11.12.2002
// Last update: $Date: 2003/10/10 21:15:30 $
//          by: $Author: wessling $
//
///////////////////////////////////////////////////////

#include <stdlib.h>

// ROOT includes
#include <TFile.h>
#include <TROOT.h>
#include <iostream>

// H1 OO includes
#include "H1Skeleton/H1Tree.h"
#include "H1Steering/H1StdCmdLine.h"

using namespace std;

int
main(int argc, char* argv[])
{
    // name with which the H1EventList is written
    char eventlistname[]="mylist";

    // parse the command line
    H1StdCmdLine opts;
    opts.Parse(&argc, argv);

    // Load mODS/HAT files
    H1Tree::Instance()->Open();            // this statement must be there!

    // Write eventlist to file
    TFile file(opts.GetOutput(), "RECREATE");
    gH1Tree->WriteEventList(opts.GetOutput(),eventlistname);

    // and close
    file.Write();
    file.Close();

    cout << "Eventlist written to " << opts.GetOutput() << endl;
    return 0;
}




