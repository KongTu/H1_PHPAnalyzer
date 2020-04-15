#include <TCanvas.h>
#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TMath.h>
#include <iostream>
#include <cmath>
#include <vector>

void normalize(char const *fileName="outputLE_TEST.root") {
   TCanvas *c=new TCanvas("test","test",600,300);
   c->Divide(2,1);
   TFile *file=new TFile(fileName);
   TH1 *hist_trackMult_cent;
   file->GetObject("hist_trackMult_cent",hist_trackMult_cent);
   if(!hist_trackMult_cent) cout<<" no hist_trackMult_cent\n";
   TH1 *hist_trackMult_all;
   file->GetObject("hist_trackMult_all",hist_trackMult_all);
   if(!hist_trackMult_all) cout<<" no hist_trackMult_all\n";
   TH2 *hist_DetaDphi_cent;
   file->GetObject("hist_DetaDphi_cent",hist_DetaDphi_cent);
   if(!hist_DetaDphi_cent) cout<<" no hist_DetaDphi_cent\n";
   TH1 *hist_eta_cent;
   file->GetObject("hist_eta_cent",hist_eta_cent);
   if(!hist_eta_cent) cout<<" no hist_eta_cent\n";
   TH2 *hist_DetaDphi_all;
   file->GetObject("hist_DetaDphiFwd_all",hist_DetaDphi_all);
   if(!hist_DetaDphi_all) cout<<" no hist_DetaDphi_all\n";
   TH1 *hist_eta_all;
   file->GetObject("hist_eta_all",hist_eta_all);
   if(!hist_eta_all) cout<<" no hist_eta_all\n";

   TFile *output=new TFile("test.out","recreate");
   vector<double> sumW(2);
   size_t CUT_NCENT=0;
   size_t CUT_NALL=1;
#define NTRACKMIN 10
   for(int i=0;i<hist_trackMult_cent->GetNbinsX();i++) {
      if(hist_trackMult_cent->GetBinCenter(i)>=NTRACKMIN) {
         sumW[CUT_NCENT]+=hist_trackMult_cent->GetBinContent(i);
      }
   }
   for(int i=0;i<hist_trackMult_all->GetNbinsX();i++) {
      if(hist_trackMult_all->GetBinCenter(i)>=NTRACKMIN) {
         sumW[CUT_NALL]+=hist_trackMult_all->GetBinContent(i);
      }
   }

   // normalized histograms

   // average number of events per bin
   //  ntrack*(ntrack-1)*sumOfWeights/nBinDeta/nBinPhi

   //  fold eta with itself
   ///   (ntrack*sumOfweights)**2 /nBinDeta
   //  -> factor:  nBinDeta/nBinPhi/sumOfWeights

   // But the eta histogram is wider than the track eta distribution
   // while the deta histogram is about the size of the deta distributions
   // -> use the ratio of bin widths rather than nBinEta
   //
   // factor = 1/nBinPhi/sumOfWeights
   {
   TH1 *hist_DetaDphiNorm_cent=
      (TH1 *)hist_DetaDphi_cent->Clone("hist_DetaDphiNorm_cent");

   vector<double> normDeta_cent(hist_DetaDphiNorm_cent->GetNbinsX());
   for(int iEta=1;iEta<=hist_eta_cent->GetNbinsX();iEta++) {
      for(int jEta=1;jEta<=hist_eta_cent->GetNbinsX();jEta++) {
         double deta=hist_eta_cent->GetBinCenter(iEta)-
            hist_eta_cent->GetBinCenter(jEta);
         size_t detaBin=hist_DetaDphiNorm_cent->GetXaxis()->FindBin(deta)-1;
         if((detaBin>=0)&&(detaBin<normDeta_cent.size())) {
            normDeta_cent[detaBin] += hist_eta_cent->GetBinContent(iEta)*
               hist_eta_cent->GetBinContent(jEta);
         }
      }
   }
   for(int iEta=1;iEta<=hist_DetaDphiNorm_cent->GetNbinsX();iEta++) {
      double scale=sumW[CUT_NCENT]/normDeta_cent[iEta-1]*
         hist_DetaDphiNorm_cent->GetNbinsY();
      for(int iPhi=1;iPhi<=hist_DetaDphiNorm_cent->GetNbinsY();iPhi++) {
         double c=hist_DetaDphiNorm_cent->GetBinContent(iEta,iPhi);
         double e=hist_DetaDphiNorm_cent->GetBinError(iEta,iPhi);
         hist_DetaDphiNorm_cent->SetBinContent(iEta,iPhi,c*scale);
         hist_DetaDphiNorm_cent->SetBinError(iEta,iPhi,e*scale);
      }
   }
   hist_DetaDphiNorm_cent->Write(); 
   c->cd(1);
   hist_DetaDphiNorm_cent->DrawCopy("LEGO"); 
  }
   {
   TH1 *hist_DetaDphiNorm_all=
      (TH1 *)hist_DetaDphi_all->Clone("hist_DetaDphiNorm_all");

   vector<double> normDeta_all(hist_DetaDphiNorm_all->GetNbinsX());
   for(int iEta=1;iEta<=hist_eta_all->GetNbinsX();iEta++) {
      for(int jEta=1;jEta<=hist_eta_all->GetNbinsX();jEta++) {
         double deta=hist_eta_all->GetBinCenter(iEta)-
            hist_eta_all->GetBinCenter(jEta);
         size_t detaBin=hist_DetaDphiNorm_all->GetXaxis()->FindBin(deta)-1;
         if((detaBin>=0)&&(detaBin<normDeta_all.size())) {
            normDeta_all[detaBin] += hist_eta_all->GetBinContent(iEta)*
               hist_eta_all->GetBinContent(jEta);
         }
      }
   }
   for(int iEta=1;iEta<=hist_DetaDphiNorm_all->GetNbinsX();iEta++) {
      double scale=sumW[CUT_NALL]/normDeta_all[iEta-1]*
         hist_DetaDphiNorm_all->GetNbinsY();
      for(int iPhi=1;iPhi<=hist_DetaDphiNorm_all->GetNbinsY();iPhi++) {
         double c=hist_DetaDphiNorm_all->GetBinContent(iEta,iPhi);
         double e=hist_DetaDphiNorm_all->GetBinError(iEta,iPhi);
         hist_DetaDphiNorm_all->SetBinContent(iEta,iPhi,c*scale);
         hist_DetaDphiNorm_all->SetBinError(iEta,iPhi,e*scale);
      }
   }
   hist_DetaDphiNorm_all->Write();
   c->cd(2);
   hist_DetaDphiNorm_all->DrawCopy("LEGO"); 
   }

   delete output;
   delete file;
}
