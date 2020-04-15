///////////////////////////////////////////////////////
// Example of 
// how to create an eventlist by entering events one by one
// and write it to a file afterwards
//
// Author     : Ursula Berthon (berthon@in2p3.fr)
// Created    : 11.12.2002// Last update: $Date: 2003/10/10 21:15:29 $
//          by: $Author: wessling $
//
///////////////////////////////////////////////////////
#include <stdlib.h>
#include <iostream>

// ROOT includes
#include <TFile.h>
#include <TROOT.h>

// H1 OO includes
#include "H1Skeleton/H1Tree.h"
#include "H1Skeleton/H1EventList.h"
#include "H1Pointers/H1FloatPtr.h"
#include "H1Steering/H1StdCmdLine.h"

using namespace std;

int
main(int argc, char* argv[])
{
    // parse the command line
    H1StdCmdLine opts;
    opts.Parse(&argc, argv);

      // Load mODS/HAT files
    H1Tree::Instance()->Open();            // this statement must be there!

     // do a selection
    H1Tree::Instance()->SelectHat("Q2e>1000.");

      // create new H1EventList
    H1EventList *list= new H1EventList("mylist");

      // and enter interesting events into this list
    H1FloatPtr yda("Yda");
    Int_t sel=0;
    while (H1Tree::Instance()->Next()) {
      if (*yda>.5)    {
        sel++;
        list->Enter();
      }
    }
    cout << "Selected "<<sel<<" entries"<<endl;


      // Write eventlist to file
    TFile file(opts.GetOutput(), "RECREATE");
    list->Write();
    list->Print();
    // close
    file.Write();
    file.Close();

    cout << "Eventlist written to " << opts.GetOutput() << endl;
    return 0;
}












