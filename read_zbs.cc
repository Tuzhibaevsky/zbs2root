//
// sample.cc     27-DEC-2011     Y.Koshio
//
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

int main(int argc, char *argv[])
{
  if(argc < 2) {
    printf("Usage: sample input.root\n");
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

  while (!bEOF) {

    readStatus = skread_(&lun);

    switch (readStatus) {
    case 0: // event read                                               
      eventID++;
      std::cout << " run " << skhead_.nrunsk
		<< " sub " << skhead_.nsubsk
		<< " ev  " << skhead_.nevsk
		<< std::endl;
      std::cout << " NQISK  " << skq_.nqisk
		<< " QISMSK " << skq_.qismsk
		<< std::endl;

      for (int i=0; i<skq_.nqisk; i++){
	int icab = skchnl_.ihcab[i];
	std::cout << "ID hit: " << i << ": "
		  << icab << " "
		  << skt_.tisk[icab-1] << " "
		  << skq_.qisk[icab-1] << " "
	          << geopmt_.xyzpm[icab-1][0] << " " 
	          << geopmt_.xyzpm[icab-1][1] << " " 
	          << geopmt_.xyzpm[icab-1][2] << " " 
		  << std::endl;
      }

      std::cout << " " << std::endl; 
      break;

    case 1: // read-error
      break;

    case 2: // end of input
      skclosef_(&lun);
      bEOF = true;
      break;
    }
  }
}
