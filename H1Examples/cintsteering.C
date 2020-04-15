/////////////////////////////////////////////////////////////////
// 
// Author     : Martin Goettlich (martin.goettlich@desy.de)
// Created    : 19.08.2004
// Last update: $Date: 2004/08/19 14:24:34 $
//         by : $Author: goett $
//
// This example shows how to use the steering class H1CintSteer
//
/////////////////////////////////////////////////////////////////



#include <stdio.h>
#include <iostream>

#include "TH1F.h"

#include "H1Steering/H1CintSteer.h"


using namespace std;

int main() {
  cout<<"################################################################\n"<<endl;
  cout<<"# This example shows how to use the steering class H1CintSteer #\n"<<endl;
  cout<<"################################################################\n"<<endl;

  H1CintSteer* steer = new H1CintSteer("steeringscript.C");

  cout<<"\nJust to show that it is possible to call a function. It is NOT NECESSARY it is just a useful FEATURE."<<endl;
  cout<<"You can use this mechanism to call member functions of an object you defined in the steering script."<<endl;
  cout<<"For example: H1CreatePartEm::SetCutEmLow(...). Now calling the function func():"<<endl;
  steer->CallFunction("func();");

  cout<<"\nWe read a double and an integer that is defined in the steering:\n"<<endl;
  cout<<"Variable 'cut1' : "<<steer->GetDouble("cut1")<<endl;
  cout<<"Variable 'par' : "<<steer->GetInt("par")<<endl;
  cout<<"Reading a variable that was calculated in the steering:"<<endl;
  cout<<"Variable 'cut2' : "<<steer->GetDouble("cut2")<<endl;

  cout<<"\n\nWe try to read a variable as a double that is defined in the steering as an int. We get an error message:\n"<<endl;
  cout<<"Variable 'par' : "<<steer->GetDouble("par")<<endl;

  cout<<"\n\nIf we try to read a variable that is not defined in the steering we get an error message:\n"<<endl;
  cout<<"Variable 'foo' : "<<steer->GetDouble("foo")<<endl;

  cout<<"\n\nNow we retrieve a TString:\n"<<endl;
  cout<<"Variable 'tstr' : "<<steer->GetString("tstr")<<endl;

  cout<<"\n\nNow we retrieve an object. In this case a TH1F but you can steer any other type of object.\n"<<endl;
  TH1F* hist = (TH1F*) steer->GetPtr("hist","TH1F");
  if(hist) {
     cout<<"Histogram read. Here the title of the histogram: "<<hist->GetTitle()<<endl;
  } else {
     cout<<"Error. Wrong type or object doesn't exist."<<endl;
  }
}
