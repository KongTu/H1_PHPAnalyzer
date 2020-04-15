///////////////////////////////////////////////////////
// Demonstrate decoding of L1 trigger element bits 
// stored in Il1te branch on HAT
//
// Author     : Christian Veelken
// Created    : 05/04/07
// Last update: $Date: 2008/01/29 15:45:13 $
//          by: $Author: ozerov $
//
///////////////////////////////////////////////////////

#include <iostream>

// H1 OO includes
#include "H1Steering/H1StdCmdLine.h"
#include "H1Skeleton/H1Tree.h"
#include "H1Pointers/H1BytePtr.h"
#include "H1Tools/H1TriggerElement.h"

using namespace std;

int main(int argc, char* argv[])
{
//--- Parse the command line
  H1StdCmdLine opts;
  opts.Parse(&argc, argv);

  static H1BytePtr l1te("Il1te");				      

//--- Load HAT files
  H1Tree::Instance()->Open(); // this statement must be here!

  cout << "\nStart looping over " << H1Tree::Instance()->GetEntries() << " events\n" << endl;

  H1TriggerElement zVtxMul("zVtx_mul"); // automatic initialisation from trigger setup steering files (see below)
  //H1TriggerElement zVtxMul(26, 3);      // manual initialisation of starting bit and number of bits in which the trigger element is coded

/* ------  TG d -------- zVtx -------- TE  24- 31 ------
#DEF  zVtx_mul            = d t2:3  coding number of entries in zVtx histogram:
                                    0 or 3 and zVtx_T0: zVtx_small
                                    0 or 1            : 0..5 entries in histo
                                    2 or 3            : 6..10
                                    4                 : 11..100 entries, peak  1..10
                                    5                 : 11..100 entries, peak  11..255
                                    6                 : 101..x entries (x free, 200 or so)
                                    7                 : (x+1)..4095 entries
*/

//--- Loop over events
  Int_t eventCounter = 0;
  while ( H1Tree::Instance()->Next() && (!opts.IsMaxEvent(eventCounter)) ){
    eventCounter++;
    cout << "L1TE : ";
    Bool_t isFirst = true;
    for ( Int_t il1te = zVtxMul.GetIndex(); il1te < (zVtxMul.GetIndex() + zVtxMul.GetSize()); il1te++ ){
      if ( l1te[il1te] ){
	if ( !isFirst ) cout << ", ";
	cout << il1te;
	isFirst = false;
      }
    }
    
    cout << " -> zVtx_mul value = " << zVtxMul.GetValue() << endl;
  }

  return 0;
}
