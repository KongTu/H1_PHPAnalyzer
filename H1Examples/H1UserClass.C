// This line is for html-docu:
// Author: Gero Flucke <mailto:flucke@mail.desy.de>
// Behind this line starts the class description for html:
//____________________________________
// H1UserClass:
//   Author:      Judith Katzy
//   Date:        ?
//   last update: $Date: 2003/09/22 14:51:41 $  
//   by:          $Author: wessling $
//               'Date' and 'Author' inside '$' 
//                 will be expanded by CVS
//
//  Trivial example of a class fully integrated into the
//  RooT framework and the html documentation system.
//  Its member functions do nothing.
//
//  If you want to use it to your program you have to
//  add H1UserClass.o and H1UserClassDict.o to the
//  object files list. The latter will be auto-generated.

// The class description stops at the first line that has no 
// '//' at the beginning. 

#include <iostream>
// RooT header:
#include <TH1.h>
#include <TH2.h>
// my header:
#include "H1UserClass.h"

// This is another RooT macro:
ClassImp(H1UserClass)


H1UserClass::H1UserClass()
{
  // default constructor: no histograms are set!

  // The first commented lines in the class implementation 
  // appear in the html documentation as class description - 
  // again untill the first line without '//' in the beginning.
  fHist = NULL;
  fHist2 = NULL;
}

H1UserClass::H1UserClass(const Char_t *histoname)
{
  // constructor creates a TH1F with name histoname
  fHist = new TH1F("histoname", "title", 40, 0, 10);
  fHist2 = NULL;
}


H1UserClass::~H1UserClass()
{
  // destructor deletes histos
  if(fHist) {
    delete fHist;
    fHist = NULL;
  }
  if(fHist2) {
    delete fHist2;
    fHist2 = NULL;
  }
}


void H1UserClass::FillHistogram()
{
  // nothing is done!
  cout << "H1UserClass::FillHistogram() not yet implemented!" 
       << endl;
}


void H1UserClass::Draw1(Option_t *opt="")
{
  // draw fHist with option opt
  if(fHist) {
    fHist->Draw(opt);
  } else {
    cout << "fHist is empty, nothing to draw!" << endl;
  }
}


void H1UserClass::Draw2()
{
  // draw fHist2 with option "BOX"
  if(fHist2) {
    fHist->Draw("BOX");
  } else {
    cout << "fHist2 is empty, nothing to draw!" << endl;
  }
}
