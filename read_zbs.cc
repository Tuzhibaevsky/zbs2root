//
// sample.cc     27-DEC-2011     Y.Koshio
//

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

// SK related

#include <skroot.h>
#undef MAXHWSK
#include <skheadC.h>
#include <sktqC.h>

#include <geopmtC.h>

extern "C" {
  void geoset_();
  void skbadopt_(int*);
  void skoptn_(const char*, int);
  int  skread_(int*);
  void skroot_init_(int*);
  void kzinit_();
  void set_rflist_(int*, const char*, const char*, const char*, const char*,
		   const char*, const char*, const char*, const char*, const char*,
		   int, int, int, int, int, int, int, int, int);
  void skopenf_(int*, int*, const char*, int*);
  void skclosef_(int*);
}

#include <TFile.h>
#include <TTree.h>

int main(int argc, char *argv[])
{
  if(argc < 3) {
    printf("Usage: sample input.root output.root\n");
    exit(0);
  }

  kzinit_();

  // open skroot file
  int lun = 10;
  int ipt = 1;
  int openError;
  string fname_in = argv[1];

  set_rflist_(&lun, fname_in.c_str(), "LOCAL", "", "RED", "", "", "recl=5670 status=old", "", "",
	      strlen(fname_in.c_str()),5,0,3,0,0,20,0,0);
  skopenf_(&lun, &ipt, "Z", &openError);
  std::cout << fname_in << std::endl;

  // Set SK options and SK geometry                                           
  int badopt = 55; skbadopt_(&badopt);
  const char* skoptn = "31,30,26,25"; skoptn_(skoptn, strlen(skoptn));
  skheadg_.sk_geometry = 6; geoset_();

  // Read data event-by-event
  int readStatus;
  int eventID = 0;
  bool bEOF = false;

  // Save as TTree
  TFile OutF(argv[2], "RECREATE");
  TTree OutT("T","T");
  std::vector<Int_t> I; std::vector<Double_t> T, Q, X, Y, Z;
  OutT.Branch("ID", &I);
  OutT.Branch("T", &T);
  OutT.Branch("Q", &Q);
  OutT.Branch("X", &X);
  OutT.Branch("Y", &Y);
  OutT.Branch("Z", &Z);


  while (!bEOF) {

    readStatus = skread_(&lun);

    switch (readStatus) {
    case 0: // event read                                               
      {
	eventID++;
	std::cout << " run " << skhead_.nrunsk
		  << " sub " << skhead_.nsubsk
		  << " ev  " << skhead_.nevsk
		  << std::endl;
	std::cout << " NQISK  " << skq_.nqisk
		  << " QISMSK " << skq_.qismsk
		  << std::endl;

	const auto NQISK = skq_.nqisk;

	I.clear();
	T.clear();
	Q.clear();
	X.clear();
	Y.clear();
	Z.clear();

	for (int i=0; i<NQISK; i++){
	  int icab = skchnl_.ihcab[i];
	  I.emplace_back(icab);
	  T.emplace_back(skt_.tisk[icab-1]);
	  Q.emplace_back(skq_.qisk[icab-1]);
	  X.emplace_back(geopmt_.xyzpm[icab-1][0]);
	  Y.emplace_back(geopmt_.xyzpm[icab-1][1]);
	  Z.emplace_back(geopmt_.xyzpm[icab-1][2]);
	}

	OutT.Fill();
	break;
      }
    case 1: // read-error
      {
	break;
      }
    case 2: // end of input
      {
      skclosef_(&lun);
      bEOF = true;
      break;
      }
    } // END SWITCH

  } // END WHILE EOF

  OutT.Write();
  OutF.Close();

  return EXIT_SUCCESS;

}
