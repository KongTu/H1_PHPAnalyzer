#ifndef __H1USERCLASS_H
#define __H1USERCLASS_H

// ROOT includes
#include <TObject.h>
#include <TH1.h>


#include "TMath.h" 


class TH2F;  // forward declaration: 
// etc. will be included in the implementation file 
// (H1UserClass.C). 
// This way time will be saved during compilation (no need to 
// look up the header file).
// Note: 
//  to create a streamer for I/O have to be included here!

  
public:
  H1UserClass(); // RooT needs a default contructor!
  explicit H1UserClass(const Char_t *histoname); // explicit to prevent
  //                conversions from const Char_t* to H1UserClass
  virtual ~H1UserClass(); // destructors always virtual
  
  // the following two member functions are examples 
  // They should be virtual to get the power of OO!
  // note the coding convention: The first letter is a capital.
  virtual void FillHistogram();
  virtual void Draw1(Option_t *opt="");
  virtual void Draw2();
  
private:
  
  // here are the data members: note the coding convention, that 
  // the name of all data members should start with the "f".
  TH1F *fHist;  // User histogram. This line will appear in html docu
  TH2F *fHist2; //! User histogram. this histogram will not be written 
  //  when H1UserClass::Write() is invoked. 
  // per default root creates a streamer for all data 
  // members; if a data member should not be written, 
  // add a ! to the comment field: after the data member.    
  
  
  // It is a macro that is used in rootcint to do e.g. root 
  // If persistent data members are changed, the 
  // See http://root.cern.ch/root/Using.html for details
  
    // (this line appears in the html documentation)
};

#endif
