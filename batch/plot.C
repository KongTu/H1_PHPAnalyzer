void plot(int which) {
   TCanvas *canvas=new TCanvas(TString::Format("plot_%d",which),"",600,600);
   TFile *file[2];
   file[0]=new TFile("outputHE_TEST.root");
   file[1]=new TFile("outputLE_TEST.root");
   if(which==1) {
      // electron tagger fiducial cut
      canvas->Divide(1,2);
      for(int run=0;run<2;run++) {
         canvas->cd(run+1);
         // before cut
         TH1 *h;
         file[run]->GetObject("hist_etag6EvsX_00haveEtag6TriggerCUTFLOW",h);
         h->DrawCopy("COL");
         double x0=h->GetXaxis()->GetXmin();
         double x1=h->GetXaxis()->GetXmax();
         double y0=h->GetYaxis()->GetXmin();
         double y1=h->GetYaxis()->GetXmax();
         double x[]={x0,x1,x1,3.,3.,0.,-3.,  -3.,x0,x0};
         double y[]={y0,y0,y1,y1,y0,y0,3./0.4,y1,y1,y0};
         TPolyLine *cut=new TPolyLine(sizeof(x)/sizeof(double),x,y);
         cut->SetFillStyle(3544);
         cut->SetFillColor(kBlack);
         cut->SetLineWidth(1);
         cut->Draw("f");
         cut->Draw("");
         TString title="E-tagger, high-energy run";
         if(run==1) {
            title="E-tagger, low-energy run";
         }
         TLegend *legend=new TLegend(0.4,0.75,0.8,0.9,title);
         legend->SetTextSize(0.07);
         legend->Draw();
      }
   } else if(which==2) {
      canvas->Divide(2,2);
      for(int run=0;run<2;run++) {
         canvas->cd(2*run+1);
         TH1 *h[2];
         file[run]->GetObject("hist_etag6PLUSphtag_01etag6acceptanceCUTFLOW",h[0]);
         file[run]->GetObject("hist_etag6PLUSphtag_02etag6PLUSphtagLT20CUTFLOW",h[1]);
         gPad->SetLogy();
         h[0]->SetLineColor(kBlue);
         TObject *o0=h[0]->DrawCopy("HIST");
         h[1]->SetLineColor(kBlue);
         h[1]->SetFillColor(kBlue-7);
         h[1]->SetFillStyle(1001);
         TObject *o1=h[1]->DrawCopy("SAME HIST");
         TString title="E-tagger, high-energy run";
         if(run==1) {
            title="E-tagger, low-energy run";
         }
         TLegend *legend=new TLegend(0.4,0.75,0.9,0.9,title);
         legend->SetFillStyle(0);
         legend->SetBorderSize(0);
         legend->SetTextSize(0.05);
         legend->AddEntry(o1,"accepted","f");
         legend->Draw();
         canvas->cd(2*run+2);
         TH2 *h2d;
         file[run]->GetObject("hist_etag6phtag_01etag6acceptanceCUTFLOW",h2d);
         h2d->DrawCopy("COLZ");
         double x0=h2d->GetXaxis()->GetXmin();
         double x1=h2d->GetXaxis()->GetXmax();
         double y0=h2d->GetYaxis()->GetXmin();
         double y1=h2d->GetYaxis()->GetXmax();
         double x[]={x0,x0,20.,x1,x1,x0};
         double y[]={y1,20.,y0,y0,y1,y1};
         TPolyLine *cut=new TPolyLine(sizeof(x)/sizeof(double),x,y);
         cut->SetFillStyle(3544);
         cut->SetFillColor(kBlack);
         cut->SetLineWidth(1);
         cut->Draw("f");
         cut->Draw("");
      }
   } else if(which==3) {
      canvas->Divide(2,2);
      for(int run=0;run<2;run++) {
         canvas->cd(2*run+1);
         TH2 *h2d;
         file[run]->GetObject("hist_WhfsWetag6_03vertextypeCUTFLOW",h2d);
         double x0=h2d->GetXaxis()->GetXmin();
         double x1=h2d->GetXaxis()->GetXmax();
         double y0=h2d->GetYaxis()->GetXmin();
         double y1=320.;//h2d->GetYaxis()->GetXmax();
         h2d->GetYaxis()->SetRangeUser(y0,y1);
         h2d->DrawCopy("COLZ");
         double x[]={x0,x0,y0/2.,y1/2.,x0};
         double y[]={y1,y0,y0   ,y1   ,y1};
         TPolyLine *cut=new TPolyLine(sizeof(x)/sizeof(double),x,y);
         cut->SetFillStyle(3544);
         cut->SetFillColor(kBlack);
         cut->SetLineWidth(1);
         cut->Draw("f");
         cut->Draw("");
         canvas->cd(2*run+2);
         TH1 *h[3];
         file[run]->GetObject("hist_zvertex_03vertextypeCUTFLOW",h[0]);
         file[run]->GetObject("hist_zvertex_04twoWhfsGTWetag6CUTFLOW",h[1]);
         file[run]->GetObject("hist_zvertex_05zvertex30CUTFLOW",h[2]);
         gPad->SetLogy();
         h[0]->SetLineColor(kBlue);
         TObject *o0=h[0]->DrawCopy("HIST");
         h[1]->SetLineColor(kBlue);
         h[1]->SetFillColor(kBlue-9);
         h[1]->SetFillStyle(1001);
         TObject *o1=h[1]->DrawCopy("SAME HIST");
         h[2]->SetLineColor(kBlue);
         h[2]->SetFillColor(kBlue-6);
         h[2]->SetFillStyle(1001);
         TObject *o2=h[2]->DrawCopy("SAME HIST");
         TString title="high-energy run";
         if(run==1) {
            title="low-energy run";
         }
         TLegend *legend=new TLegend(0.05,0.8,0.45,0.98,title);
         legend->SetFillStyle(1001);
         legend->SetFillColor(kWhite);
         legend->SetBorderSize(1);
         legend->SetTextSize(0.05);
         legend->AddEntry(o1,"W(hfs) cut","f");
         legend->AddEntry(o2,"z-vertex cut","f");
         legend->Draw();         
      }
   } else if(which==4) {
      canvas->Divide(2,2);
      for(int run=0;run<2;run++) {
         canvas->cd(2*run+1);
         TH1 *h[4];
         file[run]->GetObject("hist_eta_cent",h[0]);
         file[run]->GetObject("hist_eta_all",h[1]);
         file[run]->GetObject("hist_trackMult_cent",h[2]);
         file[run]->GetObject("hist_trackMult_all",h[3]);
         h[1]->SetLineColor(kBlue);
         h[1]->GetXaxis()->SetRangeUser(-3.,4.);
         TObject *o0=h[1]->DrawCopy("HIST");
         h[0]->SetLineColor(kBlue);
         h[0]->SetFillColor(kBlue-9);
         h[0]->SetFillStyle(1001);
         TObject *o1=h[0]->DrawCopy("SAME HIST");
         TString title="high-energy run";
         if(run==1) {
            title="low-energy run";
         }
         TLegend *legend=new TLegend(0.3,0.3,0.7,0.5,title);
         legend->SetFillStyle(1001);
         legend->SetFillColor(kWhite);
         legend->SetBorderSize(1);
         legend->SetTextSize(0.05);
         legend->AddEntry(o1,"central tracks","f");
         legend->AddEntry(o0,"all tracks","f");
         legend->Draw();

         canvas->cd(2*run+2);
         gPad->SetLogy();
         h[3]->GetXaxis()->SetRangeUser(-0.5,70.);
         h[3]->SetLineColor(kBlue);
         h[3]->DrawCopy("HIST");
         h[2]->SetLineColor(kBlue);
         h[2]->SetFillColor(kBlue-9);
         h[2]->SetFillStyle(1001);
         h[2]->DrawCopy("SAME HIST");

         TLegend *legend2=new TLegend(0.4,0.8,0.9,0.9,title);
         legend2->SetFillStyle(0);
         legend2->SetFillColor(kWhite);
         legend2->SetBorderSize(0);
         legend2->SetTextSize(0.05);
         legend2->AddEntry((TObject *)0,"track multiplicity","");
         legend2->Draw();
         
      }
   } else if(which==5) {
      canvas->Divide(2,2);
      for(int run=0;run<2;run++) {
         TH2 *h[2];
         file[run]->GetObject("hist_DetaDphiNorm_cent",h[0]);
         file[run]->GetObject("hist_DetaDphiNorm_all",h[1]);
         canvas->cd(2*run+1);
         h[0]->DrawCopy("LEGO");
         TString title="high-energy run";
         if(run==1) {
            title="low-energy run";
         }
         TLegend *legend=new TLegend(0.55,0.8,0.9,0.98,title);
         legend->SetFillStyle(1001);
         legend->SetFillColor(kWhite);
         legend->SetBorderSize(1);
         legend->SetTextSize(0.05);
         legend->AddEntry((TObject *)0,"central tracks","");
         legend->Draw();
         canvas->cd(2*run+2);
         h[1]->DrawCopy("LEGO");
         TLegend *legend2=new TLegend(0.55,0.8,0.9,0.98,title);
         legend2->SetFillStyle(1001);
         legend2->SetFillColor(kWhite);
         legend2->SetBorderSize(1);
         legend2->SetTextSize(0.05);
         legend2->AddEntry((TObject *)0,"all tracks","");
         legend2->AddEntry((TObject *)0,"(extended #Delta#eta)","");
         legend2->Draw();
         
      }
   }

   canvas->SaveAs(Form("plot%d.png",which));

   delete file[0];
   delete file[1];
}
