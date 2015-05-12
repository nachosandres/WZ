#ifndef _Sample_
#define _Sample_

#include <vector>
#include <stdlib.h>
#include <string>
#include <map>
#include "TROOT.h"

using namespace std;

class Sample {

private:

  string _name;
  int _nEvents;
  int _nProcEvents;
  
  float _xSection;
  float _kFactor;
  
  float _eqLumi;

  float _weight; //weight for 1 pb-1

  // bool _isDDriven;
  // bool _isGhost;
  // int _csCode;

public:

  Sample();
  Sample(string name, int n, int nProcEvt, 
	 float xSect, float kfact, float eqLumi);

  virtual ~Sample();

  void setName(string N){ _name=N;};
  void setNEvts(int n){ _nEvents=n;};
  void setNProcEvts(int n){ _nProcEvents=n;};

  void setXSect(float XS) { _xSection=XS;};
  void setKFact(float KF) { _kFactor=KF;};
  void setEqLum(float L) { _eqLumi=L;};
  
  //access functions
  
  string getName(){ return _name;};
  int getNEvts(){ return _nEvents;};
  int getNProcEvts(){ return _nProcEvents;};
  
  float getXSect() {return _xSection;};
  float getKFact() {return _kFactor;};
  float getEqLum() {return _eqLumi;};

  float getLumW() {return _weight;}; //weight for 1 pb-1

  // int isCS(){ return _csCode;};
  // bool isDataDriven(){ return _isDDriven;};
  // bool isGhost(){ return _isGhost;};

private:

  void computeWeight();

  ClassDef(Sample,0)  
};

#endif
