////////////////////////////////////////////////////////////////////////
// Program: steermanager
//
// Author     : Ingo Strauch
// Created    : 2001/08/17
// Last update: $Date: 2003/10/10 21:15:30 $
//          by: $Author: wessling $
//
// Description: This program shows how to use the steering classes
//
/////////////////////////////////////////////////////////////////////////

#include <TROOT.h>
#include <iostream>

#include "H1Steering/H1SteerManager.h"
#include "H1Steering/H1CmdLine.h"
#include "H1Steering/H1OptionString.h"
#include "H1Steering/H1OptionMultString.h"
#include "H1Steering/H1OptionMultInt.h"
#include "H1Skeleton/H1SteerTree.h"

using namespace std;

int main(int argc, char* argv[])
{
        // default name for steering file
    H1OptionString     steerfile("steermanager.steer"),name("myfiles");
    H1OptionMultInt    numbers; // defaults not supported yet
    H1OptionMultString strings;

    H1CmdLine opts;
    opts.AddOption("file",'f',steerfile); // override default with '-f file' or '--file=file'
    opts.AddOption("name",'N',name);
    opts.AddOption("number",'n',numbers); // can specify '-n XXX' multiple times
    opts.AddOption("strings",'s',strings);
    opts.Parse(&argc,argv);
    
    H1SteerManager steering;
    steering.ParseFile(steerfile); // one could hardcode the filename here and leave out the option parsing above

        // ROOT RTTI is used intensively so that adding new steering classes means
        // no change to the existing classes at all (in fact if a new class is put
        // into an additional library, the old code doesn't even have to be
        // recompiled except for the executable)
    H1SteerTree *myvalues;
    if (name.IsGiven()) {
            // retrieve named steering
        myvalues = (H1SteerTree*)gH1SteerManager->
            GetSteer( H1SteerTree::Class(), name ); // here's the access via RTTI
    } else {
            // retrieve unnamed steering
        myvalues = (H1SteerTree*)gH1SteerManager->
            GetSteer( H1SteerTree::Class() ); // here's the access via RTTI
    }

    cout << "Access to the values in job:";
    cout << "\n    OdsFiles : ";
    for ( Int_t count=0; count<myvalues->GetOdsFiles()->GetEntries(); ++count ){
      cout << ((TObjString*)myvalues->GetOdsFiles()->At(count))->GetName()
	   << " ";
    }
    cout << "\n    ModsFiles: ";
    for ( Int_t count=0; count<myvalues->GetModsFiles()->GetEntries(); ++count ){
      cout << ((TObjString*)myvalues->GetModsFiles()->At(count))->GetName()
	   << " ";
    }
    cout << "\n    HatFiles : ";
    for ( Int_t count=0; count<myvalues->GetHatFiles()->GetEntries(); ++count ){
      cout << ((TObjString*)myvalues->GetHatFiles()->At(count))->GetName()
	   << " ";
    }
    cout << endl;
    
        // How to use multiple options
    cout << "\nValues of the multiple integer option:\n";
    for (Int_t index=0; index<numbers.GetEntries(); index++ ) {
        cout << index << "=" << numbers[index] << "\n";
    }
    cout << endl;
    
    cout << "\nValues of the multiple string option:\n";
    for (Int_t index=0; index<strings.GetEntries(); index++ ) {
        cout << index << "=" << strings[index] << "\n";
    }
    cout << endl;
    
    return 0;
}
