#ifndef _AnaUtils_
#define _AnaUtils_

#include <assert.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string>
#include <map>
#include <set>
#include <math.h>
#include <sstream>
#include <iomanip>

#include "TH1F.h"
#include "TCanvas.h"
#include "TFile.h"

#include "tools/src/StatUtils.hh"
#include "tools/src/SystUtils.hh"
#include "tools/src/Format.cc"
#include "tools/src/Debug.cc"

using namespace std;

namespace AUtils {
  const static int kMC=0;
  const static int kGlobal=0;
};

struct EffST{

  float eff;

  //deprecated
  // float systp;
  // float systm;

  //dev
  map<string,float> systsU;
  map<string,float> systsD;

  float sumw;
  float sumw2;
  float N;

  float sumwTot;
  float sumw2Tot;
  float NTot;
  
};

typedef map<string, EffST> eIMap; //efficiencies for one dataset, one categ
typedef vector<eIMap > eMap; //efficiencies for one categ, all datasets
typedef vector<eMap> multiEMap;//efficiencies for all categs, all datasets
typedef eIMap::iterator itEIMap;
typedef eMap::iterator itEMap;
typedef multiEMap::iterator itMultiEMap;


class AnaUtils {

private:

  //Efficiencies
  multiEMap _effMap;
  itEMap _itEMap;
  itEIMap _itEIMap;
  itMultiEMap _itMEMap;

  //workflows
  std::map<int, bool> _isWF;
  int _nWF;
  int _curWF;
  std::map<string, int> _uncWorkflow;

  //Acceptance
  bool _inAcc;
  bool _useAccForEff;

  bool _invCut;
  bool _skipCut;
  set<string> _nm1Var;
  set<string> _bkgFC;
  
  //internal names...
  vector<string> _dsNames;
  map<int,string> _catNames;
  map<int, vector<string> > _effNames; //key=categ
  //map<string, int>::const_iterator _itMSI;
  
  int _kGlobal;
  int _kMC;
  

  string _uncSrc;
  int _uncDir;

public:

  enum {kGeneral=0, kMono=1, kMulti=2};


public:
  
  AnaUtils();
  ~AnaUtils();

  void setUncSrc(string uncSrc, int dir) {
    _uncSrc=uncSrc;
    _uncDir=dir;
  };

  template < typename T > inline
  bool makeCut(T value, T valcut, string type, int ids, string cName, float w, T valUp, T valDo, T seccut=0, int eCateg=AUtils::kGlobal) {

    bool mean = internalMakeCut<T>( value, valcut, type, ids, cName, w, seccut, eCateg, false);

    bool up=mean,down=mean;
   
    up =  simpleCut<T>( valUp, valcut, type, seccut);
    down =  simpleCut<T>( valDo, valcut, type, seccut);
    
    //setSystematics( ds, cName, _uncSrc, up, down, w);
    
    return mean;
  };

  template < typename T > inline
  bool makeCut( T value, T valcut, string type, int ids, string cName, float w, T seccut=0, int eCateg=AUtils::kGlobal, bool noRegister=false) {
    return internalMakeCut<T>(value, valcut,type,ids,cName,w,seccut, eCateg, noRegister);
  };

  bool makeCut( bool decision, int ids, string cName, float w,string type="=", int eCateg=AUtils::kGlobal, bool noRegister=false) {
    return internalMakeCut<bool>( decision, true, type, ids, cName, w, 0, eCateg, noRegister );
  };


  template < typename T > inline
  bool simpleCut( T value, T cut, string type, T seccut=0 ) {
    return internalMakeCut<T>( value, cut, type, 0, "dummy", 0, seccut, -1, true );
  };


  bool simpleCut( bool decision, string type="=") {
    return internalMakeCut<bool>( decision, true, type, 0, "dummy", 0, 0, -1, true );
  };

  
  std::string invCut(string s);
  bool cbool(bool skip, bool namevar);


  //Efficiencies and yields
  void setEfficiency(int ids, string cName, int eCateg, float value, bool acc);
  void setEffFromStat(int ids, string cName, int eCateg, float sw, float esw, int ngen);
  void setSystematics( int ids, string cName, string sName, bool up, bool down, float w);
  void getYieldSysts(string ds, string lvl);

  //workflows
  void setWFEfficiencies(int ids, string cName, float w, bool acc);

  void saveNumbers(string anName, string conName);
  void printNumbers();

  void printTables(string categ="global");

  // =======
// 	vector<string> listFiles(string dir, string files);
  int findElement(vector<string> v, string e);
  //vector< pair<string, vector<vector<float> > > > retrieveNumbers(string anName, string conName, vector<string> snames, vector<string> dsnames, vector<float> weights);
  vector< pair<string, vector<vector<float> > > > retrieveNumbers(string categ, int mcat, string cname);
  bool getDataCardLines(map<string,string>& lines, vector<string> dsNames, string sigName,
			string categ, string cname, int bin,
			map<string,vector<string> > intNuisPars);
 

  //void drawNumbers();
  
  void setSkipCut(vector<string> var, bool invCut); 

  void setDDCuts(vector<string> ddCuts);

  void reset();

  void init();

  static hObs cloneHObs(const hObs* o1);

  void addDataset(string dsName); 
  void addCategory(int iCateg, string eCateg); 
  void addWorkflow(int iCateg, string eCateg); 
  void addAutoWorkflow(string eCateg);
  void setCurrentWorkflow(int wf);
  int getUncWorkflow(string wf);

  int getCategId(string categ);
  
private:

  template < typename T > inline
  bool internalMakeCut( T value, T valcut, string type, int ids, string cName, float w, T seccut=0, int eCateg=AUtils::kGlobal, bool noRegister=false) {

    bool accept;

    if(!cbool(_skipCut, _nm1Var.find(cName)!=_nm1Var.end() ) )
      { return true; }
    if( !cbool( _invCut, _nm1Var.find(cName)!=_nm1Var.end() ) ) {
      type = invCut(type); }
   
    if(type=="<") {
      accept = (value < valcut);
    }
    else if(type=="<=") {
      accept = (value <= valcut);
    }
    else if( type==">") {
      accept = (value > valcut);
    }
    else if( type==">=") {
      accept = (value >= valcut);
    }
    else if( type=="=") {
      accept = (value == valcut);
    }
    else if(type=="!=") {
      accept = (value != valcut);
    }
    else if(type=="[]") {
      accept = (value >= valcut && value<= seccut );
    }
    else if(type=="][") {
      accept = (value > valcut && value< seccut );
    }
    else if(type=="]]") {
      accept = (value > valcut && value <= seccut );
    }
    else if(type=="[[") {
      accept = (value >= valcut && value < seccut );
    }
    else if(type=="[!]") {
      accept = !(value >= valcut && value <= seccut );
    }
    else if(type=="]![") {
      accept = !(value > valcut && value < seccut );
    }
    else {
      accept =false; //cout<<" Warning cut :"<<type<<":"<<" for selection "<<cName<<endl;
    }  
    
    ids+=1;//0 booked for MC

    if(!noRegister || _dsNames[ids].find("GHO")!=(size_t)-1 ) {
      if(_uncSrc=="") {
    	if(eCateg!=_kGlobal) { //eff categories
    	  setEfficiency(ids, cName, eCateg, w, accept);
    	}
    	else {
    	  if( _nWF==1 || _curWF!=-100) { //single workflow
    	    setEfficiency(ids, cName, (_curWF==-100)?eCateg:_curWF , w, accept);
    	  }
    	  else {//multiple workflows
    	    setWFEfficiencies(ids, cName, w, accept);
    	  }
    	}
      }
      else {
    	//separated workflow for uncerrtainty
    	  setEfficiency(ids, cName, _curWF, w, accept);
    	//and systematic uncertainties
    	if(_uncDir==SystUtils::kUp)
    	  setSystematics( ids, cName,_uncSrc,accept,false, w);
    	else if(_uncDir==SystUtils::kDown)
    	  setSystematics( ids, cName,_uncSrc,false,accept, w);
      } 
    }
    if( _bkgFC.find(cName)!=_bkgFC.end() ) {
      accept=true;
    }  
    return accept;
  };


  void setNumbers(int ids,string cName, int eCateg,float w, bool acc);
  void setNumFromStat(int ids,string cName, int iCateg, float sw, float esw, int ngen);

  vector<string> prepareDSNames(bool wMC, vector<int>& idxs);
  
  void internalAddCategory(int iCateg, string eCateg); 


  ClassDef(AnaUtils,0)
  
};

#endif
