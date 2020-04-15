////////////////////////////////////////////////////////////////////////
//
// Author     : Ursula Berthon
// Created    : 13.02.2002
//
// Example of access to Ods
//
// Last update: $Date: 2004/03/12 14:37:46 $
//          by: $Author: goett $
//
//
//  This is an example job with all sorts of accesses to the new 
// 'generic' ODS (release 1.8.0 on).
// For a complete description, please have a look at 
// the README in the H1Ods package
//////////////////////////////////////////////////////////////////

#include <stdlib.h>

#include <TROOT.h>
#include <TObjArray.h>
#include <TObjectTable.h>
#include <TArrayI.h>

#include "H1Skeleton/H1Tree.h"
#include "H1OOBanks/H1BankPtr.h"
#include "H1OOBanks/ODS/H1DtryBankPtr.h"
#include "H1OOBanks/ODS/H1Dtry.h"
#include "H1OOBanks/ODS/H1Ghd.h"
#include "H1OOBanks/ODS/H1GhdBankPtr.h"
#include "H1OOBanks/ODS/H1Dmis.h"
#include "H1OOBanks/ODS/H1DmisBankPtr.h"
#include "H1OOBanks/ODS/H1Str.h"
#include "H1OOBanks/ODS/H1StrBankPtr.h"
#include "H1Steering/H1SteerManager.h"
#include "H1Steering/H1StdCmdLine.h"

using namespace std;

int
main(int argc, char* argv[])
{
    // parse the command line 
  // only needed if cmd line options are given, here it is used 
  // to input the nr of events to be processed via command line:
  // read_ods -n 5
  // see opts.IsMaxEvent in event loop below

    H1StdCmdLine opts;
    opts.Parse(&argc, argv);

// initialise H1Tree

  H1Tree *tree = H1Tree::Instance();
  tree->Open();

// initialise pointers: bankname must be in upper case!
  H1DtryBankPtr dtry;                 //specific access
  H1StrBankPtr str;                   //specific access
  H1DmisBankPtr dmis;                 //specific access
  H1GhdBankPtr ghdsp;                 //specific access
  H1BankPtr ghd("Ghd");               //generic  access
  H1BankPtr frme0("FRME");            //generic  access
  H1BankPtr frme1("FRME",1);          //generic  access
  Char_t  txt1[5], txt2[5];

//
// loop over events
//
  Int_t event = 0;
  while (tree->Next() && (!opts.IsMaxEvent(event))) {
 

//Dmis : example for specific print + getters (specific and generic)
      cout << "\nDMIS" << endl;
      if (dmis.GetEntries()) {
          dmis[0]->Print(" ");
          for (Int_t i = 0; i < 4; i++) {
              Int_t y = dmis[0]->GetKl4st(i);
              Int_t y2 = dmis[0]->GetKacst(i);
              Int_t y3 = dmis[0]->GetKrwst(i);
              cout << "Specific DMIS: y: " << y << ", y2:"
                  << y2 << ", y3:" << y3 << endl;
          }
      }

//Ghd: example of character getters (specific and generic)
// Note that you have to reserve the space for the text yourself!!
      cout << "Ghd: test of char getters" << endl;
      if (ghd.GetEntries()) {
          ghdsp[0]->Print(" ");
          for (Int_t i = 0; i < ghd.GetEntries(); i++) {
              ghd[i]->GetChar(2, txt1);
              ghd[i]->GetChar(3, txt2);
              cout << "generic getchar " << i << " " << txt1 << " " << txt2 << endl;
              ghdsp[i]->GetPrgnam1(txt1);
              ghdsp[i]->GetPrgnam2(txt2);
              cout << "specific getchar " << i << " " << txt1 << " " << txt2 << endl;
          }
      }

// FRME: generic print and generic access (bank with bosnumber = 1)
      cout << "FRME " << frme1.GetEntries() << endl;
      frme1.Print();
      if (frme1.GetEntries()) {
          for (Int_t i = 0; i < frme1.GetEntries(); i++) {
              cout << "frme Nchan " << i << " " << frme1[i]->GetInt(0) << endl;
          }
      }

//FRME: how to loop over multiple banks (banknumber is stored in UniqueID)

      TArrayI bosnumbers = frme0.GetAllBankNumbers();
      for(Int_t i=0; i<bosnumbers.GetSize();i++) {
	frme0.SetBankNumber(bosnumbers[i]);
        frme0.Print();
        cout << "Frme bank " << bosnumbers[i] << " has banknumber " << frme0.GetBankNumber() << endl;
      }
      cout << "Number of banks: " << bosnumbers.GetSize() << endl;

//Dtry: test getters for pointers need non D1TM bank
//      printf("DTRY %d \n", dtry.GetEntries());
//      if(dtry.GetEntries()) {
//        for (Int_t ic=0;ic<dtry.GetEntries();ic++) {
//  //Attention: needs a test in case STR bank is not there !!!
//          if (dtry[ic]->GetStr())  printf(" DTRY pointers: get Px from STR %d %f \n",ic,(dtry[ic]->GetStr())->GetPx());
//        }
//      }
      event++;
  }
  return 0;
}












