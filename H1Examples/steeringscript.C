////////////////////////////////////////////////////////////////////////
//
// Author     : Martin Goettlich (martin.goettlich@desy.de)
// Created    : 19.08.2004
// Last update: $Date: 2004/08/19 14:24:34 $
//         by : $Author: goett $
//
// This is the steering script read by the example 'cintsteering.C'
//
////////////////////////////////////////////////////////////////////////

/*
 Okay, this is the way a steering-script looks like.
 You can steer anything that can be defined under Cint.
 You can also do any calculations using c++ syntax.
*/ 

// steer some numbers
 double cut1=0.0815; // You could also write the following: 'Double_t cut1=0.0815;' even just 'cut1=0.0815;' is sufficient!
 double cut2=42.42*TMath::Pi(); // you can do some calculations if you want ...
 float  cut3=47.11;
 int    par=78; // Here also just 'par=78;' would be sufficient because in this case Cint assumes it's an integer.
// steer a string
 const char text[]="blafasel"; // I prefer to steer a TString but this is of course also possible.
// steer an object
 TString tstr="This is a TString.";
 TH1F hist("name","This is the title of the histogram you have steered.",50,0,50);

// End of normal steering script.

// Here a FEATURE:
// You can also define functions and call them from your executable. This is useful if you want
// to call member functions of an object you defined in the steering script. For example:
// H1CreatePartEm::SetCutEmLow(...). In this example we change the title of the histogram.

func()
{
  printf("Function func() called. Changing title of histogram to 'changed title'.\n");
  hist.SetTitle("changed title");
}


