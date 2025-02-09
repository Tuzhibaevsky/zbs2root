//
// sample.cc     27-DEC-2011     Y.Koshio
//

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <stdio.h>

// SK related

#include <skroot.h>
#undef MAXHWSK
#include <skheadC.h>
#include <sktqC.h>
#include <geopmtC.h>
#include <vcworkC.h>

#include "/usr/local/sklib_gcc8/atmpd_23c/src/recon/ska/zbsmc.h"

#define MAX_MC_VERTICES           10
#define MAX_MC_PRIMARIES          50
#define MAX_MC_SECONDARIES        400
#define MAX_GENERATOR_PRIMARIES   100

#define ZBSMC_NO_BANK  -1
#define ZBSMC_TOO_MANY -2


extern "C" {
  void geoset_();
  void skbadopt_(int*);
  void skoptn_(const char*, int);
  int  skread_(int*);
  void skroot_init_(int*);
  void kzinit_();
  void vcrdvccm_();
  //void skgetv_();
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
    printf("Usage: read_zbs input.zbs output.root\n");
    exit(0);
  }

  /*
  struct convvect_emb convvect_emb;
  struct convvect     convvect[MAX_MC_SECONDARIES];

    if (get_convvect(&convvect_emb, convvect, MAX_MC_SECONDARIES) !=
      ZBSMC_NO_BANK)
    {
    	std::cout << "CONVVECT NP: " << convvect_emb.n_particles << std::endl;
    }

  */
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
  int    nvc;
  float  posvc[3];
  int    ipvc[100];
  float  amasvc[100];
  float  pvc[100][3];
  int    iorgvc[100];
  int    iflgvc[100];
  int    icrnvc[100];
  float  timvc[100];
  float  posivc[100][3];
  int    ivtivc[100];
  float  posfvc[100][3];
  int    ivtfvc[100];
  /**/
/*     NVC         : # OF PARTICLE*/
/*     POSVC(3)    : VERTEX POSITION OF INITIAL INTERACTION*/
/*     IPVC(I)     : PARTICLE CODE OF I-TH PARTICLE*/
/*     AMASVC(I)   : MASS OF I-TH PARTICLE ( MEV/C**2 )*/
/*     PVC(3,I)    : MOMENTUM OF I-TH PARTICLE ( MEV/C )*/
/*     IORGVC(I)   : ID OF ORIGIN PARTICLE*/
/*     IFLGVC(I)   : FLAG OF FINAL STATE */
/*                   0 : DETERMINED LATER PROCEDURE*/
/*                   1 : DECAY TO OTHER PARTICLE*/
/*                   2 : ESCAPE FROM DETECTOR*/
/*                   3 : ABSORPTION*/
/*                   4 : CHARGE EXCHANGE*/
/*                   5 : STOP AND NOT CONSIDER IN M.C. */
/*                   6 : E.M. SHOWER*/
/*                   7 : Particle production*/
/*                   8 : Inelastic scattering*/
/*     ICRNVC(I)   : FLAG OF TO CHASE OR NOT*/
/*                   0 : DON'T CHASE*/
/*                   1 : CHASE */
/*     TIMVC(I)    : TIME OF VERTEX*/
/*     POSIVC(3,I) : INITIAL VERTEX POSITION*/
/*     IVTIVC(I)   : ID OF INITIAL VERTEX*/
/*     POSFVC(3,I) : FINAL VERTEX POSITION*/
/*     IVTFVC(I)   : ID OF FINAL VERTEX*/
/**/
/*common vcwork was here*/

  OutT.Branch("ID", &I);
  OutT.Branch("T", &T);
  OutT.Branch("Q", &Q);
  OutT.Branch("X", &X);
  OutT.Branch("Y", &Y);
  OutT.Branch("Z", &Z);
  OutT.Branch("nvc", &nvc, "nvc/I");
  OutT.Branch("posvc", &posvc, "posvc[3]/F");
  OutT.Branch("posivc", &posivc, "posivc[nvc][3]/F");
  OutT.Branch("posfvc", &posfvc, "posfvc[nvc][3]/F");
  OutT.Branch("ipvc", &ipvc, "ipvc[nvc]/I");
  OutT.Branch("amasvc", &amasvc, "amasvc[nvc][3]/F");
  OutT.Branch("pvc", &pvc, "pvc[nvc][3]/F");
  OutT.Branch("iorgvc", &iorgvc, "iorgvc[nvc]/I");
  OutT.Branch("iflgvc", &iflgvc, "iflgvc[nvc]/I");
  OutT.Branch("timvc", &timvc, "timvc[nvc]/F");

  //  OutT.Branch("icrnvc", &icrnvc);
  //  OutT.Branch("posivc", &posivc);
  //  OutT.Branch("ivtivc", &ivtivc);
  //  OutT.Branch("posfvc", &posfvc);
  //  OutT.Branch("ivtfvc", &ivtfvc);

  //int counter = 0;
  //
  
  if (argc > 3){
    std::cout << "Using 4th argument, creating geofile with name: " << argv[3] << std::endl;

    char *filename = argv[3];
    FILE *fp = fopen(filename, "w");
    if (fp == NULL)
      {
        printf("Error opening the file %s", filename);
        return -1;
      }

    for (int k=1; k<11147; k++){
      float geofile_x = geopmt_.xyzpm[k-1][0];
      float geofile_y = geopmt_.xyzpm[k-1][1];
      float geofile_z = geopmt_.xyzpm[k-1][2];
      float geofile_dx = geopmt_.dxyzpm[k-1][0];
      float geofile_dy = geopmt_.dxyzpm[k-1][1];
      float geofile_dz = geopmt_.dxyzpm[k-1][2];
      //fprintf(fp, "%f,%f,%f,%f,%f,%f\n", geofile_x, geofile_y, geofile_z, geofile_dx, geofile_dy, geofile_dz);
      //std::cout << "ID: " << k << ", dx: " << geofile_dx << ", dy: " << geofile_dy << ", dz: " << geofile_dz << std::endl;
    }
    fclose(fp);
    return 0;
  }

  while (!bEOF) {
    //    if (counter>0)
    //      break;
    //    counter++;
    readStatus = skread_(&lun);
    vcrdvccm_();
    //skgetv_();
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
	std::cout << " Incident particle ID " << vcwork_.ipvc[0]
		  << ". Total number of particles " << vcwork_.nvc
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

	nvc = vcwork_.nvc;
	posvc[0] = vcwork_.posvc[0];
	posvc[1] = vcwork_.posvc[1];
	posvc[2] = vcwork_.posvc[2];
	for (int i=0; i<nvc; ++i) {
	  ipvc[i] = vcwork_.ipvc[i];
	  amasvc[i] = vcwork_.amasvc[i];
	  timvc[i] = vcwork_.timvc[i];
	  pvc[i][0] = vcwork_.pvc[i][0];
	  pvc[i][1] = vcwork_.pvc[i][1];
	  pvc[i][2] = vcwork_.pvc[i][2];
	  posivc[i][0] = vcwork_.posivc[i][0];
	  posivc[i][1] = vcwork_.posivc[i][1];
	  posivc[i][2] = vcwork_.posivc[i][2];
	  posfvc[i][0] = vcwork_.posfvc[i][0];
	  posfvc[i][1] = vcwork_.posfvc[i][1];
	  posfvc[i][2] = vcwork_.posfvc[i][2];
	  std::cout << "time: " << vcwork_.timvc[i] << std::endl;
	  iorgvc[i] = vcwork_.iorgvc[i];
	  iflgvc[i] = vcwork_.iflgvc[i];	  
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
