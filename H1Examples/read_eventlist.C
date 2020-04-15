///////////////////////////////////////////////////////
// Reads a H1Eventlist. In this case the H1Eventlist was written on a file
// with the program create_eventlist and is stored in create_eventlist.root. 
//
//
// Author     : Ursula Berthon (berthon@in2p3.fr)
// Created    : 17.12.2002
// Last update: $Date: 2004/07/07 06:30:08 $
//          by: $Author: goett $
//
///////////////////////////////////////////////////////
#include <iostream>

#include <TROOT.h>
#include <TObjArray.h>
#include <H1Skeleton/H1Tree.h>
#include "H1Steering/H1StdCmdLine.h"

using namespace std;

int main(int argc, char* argv[])
{

    H1StdCmdLine opts;
    opts.Parse(&argc, argv);
	
    // Load files according to H1EventList
    H1Tree::Instance()->Open();            // this statement must be there!
    Int_t nent=H1Tree::Instance()->GetEntries();
    cout <<" Nr Entries: "<<nent<<endl;
  
   // do another selection (they are cumulative)
    H1Tree::Instance()->SelectHat("Epz > 45");
    nent=H1Tree::Instance()->GetEntries();
    cout <<"Entries "<<nent<<endl;

   // go back to standard initial selection
    H1Tree::Instance()->SelectAll();
    nent=H1Tree::Instance()->GetEntries();
    cout <<"Entries for basic selection: "<<nent<<endl;

    return 0;
}



