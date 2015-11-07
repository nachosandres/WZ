/*****************************************************************************
******************************************************************************
******************************************************************************
**                                                                          **
** The Multi-Purpose Analysis Framework                                     **
**                                                                          **
** Constantin Heidegger, Matthieu Marionneau                                **
** CERN, Fall 2014                                                          **
**                                                                          **
******************************************************************************
******************************************************************************
*****************************************************************************/


#include "analysis/src/FakeRatio.hh"





/*****************************************************************************
******************************************************************************
** CLASS MEMBERS FOR RUNNING THE CODE                                       **
******************************************************************************
*****************************************************************************/


//____________________________________________________________________________
FakeRatio::FakeRatio(std::string cfg){
  /* 
     constructs the FakeRatio class 
     parameters: configuration_file
     return: none
  */
  
  startExecution(cfg);
  initialize();

}


//____________________________________________________________________________
FakeRatio::~FakeRatio(){
  /* 
     destructs the FakeRatio class 
     parameters: none
     return: none
  */

}


//____________________________________________________________________________
int FakeRatio::findBin(vector<float> list, float value){

  int idx = -1;
  float diff = 9999999999;
  for(unsigned int i = 0; i < list.size(); ++i){
    if(list[i] <= value && value - list[i] < diff){
      diff = value - list[i];
      idx = i;
    }
  }

  return idx;

}


//____________________________________________________________________________
vector<vector<vector<string> > > FakeRatio::findTriggerLines(vector<vector<string> > trigger_lines, \
                                                             vector<float> trigger_pts, vector<float> trigger_etas, \
                                                             vector<float> search_pts , vector<float> search_etas){

  vector<vector<vector<string> > > result_lines;
  result_lines.resize(search_etas.size());

  // CH: this is a bit nasty, we put in etaidxs and ptidxs only the last
  // bin, that we have found, we should be able to put more there

  string line = "";
  for(unsigned int ii = 0; ii < search_etas.size(); ++ii){
    result_lines[ii].resize(search_pts.size());
	vector<int> etaidxs;
    etaidxs.push_back(findBin(trigger_etas, search_etas[ii]));
    for(unsigned int ij = 0; ij < search_pts.size(); ++ij){
      vector<int> ptidxs;
      ptidxs.push_back(findBin(trigger_pts, search_pts[ij]));
      for(unsigned int ie = 0; ie < etaidxs.size(); ++ie)
        for(unsigned int ip = 0; ip < ptidxs.size(); ++ip)
          if(etaidxs[ie] == -1 || ptidxs[ip] == -1) line = "";
          else line = trigger_lines[etaidxs[ie]][ptidxs[ip]];
          result_lines[ii][ij].push_back(line);
    }
  } 

  return result_lines;

}


//____________________________________________________________________________
vector<float> FakeRatio::findTriggerPts(vector<vector<string> > trigger_lines, string name){

  vector<float> trigger_pts;
  for(unsigned int i = 0; i < trigger_lines[0].size(); ++i){
    unsigned int pos = trigger_lines[0][i].find(name) + name.size();
    string num = trigger_lines[0][i].substr(pos, trigger_lines[0][i].find("_", pos) - pos);
    trigger_pts.push_back(atof(num.c_str()));
  }

  return trigger_pts;

}


//____________________________________________________________________________
void FakeRatio::initialize(){
  /*
    initializes the FakeRatio class
    parameters: none
    return: none
  */


  // trigger lines
 
  string TR_lines_el_non[1] = {"HLT_FR_Ele12_CaloIdM_TrackIdM_PFJet30"};
  string TR_lines_el_iso[1] = {"HLT_FR_Ele12_CaloIdL_TrackIdL_IsoVL_PFJet30"};
  //string TR_lines_mu_non[1] = {"HLT_FR_Mu17"};
  //string TR_lines_mu_iso[1] = {"HLT_FR_Mu17_TrkIsoVVL"};


  //string TR_lines_el_non[4] = {"HLT_FR_Ele8_CaloIdM_TrackIdM_PFJet30" , \
  //                             "HLT_FR_Ele12_CaloIdM_TrackIdM_PFJet30", \
  //                             "HLT_FR_Ele23_CaloIdM_TrackIdM_PFJet30", \
  //                             "HLT_FR_Ele33_CaloIdM_TrackIdM_PFJet30"};
  //string TR_lines_el_iso[3] = {"HLT_FR_Ele12_CaloIdL_TrackIdL_IsoVL_PFJet30", \
  //                             "HLT_FR_Ele23_CaloIdL_TrackIdL_IsoVL_PFJet30", \
  //                             "HLT_FR_Ele33_CaloIdL_TrackIdL_IsoVL_PFJet30"};
  string TR_lines_mu_non[2] = {"HLT_FR_Mu8" , \
                               "HLT_FR_Mu17"};
  string TR_lines_mu_iso[2] = {"HLT_FR_Mu8_TrkIsoVVL" , \
                               "HLT_FR_Mu17_TrkIsoVVL"};
  //                             "HLT_FR_Ele18_CaloIdM_TrackIdM_PFJet30", \
  //                             "HLT_FR_Ele18_CaloIdL_TrackIdL_IsoVL_PFJet30", \
  //string TR_lines_mu_non[4] = {"HLT_FR_Mu8" , \
  //                             "HLT_FR_Mu17", \
  //                             "HLT_FR_Mu24", \
  //                             "HLT_FR_Mu34"};
  //string TR_lines_mu_iso[4] = {"HLT_FR_Mu8_TrkIsoVVL" , \
  //                             "HLT_FR_Mu17_TrkIsoVVL", \
  //                             "HLT_FR_Mu24_TrkIsoVVL", \
  //                             "HLT_FR_Mu34_TrkIsoVVL"};

  vector<string> veln = Tools::toVector(TR_lines_el_non);
  vector<string> veli = Tools::toVector(TR_lines_el_iso);
  vector<string> vmun = Tools::toVector(TR_lines_mu_non);
  vector<string> vmui = Tools::toVector(TR_lines_mu_iso);

  // tree branches containing hlt prescales per event
  string TR_psbh_el_non[1] = {"HLT_BIT_HLT_Ele12_CaloIdM_TrackIdM_PFJet30_v_prescaleHLT"};
  string TR_psbh_el_iso[1] = {"HLT_BIT_HLT_Ele12_CaloIdL_TrackIdL_IsoVL_PFJet30_v_prescaleHLT"};
  string TR_psbh_mu_non[2] = {"HLT_BIT_HLT_Mu8_v_prescaleHLT" , \
                              "HLT_BIT_HLT_Mu17_v_prescaleHLT"};
  string TR_psbh_mu_iso[2] = {"HLT_BIT_HLT_Mu8_TrkIsoVVL_v_prescaleHLT" , \
                              "HLT_BIT_HLT_Mu17_TrkIsoVVL_v_prescaleHLT"};

  // tree branches containing l1 prescales per event
  string TR_psbl_el_non[1] = {"HLT_BIT_HLT_Ele12_CaloIdM_TrackIdM_PFJet30_v_prescaleL1Min"};
  string TR_psbl_el_iso[1] = {"HLT_BIT_HLT_Ele12_CaloIdL_TrackIdL_IsoVL_PFJet30_v_prescaleL1Min"};
  string TR_psbl_mu_non[2] = {"HLT_BIT_HLT_Mu8_v_prescaleL1Min" , \
                              "HLT_BIT_HLT_Mu17_v_prescaleL1Min"};
  string TR_psbl_mu_iso[2] = {"HLT_BIT_HLT_Mu8_TrkIsoVVL_v_prescaleL1Min" , \
                              "HLT_BIT_HLT_Mu17_TrkIsoVVL_v_prescaleL1Min"};

  // trigger effective luminosities
  //float TR_efflum_el_non[1] = {getCfgVarF("LUMINOSITY_ELE12")};
  //float TR_efflum_el_iso[1] = {getCfgVarF("LUMINOSITY_ELE12_ISO")};
  //float TR_efflum_mu_non[1] = {getCfgVarF("LUMINOSITY_MU17")};
  //float TR_efflum_mu_iso[1] = {getCfgVarF("LUMINOSITY_MU17_ISO")};

  float TR_efflum_el_non[5] = {getCfgVarF("LUMINOSITY_ELE8") , \
                               getCfgVarF("LUMINOSITY_ELE12"), \
                               getCfgVarF("LUMINOSITY_ELE18"), \
                               getCfgVarF("LUMINOSITY_ELE23"), \
                               getCfgVarF("LUMINOSITY_ELE33")};
  float TR_efflum_el_iso[4] = {getCfgVarF("LUMINOSITY_ELE12_ISO"), \
                               getCfgVarF("LUMINOSITY_ELE18_ISO"), \
                               getCfgVarF("LUMINOSITY_ELE23_ISO"), \
                               getCfgVarF("LUMINOSITY_ELE33_ISO")};
  float TR_efflum_mu_non[2] = {getCfgVarF("LUMINOSITY_MU8") , \
                               getCfgVarF("LUMINOSITY_MU17")};
  float TR_efflum_mu_iso[2] = {getCfgVarF("LUMINOSITY_MU8_ISO") , \
                               getCfgVarF("LUMINOSITY_MU17_ISO")};
  //float TR_efflum_mu_non[4] = {getCfgVarF("LUMINOSITY_MU8") , \
  //                             getCfgVarF("LUMINOSITY_MU17"), \
  //                             getCfgVarF("LUMINOSITY_MU24"), \
  //                             getCfgVarF("LUMINOSITY_MU34")};
  //float TR_efflum_mu_iso[4] = {getCfgVarF("LUMINOSITY_MU8_ISO") , \
  //                             getCfgVarF("LUMINOSITY_MU17_ISO"), \
  //                             getCfgVarF("LUMINOSITY_MU24_ISO"), \
  //                             getCfgVarF("LUMINOSITY_MU34_ISO")};

  // trigger lines
  _vTR_lines_el_non.push_back(veln);
  _vTR_lines_el_iso.push_back(veli);
  _vTR_lines_mu_non.push_back(vmun);
  _vTR_lines_mu_iso.push_back(vmui);

  // tree branches containing hlt prescales per event
  _vTR_psbh_el_non    = Tools::toVector(TR_psbh_el_non);
  _vTR_psbh_el_iso    = Tools::toVector(TR_psbh_el_iso);
  _vTR_psbh_mu_non    = Tools::toVector(TR_psbh_mu_non);
  _vTR_psbh_mu_iso    = Tools::toVector(TR_psbh_mu_iso);

  // tree branches containing l1 prescales per event
  _vTR_psbl_el_non    = Tools::toVector(TR_psbl_el_non);
  _vTR_psbl_el_iso    = Tools::toVector(TR_psbl_el_iso);
  _vTR_psbl_mu_non    = Tools::toVector(TR_psbl_mu_non);
  _vTR_psbl_mu_iso    = Tools::toVector(TR_psbl_mu_iso);

  // trigger effective luminosities
  _vTR_efflum_el_non = Tools::toVector(TR_efflum_el_non);
  _vTR_efflum_el_iso = Tools::toVector(TR_efflum_el_iso);
  _vTR_efflum_mu_non = Tools::toVector(TR_efflum_mu_non);
  _vTR_efflum_mu_iso = Tools::toVector(TR_efflum_mu_iso);

  // trigger pt bins
  _vTR_bins_pt_el_non = findTriggerPts(_vTR_lines_el_non, "Ele");
  _vTR_bins_pt_el_iso = findTriggerPts(_vTR_lines_el_iso, "Ele");
  _vTR_bins_pt_mu_non = findTriggerPts(_vTR_lines_mu_non, "Mu");
  _vTR_bins_pt_mu_iso = findTriggerPts(_vTR_lines_mu_iso, "Mu");

  // trigger eta bins
  _vTR_bins_eta_el.push_back(0);
  _vTR_bins_eta_mu.push_back(0);

  // fake ratio pt bins
  float FR_bins_pt_el[6]  = {10.0, 15.0, 25.0, 35.0, 50.0, 70.0};
  float FR_bins_pt_mu[6]  = {10.0, 15.0, 25.0, 35.0, 50.0, 70.0};
  _vFR_bins_pt_el  = Tools::toVector(FR_bins_pt_el );
  _vFR_bins_pt_mu  = Tools::toVector(FR_bins_pt_mu );

  // fake ratio eta bins
  //float FR_bins_eta_el[4] = { 0.0, 1.0, 2.0, 2.5};
  //float FR_bins_eta_mu[4] = { 0.0, 1.0, 2.0, 2.4};
  float FR_bins_eta_el[4] = { 0.0, 0.8, 1.479, 2.5};
  float FR_bins_eta_mu[4] = { 0.0, 1.2, 2.1, 2.4};
  _vFR_bins_eta_el = Tools::toVector(FR_bins_eta_el);
  _vFR_bins_eta_mu = Tools::toVector(FR_bins_eta_mu);


  // trigger - fake ratio correspondence lines
  // i.e. which trigger line do we need for a given fake ratio pt bin? 
  _vTR_lineperpteta_el_non = findTriggerLines(_vTR_lines_el_non, _vTR_bins_pt_el_non, _vTR_bins_eta_el, _vFR_bins_pt_el, _vFR_bins_eta_el);
  _vTR_lineperpteta_el_iso = findTriggerLines(_vTR_lines_el_iso, _vTR_bins_pt_el_iso, _vTR_bins_eta_el, _vFR_bins_pt_el, _vFR_bins_eta_el);
  _vTR_lineperpteta_mu_non = findTriggerLines(_vTR_lines_mu_non, _vTR_bins_pt_mu_non, _vTR_bins_eta_mu, _vFR_bins_pt_mu, _vFR_bins_eta_mu);
  _vTR_lineperpteta_mu_iso = findTriggerLines(_vTR_lines_mu_iso, _vTR_bins_pt_mu_iso, _vTR_bins_eta_mu, _vFR_bins_pt_mu, _vFR_bins_eta_mu);


  //Tree Variables
  _bvar  = "nBJetMedium25";
  _leps  = "LepGood";
  _jets  = "Jet"; 
  _djets = "DiscJet"; 
  _mets  = "met";
  _nvert = "nVert";

  //Tree Branches
  _vc->registerVar("run"                           );
  _vc->registerVar("lumi"                          );
  _vc->registerVar("evt"                           );
  _vc->registerVar(_nvert                          );
  _vc->registerVar("nTrueInt"                      );
  _vc->registerVar("vtxWeight"                     );
  _vc->registerVar("genWeight"                     );
  _vc->registerVar("n" + _leps                     );
  _vc->registerVar(_leps + "_pt"                   );
  _vc->registerVar(_leps + "_eta"                  );
  _vc->registerVar(_leps + "_phi"                  );
  _vc->registerVar(_leps + "_charge"               );
  _vc->registerVar(_leps + "_tightCharge"          );
  _vc->registerVar(_leps + "_mediumMuonId"         );
  _vc->registerVar(_leps + "_mvaIdPhys14"          );
  _vc->registerVar(_leps + "_mvaIdSpring15"        );
  _vc->registerVar(_leps + "_pdgId"                );
  _vc->registerVar(_leps + "_relIso03"             );
  _vc->registerVar(_leps + "_relIso04"             );
  _vc->registerVar(_leps + "_jetPtRatiov2"         );
  _vc->registerVar(_leps + "_jetPtRelv2"           );
  _vc->registerVar(_leps + "_jetRawPt"             );
  _vc->registerVar(_leps + "_jetCorrFactor_L1L2L3Res");
  _vc->registerVar(_leps + "_miniRelIso"           );
  _vc->registerVar(_leps + "_chargedHadRelIso03"   );
  _vc->registerVar(_leps + "_miniRelIso"           );
  _vc->registerVar(_leps + "_miniRelIso"           );
  _vc->registerVar(_leps + "_dxy"                  );
  _vc->registerVar(_leps + "_dz"                   );
  _vc->registerVar(_leps + "_sip3d"                );
  _vc->registerVar(_leps + "_pfMuonId"             );
  _vc->registerVar(_leps + "_tightId"              );
  _vc->registerVar(_leps + "_looseIdSusy"          );
  _vc->registerVar(_leps + "_convVeto"             );
  _vc->registerVar(_leps + "_lostHits"             );
  _vc->registerVar(_leps + "_eleCutIdCSA14_50ns_v1");
  _vc->registerVar(_leps + "_eleCutIdCSA14_50ns_v1");
  _vc->registerVar(_leps + "_eleCutId2012_full5x5" );
  _vc->registerVar(_leps + "_mvaSusyPHYS14"        );
  _vc->registerVar(_leps + "_mvaSusy"              );
  _vc->registerVar(_leps + "_mcMatchId"            );
  _vc->registerVar(_leps + "_mcMatchAny"           );
  _vc->registerVar(_leps + "_jetBTagCSV"           );
  _vc->registerVar(_leps + "_sigmaIEtaIEta"        );
  _vc->registerVar(_leps + "_hadronicOverEm"       );
  _vc->registerVar(_leps + "_eInvMinusPInv"        );
  _vc->registerVar(_leps + "_dEtaScTrkIn"          );
  _vc->registerVar(_leps + "_dPhiScTrkIn"          );
  _vc->registerVar(_leps + "_ecalPFClusterIso"     );
  _vc->registerVar(_leps + "_hcalPFClusterIso"     );
  _vc->registerVar(_leps + "_dr03TkSumPt"          );
  _vc->registerVar(_mets + "_pt"                   );
  _vc->registerVar(_mets + "_eta"                  );
  _vc->registerVar(_mets + "_phi"                  );
  _vc->registerVar(_mets + "_mass"                 );
  _vc->registerVar("n" + _jets                     );
  _vc->registerVar(_jets + "_id"                   );
  _vc->registerVar(_jets + "_pt"                   );
  _vc->registerVar(_jets + "_rawPt"                );
  _vc->registerVar(_jets + "_eta"                  );
  _vc->registerVar(_jets + "_phi"                  );
  _vc->registerVar(_jets + "_mass"                 );
  _vc->registerVar(_jets + "_btagCSV"              );
  _vc->registerVar("n" + _djets                    );
  _vc->registerVar(_djets + "_id"                  );
  _vc->registerVar(_djets + "_pt"                  );
  _vc->registerVar(_djets + "_rawPt"               );
  _vc->registerVar(_djets + "_eta"                 );
  _vc->registerVar(_djets + "_phi"                 );
  _vc->registerVar(_djets + "_mass"                );
  _vc->registerVar(_djets + "_btagCSV"             );
  
  //generator informations
  _vc->registerVar("nGenPart"                      );
  _vc->registerVar("GenPart_pt"                    );
  _vc->registerVar("GenPart_eta"                   );
  _vc->registerVar("GenPart_phi"                   );
  _vc->registerVar("GenPart_pdgId"                 );
  _vc->registerVar("GenPart_motherId"              );
  _vc->registerVar("GenPart_grandmotherId"         );

  _vc->registerVar("nGenBHad"                      );
  _vc->registerVar("GenBHad_pt"                    ); 
  _vc->registerVar("GenBHad_eta"                   ); 
  _vc->registerVar("GenBHad_phi"                   ); 
  _vc->registerVar("GenBHad_pdgId"                 ); 
 
  //bjets
  _vc->registerVar("nBJetLoose25"                  );
  _vc->registerVar("nBJetMedium40"                 );
  _vc->registerVar("nBJetMedium25"                 );
  _vc->registerVar("nSoftBJetMedium25"             );

  //triggers
  registerTriggerVars();


  //additional counter categories
  _au->addCategory( kEwkSel  , "Ewk Enriched MR Sel"     );
  _au->addCategory( kUcsSel  , "UCSX Ewk Enriched MR Sel");
  _au->addCategory( kQcdSel  , "Qcd Enriched MR Sel"     );
  _au->addCategory( kTrigger , "Trigger Sel"             );
  _au->addCategory( kSync    , "Sync Sel"                );
  _au->addCategory( kDenEls  , "Denominator Electrons"   );
  _au->addCategory( kNumEls  , "Numerator Electrons"     );
  _au->addCategory( kVetEls  , "Veto Electrons"          );
  _au->addCategory( kDenMus  , "Denominator Muons"       );
  _au->addCategory( kNumMus  , "Numerator Muons"         );
  _au->addCategory( kVetMus  , "Veto Muons"              );
  _au->addCategory( kGoodJets, "Good Jets"               );

  //SusyModule
  _susyMod = new SusyModule(_vc, _dbm);

  //Databases
  _dbm -> loadDb   ("XS"         , "XSectionsSpring15.db");  
  _dbm -> loadDb   ("KF"         , "kFactorsSpring15.db" );  

  // BTAGSF: need these libarries
  //_dbm -> loadDb   ("BTagEffUSDG", "GC_BTagEffs.root"    , "h2_BTaggingEff_csv_med_Eff_udsg");
  //_dbm -> loadDb   ("BTagEffCB"  , "GC_BTagEffs.root"    , "h2_BTaggingEff_csv_med_Eff_b"   );
  //_dbm -> loadDbCSV("BTagSF"     , "CSVv2_50ns.csv"      , ','                              );


  //input Variables
  //_lumi   = getCfgVarF("LUMINOSITY");
  _norm   = getCfgVarS("NORMALIZATION");
  _ewkSub = getCfgVarS("EWKSUB"); // all, el, mu

  //default Variables
  _doEwkSubCERN = false;
  _doEwkSubETH  = false;

}


//____________________________________________________________________________
void FakeRatio::run(){

  _TR_lines.clear();
  _exts    .clear();
  _trws    .clear();

  _denEls    .clear();
  _denLeps   .clear();
  _denMus    .clear();
  _numEls    .clear();
  _numLeps   .clear();
  _numMus    .clear();
  _vetEls    .clear();
  _vetLeps   .clear();
  _vetMus    .clear();

  _bJets        .clear();
  _goodJets     .clear();

  _bJetsIdx     .clear();
  _denElsIdx    .clear();
  _denLepsIdx   .clear();
  _denMusIdx    .clear();
  _goodJetsIdx  .clear();
  _isoLepsIdx   .clear();
  _numElsIdx    .clear();
  _numLepsIdx   .clear();
  _numMusIdx    .clear();

  counter("denominator");
  counter("denominator", kEwkSel);
  counter("denominator", kUcsSel);
  counter("denominator", kQcdSel);
  counter("all events" , kSync  );

  // trigger selection for SYNC
  //if(_vc->get("HLT_FR_Mu17") != 1) return;
  //if(_vc->get("HLT_FR_Mu8") != 1) return;
  //counter("trigger", kSync);
  //_met = Candidate::create(_vc->get(_mets + "_pt"), _vc->get(_mets + "_phi") );


  // prepare event selection
  collectKinematicObjects();

  // BTAGSF: this is how you use it!
  //cout << _susyMod -> bTagSF("BTagEffCB", "BTagEffUSDG", "BTagSF", _goodJets, _goodJetsIdx, _bJets, _bJetsIdx, 0) << endl; 

  // skimming
  //if(!skimSelection()) return;
  //fillSkimTree();
  //return;

  // base Selection
  if(!baseSelection()) return;

  // qcd-enriched measurement region (for CERN method)
  if(qcdSelection()){
    fillQcdEventPlots   ();
    fillQcdFakeRatioMaps();
  }

  // ewk-enriched measurement region selection (for ETH method)
  if(ewkSelection()) {
    fillEwkEventPlots ();
    fillEwkLeptonPlots();
  }

  // UCSX ewk-enriched measurement region selection (for UCSX method)
  if(ucsxEwkSelection()) {
    fillUcsxEwkEventPlots ();
    fillUcsxEwkLeptonPlots();
  }

  // measurement region selection
  if(mrSelection()){
    fillEventPlots   ();
    fillFakeRatioMaps();
    fillJetPlots     ();
    fillLeptonPlots  ();
  }
	
}



/*****************************************************************************
******************************************************************************
** DEFINE INPUTS AND OUTPUTS                                                **
******************************************************************************
*****************************************************************************/


//____________________________________________________________________________
void FakeRatio::defineOutput(){

  /*
    defines and reserves all output that is produced in this class
    parameters: none
    return: none
  */ 


  // leptons
  string MR_els[3]  = {"MR_DenEl", "MR_NumEl", "MR_RatEl"};
  string MR_leps[4] = {"MR_DenEl", "MR_NumEl", "MR_DenMu", "MR_NumMu"};
  string MR_mus[3]  = {"MR_DenMu", "MR_NumMu", "MR_RatMu"};

  string ER_leps[4] = {"ER_DenEl", "ER_NumEl", "ER_DenMu", "ER_NumMu"};
  string UR_leps[4] = {"UR_DenEl", "UR_NumEl", "UR_DenMu", "UR_NumMu"};

  string QR_els[6]  = {"QR_small_DenEl", "QR_small_NumEl", "QR_small_RatEl", "QR_large_DenEl", "QR_large_NumEl", "QR_large_RatEl"};
  string QR_leps[4] = {"QR_DenEl", "QR_NumEl", "QR_DenMu", "QR_NumMu"};
  string QR_mus[6]  = {"QR_small_DenMu", "QR_small_NumMu", "QR_small_RatMu", "QR_large_DenMu", "QR_large_NumMu", "QR_large_RatMu"};

  vector<string> vMR_els  = Tools::toVector(MR_els);
  vector<string> vMR_leps = Tools::toVector(MR_leps);
  vector<string> vMR_mus  = Tools::toVector(MR_mus);

  vector<string> vER_leps = Tools::toVector(ER_leps);
  vector<string> vUR_leps = Tools::toVector(UR_leps);

  vector<string> vQR_els  = Tools::toVector(QR_els);
  vector<string> vQR_leps = Tools::toVector(QR_leps);
  vector<string> vQR_mus  = Tools::toVector(QR_mus);

  // Setting Measurement Region
  setMeasurementRegion();

  // Special feature of this class: it produces FR maps for 'data', 'datacorr', 'ewk', 'qcd'
  _hm -> addDataset("data");
  _hm -> addDataset("datacorrETH");
  _hm -> addDataset("datacorrCERN");
  _hm -> addDataset("datacorrUCSX");
  _hm -> addDataset("ewk");
  _hm -> addDataset("ewk_dy");
  _hm -> addDataset("ewk_tt");
  _hm -> addDataset("ewk_wj");
  _hm -> addDataset("qcd");
  _hm -> addDataset("qcd_bc");
  _hm -> addDataset("qcd_em");
  _hm -> addDataset("qcd_mu15");
  _hm -> addDataset("qcd_mu5");
  _idx_data         = _numDS;
  _idx_datacorrETH  = _numDS+1;
  _idx_datacorrCERN = _numDS+2;
  _idx_datacorrUCSX = _numDS+3;
  _idx_ewk          = _numDS+4;
  _idx_ewk_dy       = _numDS+5;
  _idx_ewk_tt       = _numDS+6;
  _idx_ewk_wj       = _numDS+7;
  _idx_qcd          = _numDS+8;
  _idx_qcd_bc       = _numDS+9; 
  _idx_qcd_em       = _numDS+10; 
  _idx_qcd_mu15     = _numDS+11; 
  _idx_qcd_mu5      = _numDS+12;
  int idxs[13] = {_idx_data, _idx_datacorrETH, _idx_datacorrCERN, _idx_datacorrUCSX, _idx_ewk, _idx_ewk_dy, _idx_ewk_tt, _idx_ewk_wj, _idx_qcd, _idx_qcd_bc, _idx_qcd_em, _idx_qcd_mu15, _idx_qcd_mu5};
  _idxs  = Tools::toVector(idxs);
  _idxsmc.push_back(_idx_ewk);
  _idxsmc.push_back(_idx_qcd);


  // Electroweak-Enriched Region
  registerVariable("ER_HT"              , 1000,  0.0, 1000.0, "H_T [GeV]"                            ); 
  registerVariable("ER_MET"             , 1000,  0.0, 1000.0, "#slash{E}_T [GeV]"                    );
  
  //registerLepPlots(vER_leps, "DXY"      , 5000,  0.0,   10.0, "#||{dxy}(lep) [cm]"                   );
  //registerLepPlots(vER_leps, "DZ"       , 5000,  0.0,   10.0, "#||{dz}(lep) [cm]"                    );
  //registerLepPlots(vER_leps, "SIP"      , 1000,  0.0,   10.0, "SIP3d(lep)"                           );
  //registerLepPlots(vER_leps, "Eta"      ,  250,  0.0,    2.5, "#||{#eta(lep)}"                       );
  //registerLepPlots(vER_leps, "MiniIso"  ,   50,  0.0,    1.0, "PF MiniIso (lep)"                     );
  registerLepPlots(vER_leps, "MT"       ,   20,  0.0,  200.0, "M_T(lep, MET) [GeV]"                  );
  registerLepPlots(vER_leps, "MT0"      ,   20,  0.0,  200.0, "M_T(lep, MET) [GeV]"                  );
  registerLepPlots(vER_leps, "MT1"      ,   20,  0.0,  200.0, "M_T(lep, MET) [GeV]"                  );
  //registerLepPlots(vER_leps, "Pt"       , 1000,  0.0, 1000.0, "P_T(lep) [GeV]"                       );

  // UCSX Electroweak-Enriched Region
  registerVariable("UR_HT"              , 1000,  0.0, 1000.0, "H_T [GeV]"                            ); 
  registerVariable("UR_MET"             , 1000,  0.0, 1000.0, "#slash{E}_T [GeV]"                    );
  registerLepPlots(vUR_leps, "MT"       ,   20,  0.0,  200.0, "M_T(lep, MET) [GeV]"                  );
  registerLepPlots(vUR_leps, "MT0"      ,   20,  0.0,  200.0, "M_T(lep, MET) [GeV]"                  );
  registerLepPlots(vUR_leps, "MT1"      ,   20,  0.0,  200.0, "M_T(lep, MET) [GeV]"                  );

  // Electroweak-Enriched Region
  registerVariable("QR_HT"              , 1000,  0.0, 1000.0, "H_T [GeV]"                            ); 
  registerVariable("QR_MET"             , 1000,  0.0, 1000.0, "#slash{E}_T [GeV]"                    );
  registerVariable("QR_MET_small"       ,    1,  0.0,   20.0, "#slash{E}_T [GeV]"                    );
  registerVariable("QR_MET_large"       ,    1, 45.0,   80.0, "#slash{E}_T [GeV]"                    );

  //registerLepPlots(vQR_leps, "DXY"      , 5000,  0.0,   10.0, "#||{dxy}(lep) [cm]"                   );
  //registerLepPlots(vQR_leps, "DZ"       , 5000,  0.0,   10.0, "#||{dz}(lep) [cm]"                    );
  //registerLepPlots(vQR_leps, "SIP"      , 1000,  0.0,   10.0, "SIP3d(lep)"                           );
  //registerLepPlots(vQR_leps, "Eta"      ,  250,  0.0,    2.5, "#||{#eta(lep)}"                       );
  //registerLepPlots(vQR_leps, "MiniIso"  ,   50,  0.0,    1.0, "PF MiniIso (lep)"                     );
  //registerLepPlots(vQR_leps, "MT"       ,   20,  0.0,  200.0, "M_T(lep, MET) [GeV]"                  );
  //registerLepPlots(vQR_leps, "MT0"      ,   20,  0.0,  200.0, "M_T(lep, MET) [GeV]"                  );
  //registerLepPlots(vQR_leps, "Pt"       , 1000,  0.0, 1000.0, "P_T(lep) [GeV]"                       );

  // fake ratio maps in both MET bins 
  registerLepPlots(vQR_els, "MapPt"     , _vFR_bins_pt_el .size()-1, _vFR_bins_pt_el , \
                                          _vFR_bins_eta_el.size()-1, _vFR_bins_eta_el, "P_T(e) [GeV]"                 , "#||{#eta}(e)"  ); 
  registerLepPlots(vQR_els, "MapPtJet"  , _vFR_bins_pt_el .size()-1, _vFR_bins_pt_el , \
                                          _vFR_bins_eta_el.size()-1, _vFR_bins_eta_el, "P_T(jet closest to e) [GeV]"  , "#||{#eta}(e)"  ); 
  registerLepPlots(vQR_els, "MapPtCorr" , _vFR_bins_pt_el .size()-1, _vFR_bins_pt_el , \
                                          _vFR_bins_eta_el.size()-1, _vFR_bins_eta_el, "cone corr. P_T(e) [GeV]"      , "#||{#eta}(e)"  ); 
  registerLepPlots(vQR_mus, "MapPt"     , _vFR_bins_pt_mu .size()-1, _vFR_bins_pt_mu , \
                                          _vFR_bins_eta_mu.size()-1, _vFR_bins_eta_mu, "P_T(#mu) [GeV]"               , "#||{#eta}(#mu)"); 
  registerLepPlots(vQR_mus, "MapPtJet"  , _vFR_bins_pt_mu .size()-1, _vFR_bins_pt_mu , \
                                          _vFR_bins_eta_mu.size()-1, _vFR_bins_eta_mu, "P_T(jet closest to #mu) [GeV]", "#||{#eta}(#mu)"); 
  registerLepPlots(vQR_mus, "MapPtCorr" , _vFR_bins_pt_mu .size()-1, _vFR_bins_pt_mu , \
                                          _vFR_bins_eta_mu.size()-1, _vFR_bins_eta_mu, "cone corr. P_T(#mu) [GeV]"    , "#||{#eta}(#mu)"); 
  // Measurement Region
  registerVariable("MR_HT"                , 1000,  0.0, 1000.0, "H_T [GeV]"                            ); 
  registerVariable("MR_MET"               , 1000,  0.0, 1000.0, "#slash{E}_T [GeV]"                    );
  registerVariable("MR_NumJets"           ,   20,  0.0,   20.0, "jet multiplicity"                     );
  registerVariable("MR_NumBJets"          ,   20,  0.0,   20.0, "b-jet multiplicity"                   );
  registerVariable("MR_NumDenEls"         ,   20,  0.0,   20.0, "denominator electron multiplicity"    );
  registerVariable("MR_NumDenLeps"        ,   20,  0.0,   20.0, "denominator lepton multiplicity"      );
  registerVariable("MR_NumDenMus"         ,   20,  0.0,   20.0, "denominator muon multiplicity"        );
  registerVariable("MR_NumNumEls"         ,   20,  0.0,   20.0, "numerator electron multiplicity"      );
  registerVariable("MR_NumNumLeps"        ,   20,  0.0,   20.0, "numerator lepton multiplicity"        );
  registerVariable("MR_NumNumMus"         ,   20,  0.0,   20.0, "numerator muon multiplicity"          );
  registerVariable("MR_NumVrtx"           ,   40,  0.0,   40.0, "vertex multiplicity"                  );
  registerVariable("MR_JetCSVBTag"        ,   50,  0.0,    1.0, "jet CSV B-Tag"                        );
  registerVariable("MR_JetPt"             , 1000,  0.0, 1000.0, "P_T(jet) [GeV]"                       );

  registerLepPlots(vMR_leps, "DXY"        , 5000,  0.0,   10.0, "#||{dxy}(lep) [cm]"                   );
  registerLepPlots(vMR_leps, "DZ"         , 5000,  0.0,   10.0, "#||{dz}(lep) [cm]"                    );
  registerLepPlots(vMR_leps, "SIP"        , 1000,  0.0,   10.0, "SIP3d(lep)"                           );
  registerLepPlots(vMR_leps, "Eta"        ,  250,  0.0,    2.5, "#||{#eta(lep)}"                       );
  registerLepPlots(vMR_leps, "RelIso"     ,   50,  0.0,    1.0, "PF RelIso (lep)"                      );
  registerLepPlots(vMR_leps, "MiniIso"    ,   50,  0.0,    1.0, "PF MiniIso (lep)"                     );
  registerLepPlots(vMR_leps, "MT"         , 1000,  0.0, 1000.0, "M_T(lep, MET) [GeV]"                  );
  registerLepPlots(vMR_leps, "Pt"         , 1000,  0.0, 1000.0, "P_T(lep) [GeV]"                       );
  registerLepPlots(vMR_leps, "PtJet"      , 1000,  0.0, 1000.0, "P_T(jet closest to lep) [GeV]"        );
  registerLepPlots(vMR_leps, "PtCorr"     , 1000,  0.0, 1000.0, "cone corr. P_T(lep) [GeV]"            );
  registerLepPlots(vMR_leps, "PtRel"      , 1000,  0.0, 1000.0, "PtRel(lep)"                           );

  // fake ratio projections
  registerLepPlots(vMR_els , "EtaFR"      , _vFR_bins_eta_el.size()-1, _vFR_bins_eta_el, "#||{#eta(e)}"            );
  registerLepPlots(vMR_mus , "EtaFR"      , _vFR_bins_eta_mu.size()-1, _vFR_bins_eta_mu, "#||{#eta(#mu)}"          );
  registerLepPlots(vMR_els , "PtCorrFR"   , _vFR_bins_pt_el .size()-1, _vFR_bins_pt_el , "cone corr. P_T(e) [GeV]" );
  registerLepPlots(vMR_mus , "PtCorrFR"   , _vFR_bins_pt_mu .size()-1, _vFR_bins_pt_mu , "cone corr. P_T(e) [GeV]" );

  registerLepPlots(vMR_els , "EtaFRHI"    , _vFR_bins_eta_el.size()-1, _vFR_bins_eta_el, "#||{#eta(e)}"            );
  registerLepPlots(vMR_mus , "EtaFRHI"    , _vFR_bins_eta_mu.size()-1, _vFR_bins_eta_mu, "#||{#eta(#mu)}"          );
  registerLepPlots(vMR_els , "PtCorrFRHI" , _vFR_bins_pt_el .size()-1, _vFR_bins_pt_el , "cone corr. P_T(e) [GeV]" );
  registerLepPlots(vMR_mus , "PtCorrFRHI" , _vFR_bins_pt_mu .size()-1, _vFR_bins_pt_mu , "cone corr. P_T(e) [GeV]" );

  registerLepPlots(vMR_els , "EtaFRLO"    , _vFR_bins_eta_el.size()-1, _vFR_bins_eta_el, "#||{#eta(e)}"            );
  registerLepPlots(vMR_mus , "EtaFRLO"    , _vFR_bins_eta_mu.size()-1, _vFR_bins_eta_mu, "#||{#eta(#mu)}"          );
  registerLepPlots(vMR_els , "PtCorrFRLO" , _vFR_bins_pt_el .size()-1, _vFR_bins_pt_el , "cone corr. P_T(e) [GeV]" );
  registerLepPlots(vMR_mus , "PtCorrFRLO" , _vFR_bins_pt_mu .size()-1, _vFR_bins_pt_mu , "cone corr. P_T(e) [GeV]" );

  // fake ratio maps electrons 
  registerLepPlots(vMR_els, "MapPt"       , _vFR_bins_pt_el .size()-1, _vFR_bins_pt_el , \
                                            _vFR_bins_eta_el.size()-1, _vFR_bins_eta_el, "P_T(e) [GeV]"                 , "#||{#eta}(e)"); 
  registerLepPlots(vMR_els, "MapPtJet"    , _vFR_bins_pt_el .size()-1, _vFR_bins_pt_el , \
                                            _vFR_bins_eta_el.size()-1, _vFR_bins_eta_el, "P_T(jet closest to e) [GeV]"  , "#||{#eta}(e)"); 
  registerLepPlots(vMR_els, "MapPtCorr"   , _vFR_bins_pt_el .size()-1, _vFR_bins_pt_el , \
                                            _vFR_bins_eta_el.size()-1, _vFR_bins_eta_el, "cone corr. P_T(e) [GeV]"      , "#||{#eta}(e)"); 
  registerLepPlots(vMR_els, "MapPtCorrHI" , _vFR_bins_pt_el .size()-1, _vFR_bins_pt_el , \
                                            _vFR_bins_eta_el.size()-1, _vFR_bins_eta_el, "cone corr. P_T(e) [GeV]"      , "#||{#eta}(e)"); 
  registerLepPlots(vMR_els, "MapPtCorrLO" , _vFR_bins_pt_el .size()-1, _vFR_bins_pt_el , \
                                            _vFR_bins_eta_el.size()-1, _vFR_bins_eta_el, "cone corr. P_T(e) [GeV]"      , "#||{#eta}(e)"); 

  // fake ratio maps muons
  registerLepPlots(vMR_mus, "MapPt"       , _vFR_bins_pt_mu .size()-1, _vFR_bins_pt_mu , \
                                            _vFR_bins_eta_mu.size()-1, _vFR_bins_eta_mu, "P_T(#mu) [GeV]"               , "#||{#eta}(#mu)"); 
  registerLepPlots(vMR_mus, "MapPtJet"    , _vFR_bins_pt_mu .size()-1, _vFR_bins_pt_mu , \
                                            _vFR_bins_eta_mu.size()-1, _vFR_bins_eta_mu, "P_T(jet closest to #mu) [GeV]", "#||{#eta}(#mu)"); 
  registerLepPlots(vMR_mus, "MapPtCorr"   , _vFR_bins_pt_mu .size()-1, _vFR_bins_pt_mu, \
                                            _vFR_bins_eta_mu.size()-1, _vFR_bins_eta_mu, "cone corr. P_T(#mu) [GeV]"    , "#||{#eta}(#mu)"); 
  registerLepPlots(vMR_mus, "MapPtCorrHI" , _vFR_bins_pt_mu .size()-1, _vFR_bins_pt_mu, \
                                            _vFR_bins_eta_mu.size()-1, _vFR_bins_eta_mu, "cone corr. P_T(#mu) [GeV]"    , "#||{#eta}(#mu)"); 
  registerLepPlots(vMR_mus, "MapPtCorrLO" , _vFR_bins_pt_mu .size()-1, _vFR_bins_pt_mu, \
                                            _vFR_bins_eta_mu.size()-1, _vFR_bins_eta_mu, "cone corr. P_T(#mu) [GeV]"    , "#||{#eta}(#mu)"); 
 
  //registerLepPlots(vMR_els, "MapTrTest" , _vFR_bins_pt_el .size()-1, _vFR_bins_pt_el , \
  //                                        _vFR_bins_eta_el.size()-1, _vFR_bins_eta_el, "P_T(e) [GeV]"                 , "#||{#eta}(e)"); 
  //registerLepPlots(vMR_mus, "MapTrTest" , _vFR_bins_pt_mu .size()-1, _vFR_bins_pt_mu , \
  //                                        _vFR_bins_eta_mu.size()-1, _vFR_bins_eta_mu, "P_T(#mu) [GeV]"               , "#||{#eta}(#mu)"); 
}


//____________________________________________________________________________
void FakeRatio::divideFRMap(string postpend){

  for(unsigned int i = 0; i < _numDS + _idxs.size(); ++i){
    if(i == _idx_datacorrCERN) continue;
    TH2F * denom = (TH2F*) _hm -> getHisto("MR_Den" + postpend, i);
    TH2F * num   = (TH2F*) _hm -> getHisto("MR_Rat" + postpend, i);
    num -> Divide(denom);
  }
 
}


//____________________________________________________________________________
void FakeRatio::divideFRMaps(){

  vector<string> exts;
  exts.push_back("non");
  exts.push_back("iso");

  for(unsigned int i = 0; i < _vTR_lines_el_non[0].size(); ++i) exts.push_back("non" + Tools::intToString(i));
  for(unsigned int i = 0; i < _vTR_lines_mu_non[0].size(); ++i) exts.push_back("non" + Tools::intToString(i));
  for(unsigned int i = 0; i < _vTR_lines_el_iso[0].size(); ++i) exts.push_back("iso" + Tools::intToString(i));
  for(unsigned int i = 0; i < _vTR_lines_mu_iso[0].size(); ++i) exts.push_back("iso" + Tools::intToString(i));

  for(unsigned int i = 0; i < exts.size(); ++i){
    divideFRMap("ElMapPt_"     + exts[i]);
    divideFRMap("ElMapPtJet_"  + exts[i]);
    divideFRMap("ElMapPtCorr_" + exts[i]);
    divideFRMap("MuMapPt_"     + exts[i]);
    divideFRMap("MuMapPtJet_"  + exts[i]);
    divideFRMap("MuMapPtCorr_" + exts[i]);
  }
} 


//____________________________________________________________________________
void FakeRatio::doEwkSub(){


  bool foundData = false;
  bool foundEwk  = false;
  bool foundQcd  = false;

  for(int i = 0; i < _numDS; ++i){
    
    string sname = _datasets[i]->getName();

    if     (sname.find("data") != std::string::npos) foundData = true;
    else if(sname.find("ewk")  != std::string::npos) foundEwk  = true;
    else if(sname.find("qcd")  != std::string::npos) foundQcd  = true;

  }

  if(foundData && foundEwk && foundQcd && (_ewkSub == "el" || _ewkSub == "mu")){ 
    _doEwkSubETH  = true;
    _doEwkSubUCSX = true;
  }

  if(foundData && foundEwk && (_ewkSub == "el" || _ewkSub == "mu")) 
    _doEwkSubCERN = true;

}


//  ____________________________________________________________________________
vector<float> FakeRatio::doubleFit(string ext, TH1* h_data, TH1* h_ewk, TH1* h_qcd, float hmin, float hmax){

  TH1* ch_data = (TH1*) h_data -> Clone();
  TH1* ch_ewk  = (TH1*) h_ewk  -> Clone();
  TH1* ch_qcd  = (TH1*) h_qcd  -> Clone();

  if(hmin == 0) hmin = h_data -> GetXaxis() -> GetXmin();
  if(hmax == 0) hmax = h_data -> GetXaxis() -> GetXmax();

  RooRealVar x("x", "x", hmin, hmax);

  string rdhd = "rdh_data" + ext;
  string rdhe = "rdh_ewk"  + ext;
  string rdhq = "rdh_qcd"  + ext;
  RooDataHist rdh_data(rdhd.c_str(), rdhd.c_str(), x, ch_data  );
  RooDataHist rdh_ewk (rdhe.c_str(), rdhe.c_str(), x, ch_ewk   );
  RooDataHist rdh_qcd (rdhq.c_str(), rdhq.c_str(), x, ch_qcd   );
              
  string pdfe = "pdf_ewk"  + ext;
  string pdfq = "pdf_qcd"  + ext;
  RooHistPdf  pdf_ewk (pdfe.c_str(), pdfe.c_str(), x, rdh_ewk );
  RooHistPdf  pdf_qcd (pdfq.c_str(), pdfq.c_str(), x, rdh_qcd );

  float int_data = h_data -> Integral(); //h_data -> GetXaxis() -> FindBin(hmin), h_data -> GetXaxis() -> FindBin(hmax));
  float int_ewk  = h_ewk  -> Integral(); //h_ewk  -> GetXaxis() -> FindBin(hmin), h_ewk  -> GetXaxis() -> FindBin(hmax));
  float int_qcd  = h_qcd  -> Integral(); //h_qcd  -> GetXaxis() -> FindBin(hmin), h_qcd  -> GetXaxis() -> FindBin(hmax));

  string rrve = "rrv_ewk" + ext;
  string rrvq = "rrv_qcd" + ext; 
  //RooRealVar  rrv_ewk ("rrv_ewk" + ext, "rrv_ewk" , int_ewk, 1400, 14695 );
  //RooRealVar  rrv_qcd ("rrv_qcd" + ext, "rrv_qcd" , int_qcd, 1400, 14695 );
  RooRealVar  rrv_ewk (rrve.c_str(), rrve.c_str(), int_ewk, int_data * 0.1, int_data );
  RooRealVar  rrv_qcd (rrvq.c_str(), rrvq.c_str(), int_qcd, int_data * 0.1, int_data );

  RooArgList pdfs (pdf_ewk, pdf_qcd); 
  RooArgList coeff(rrv_ewk, rrv_qcd); 

  string tot = "totPdf" + ext;
  RooAddPdf totPdf(tot.c_str(), tot.c_str(), pdfs, coeff);
  totPdf.fitTo(rdh_data, RooFit::SumW2Error(false)); //, RooFit::Extended(), RooFit::PrintLevel(-1));

  vector<float> central;
  central.push_back(rrv_ewk.getVal() / int_ewk);
  central.push_back(rrv_qcd.getVal() / int_qcd);

  return central;

}  


//____________________________________________________________________________
vector<float> FakeRatio::getScalesETH(string obs, float lumi){

  TH1 * h_data   = _hm -> getHisto(obs, _idx_datacorrETH);   
  TH1 * h_ewk    = _hm -> getHisto(obs, _idx_ewk        );   
  TH1 * h_qcd    = _hm -> getHisto(obs, _idx_qcd        );   

  vector<float> scales;
  scales.push_back(1.);
  scales.push_back(1.);

  if(lumi                   == 0) return scales;
  if(h_data -> GetEntries() == 0) return scales;
  if(h_data -> GetEntries() < 10) return scales;


  // first fit EWK and QCD together to data in range [0, infinity]
  string ext = obs.substr(obs.find_last_of("_"));
  vector<float> sc_first = doubleFit(ext, h_data, h_ewk, h_qcd);

  if(sc_first[0] >= 0 && sc_first[1] >= 0){

  // save plots after first fit
  string nobs  = obs.substr(0, obs.find_last_of("_")) + "1" + ext;
  TH1 * h_ewk1 = (TH1*) _hm -> getHisto(nobs, _idx_ewk);
  TH1 * h_qcd1 = (TH1*) _hm -> getHisto(nobs, _idx_qcd);
  h_ewk1 -> Scale(sc_first[0]);
  h_qcd1 -> Scale(sc_first[1]);

  // fix QCD and subtract from data
  TH1 * h_dataqcdsub = (TH1*) h_data -> Clone(); 
  h_dataqcdsub -> Add(h_qcd1, -1);

  // fit EWK to QCD-subtracted-data in range [50,120]
  scales[0] = sc_first[0] * singleFit(ext, h_dataqcdsub, h_ewk1, 50, 120); // ewk scale
  scales[1] = sc_first[1];                                            // qcd scale
  }

cout << "ETH EWK fit performed for observable " << obs << endl;
cout << "first : " << scales[0] << "; " << scales[1] << endl;
cout << "second: " << (scales[0] / sc_first[0]) << endl;

  return scales;

}


//____________________________________________________________________________
vector<float> FakeRatio::getScalesUCSX(string obs, float lumi){

  TH1 * h_data   = _hm -> getHisto(obs, _idx_datacorrUCSX);   
  TH1 * h_ewk    = _hm -> getHisto(obs, _idx_ewk         );   

  float sc[3] = {1.0, 1.0, 1.0};
  vector<float> scales = Tools::toVector(sc);

  if(lumi                   == 0) return scales;
  if(h_data -> GetEntries() == 0) return scales;
  if(h_data -> GetEntries() < 10) return scales;

  // fit EWK to QCD-subtracted-data in range [70,120]
  string ext = obs.substr(obs.find_last_of("_"));
  singleFit(scales, ext, h_data, h_ewk, 70, 120); // ewk scale

cout << "UCSX EWK fit performed for observable " << obs << endl;
cout << "scale : " << scales[0] << endl;

  return scales;

}


//____________________________________________________________________________
vector<float> FakeRatio::getScalesUCSXlite(string obs, float lumi){

  TH1 * h_data   = _hm -> getHisto(obs, _idx_datacorrUCSX);   
  TH1 * h_ewk    = _hm -> getHisto(obs, _idx_ewk         );   

  float sc[3] = {1.0, 1.0, 1.0};
  vector<float> scales = Tools::toVector(sc);

  if(lumi                   == 0) return scales;
  if(h_data -> GetEntries() == 0) return scales;
  if(h_data -> GetEntries() < 10) return scales;

  // fit EWK to QCD-subtracted-data in range [70,120]
  scales[0] = float(h_data -> Integral(h_data -> GetXaxis() -> FindBin(70), h_data -> GetXaxis() -> FindBin(120))) / 
              float(h_ewk  -> Integral(h_ewk  -> GetXaxis() -> FindBin(70), h_ewk  -> GetXaxis() -> FindBin(120)));

  float dev = std::abs(scales[0] - 1.0);
  scales[1] = scales[0] + dev;
  scales[2] = scales[0] - dev;

cout << "UCSX lite EWK fit performed for observable " << obs << endl;
cout << "scale : " << scales[0] << endl;

  return scales;

}


//____________________________________________________________________________
void FakeRatio::modifySkimming(){
  // if adding variables in the skimming tree is needed...
  addSkimBranch("vtxWeight", &_vtxWeight); 

}


//____________________________________________________________________________
void FakeRatio::modifyWeight() {
  /*
    modifies the event weight for every entry in the tree
    parameters: none
    return: none
  */ 
	
  //_weight = (i->second)->Getweight();

  //if(_sampleName.find("data") == std::string::npos && _vc->get("puWeight") > 0)
  //  _weight *= _vc->get("puWeight");
  //else
  //  _weight = 1.;

  _isData = true;
  if(_sampleName.find("data")==std::string::npos && _sampleName.find("runs")==std::string::npos) {

    _vtxWeight = _vc->get("vtxWeight");

    string sname = _sampleName;
    if(sname.find("ewk") != std::string::npos) sname = sname.erase(sname.find("ewk"), 3); 
    if(sname.find("qcd") != std::string::npos) sname = sname.erase(sname.find("qcd"), 3); 

    _weight *= _vc->get("vtxWeight")*_vc->get("genWeight");

    double nProc=getCurrentDS()->getSumProcWgts(); 

    if(nProc==-1) nProc=getCurrentDS()->getNProcEvents();

    // keep this for /pnfs/psi.ch/cms/trivcat/store/user/cheidegg/sea/11/2015-10-18-13-19-00/
    if(sname.find("QCD_Pt20to30_Mu5")    != std::string::npos) nProc = 1271383. / 1629978.  * nProc;
    if(sname.find("QCD_Pt_20to30_bcToE") != std::string::npos) nProc = 950000.  / 1496051.  * nProc;
    if(sname.find("TTJets")              != std::string::npos) nProc = 8405382. / 16164196. * 7.96328e+10;

    double w=_dbm->getDBValue("XS", sname) * _dbm->getDBValue("KF", sname)/nProc * 1;//last number is lumi
    _weight *=w;
    _isData=false;
  }

}


//  ____________________________________________________________________________
vector<float> FakeRatio::prepareHists(TH1* h_data, TH1* h_ewk, TH1* h_qcd){

  vector<float> scales;
  scales.push_back(1.);
  scales.push_back(1.);

  if(h_data -> Integral() < h_ewk -> Integral())
    scales[0] = floor(h_data->Integral() / h_ewk->Integral() * 1000) / 1000;

  if(h_data -> Integral() < h_qcd -> Integral())
    scales[1] = floor(h_data->Integral() / h_qcd->Integral() * 1000) / 1000;

  return scales;

}


//____________________________________________________________________________
void FakeRatio::registerLepPlots(vector<string> leps, string var, int nbins, float bmin, float bmax, string axis){

  for(unsigned int i = 0; i < leps.size(); ++i)
    registerVariable(leps[i] + var, nbins, bmin, bmax, axis);
    //_hm->addVariable(leps[i] + var, nbins, bmin, bmax, axis);

} 


//____________________________________________________________________________
void FakeRatio::registerLepPlots(vector<string> leps, string var, int nxbins, vector<float> xbins, string axis){

  for(unsigned int i = 0; i < leps.size(); ++i)
    registerVariable(leps[i] + var, nxbins, xbins, axis);
    //_hm->addVariable(leps[i] + var, nxbins, xbins, nybins, ybins, xaxis, yaxis);

}


//____________________________________________________________________________
void FakeRatio::registerLepPlots(vector<string> leps, string var, int nxbins, vector<float> xbins, int nybins, vector<float> ybins, string xaxis, string yaxis){

  for(unsigned int i = 0; i < leps.size(); ++i)
    registerVariable(leps[i] + var, nxbins, xbins, nybins, ybins, xaxis, yaxis);
    //_hm->addVariable(leps[i] + var, nxbins, xbins, nybins, ybins, xaxis, yaxis);

}


//  ____________________________________________________________________________
void FakeRatio::registerTriggerVars(){

  for(unsigned int i = 0; i < _vTR_lines_el_non[0].size(); ++i) {
    _vc->registerVar(_vTR_lines_el_non[0][i]); 
    _vc->registerVar(_vTR_psbh_el_non[i]);
    _vc->registerVar(_vTR_psbl_el_non[i]);
  }
  for(unsigned int i = 0; i < _vTR_lines_el_iso[0].size(); ++i) {
    _vc->registerVar(_vTR_lines_el_iso[0][i]); 
    _vc->registerVar(_vTR_psbh_el_iso[i]);
    _vc->registerVar(_vTR_psbl_el_iso[i]);
  }
  for(unsigned int i = 0; i < _vTR_lines_mu_non[0].size(); ++i) {
    _vc->registerVar(_vTR_lines_mu_non[0][i]); 
    _vc->registerVar(_vTR_psbh_mu_non[i]);
    _vc->registerVar(_vTR_psbl_mu_non[i]);
  }
  for(unsigned int i = 0; i < _vTR_lines_mu_iso[0].size(); ++i) {
    _vc->registerVar(_vTR_lines_mu_iso[0][i]); 
    _vc->registerVar(_vTR_psbh_mu_iso[i]);
    _vc->registerVar(_vTR_psbl_mu_iso[i]);
  }

}


//  ____________________________________________________________________________
void FakeRatio::registerVariable(string var, int nBin, float min, float max, string Xleg, bool isglb, bool prof, string type){

  _hm->addVariable(var + "_non", nBin, min, max, Xleg, isglb, prof, type);
  _hm->addVariable(var + "_iso", nBin, min, max, Xleg, isglb, prof, type);

  for(unsigned int i = 0; i < _vTR_lines_el_non[0].size() + _vTR_lines_mu_non[0].size(); ++i)
    _hm->addVariable(var + "_non" + Tools::intToString(i), nBin, min, max, Xleg, isglb, prof, type);
  for(unsigned int i = 0; i < _vTR_lines_el_iso[0].size() + _vTR_lines_mu_iso[0].size(); ++i)
    _hm->addVariable(var + "_iso" + Tools::intToString(i), nBin, min, max, Xleg, isglb, prof, type);
  
}


//  ____________________________________________________________________________
void FakeRatio::registerVariable(string var, int nBinX, float minX, float maxX, int nBinY, float minY, float maxY, string Xleg, string Yleg, bool isglb, bool prof, string type){

  _hm->addVariable(var + "_non", nBinX, minX, maxX, nBinY, minY, maxY, Xleg, Yleg, isglb, prof, type);
  _hm->addVariable(var + "_iso", nBinX, minX, maxX, nBinY, minY, maxY, Xleg, Yleg, isglb, prof, type);

  for(unsigned int i = 0; i < _vTR_lines_el_non[0].size() + _vTR_lines_mu_non[0].size(); ++i)
    _hm->addVariable(var + "_non" + Tools::intToString(i), nBinX, minX, maxX, nBinY, minY, maxY, Xleg, Yleg, isglb, prof, type);
  for(unsigned int i = 0; i < _vTR_lines_el_iso[0].size() + _vTR_lines_mu_iso[0].size(); ++i)
    _hm->addVariable(var + "_iso" + Tools::intToString(i), nBinX, minX, maxX, nBinY, minY, maxY, Xleg, Yleg, isglb, prof, type);

}


//  ____________________________________________________________________________
void FakeRatio::registerVariable(string var, int nBinX, vector<float> binsX, string Xleg, bool isglb, bool prof, string type){

  _hm->addVariable(var + "_non", nBinX, binsX, Xleg, isglb, prof, type);
  _hm->addVariable(var + "_iso", nBinX, binsX, Xleg, isglb, prof, type);

  for(unsigned int i = 0; i < _vTR_lines_el_non[0].size() + _vTR_lines_mu_non[0].size(); ++i)
    _hm->addVariable(var + "_non" + Tools::intToString(i), nBinX, binsX, Xleg, isglb, prof, type);
  for(unsigned int i = 0; i < _vTR_lines_el_iso[0].size() + _vTR_lines_mu_iso[0].size(); ++i)
    _hm->addVariable(var + "_iso" + Tools::intToString(i), nBinX, binsX, Xleg, isglb, prof, type);

}


//  ____________________________________________________________________________
void FakeRatio::registerVariable(string var, int nBinX, vector<float> binsX, int nBinY, vector<float> binsY, string Xleg, string Yleg, bool isglb, bool prof, string type){

  _hm->addVariable(var + "_non", nBinX, binsX, nBinY, binsY, Xleg, Yleg, isglb, prof, type);
  _hm->addVariable(var + "_iso", nBinX, binsX, nBinY, binsY, Xleg, Yleg, isglb, prof, type);

  for(unsigned int i = 0; i < _vTR_lines_el_non[0].size() + _vTR_lines_mu_non[0].size(); ++i)
    _hm->addVariable(var + "_non" + Tools::intToString(i), nBinX, binsX, nBinY, binsY, Xleg, Yleg, isglb, prof, type);
  for(unsigned int i = 0; i < _vTR_lines_el_iso[0].size() + _vTR_lines_mu_iso[0].size(); ++i)
    _hm->addVariable(var + "_iso" + Tools::intToString(i), nBinX, binsX, nBinY, binsY, Xleg, Yleg, isglb, prof, type);

}
 

//  ____________________________________________________________________________
float FakeRatio::singleFit(string ext, TH1* h_data, TH1* h_mc, float hmin, float hmax){

  TH1* ch_data = (TH1*) h_data -> Clone();
  TH1* ch_mc   = (TH1*) h_mc   -> Clone();

  if(hmin == 0) hmin = h_data -> GetXaxis() -> GetXmin();
  if(hmax == 0) hmax = h_data -> GetXaxis() -> GetXmax();

  RooRealVar x("x", "x", hmin, hmax);
  RooArgList rlist(x);
  RooArgSet  rset (x);

  string rdhd  = "rdhs_data" + ext;
  string rdhmc = "rdhs_mc" + ext;
  string pdfmc = "pdfs_mc" + ext;
  string rrvmc = "rrvs_mc" + ext;
  string tot   = "totPdfs" + ext;
  RooDataHist rdh_data(rdhd .c_str(), rdhd .c_str(), rlist, ch_data );
  RooDataHist rdh_mc  (rdhmc.c_str(), rdhmc.c_str(), rlist, ch_mc   );
  RooHistPdf  pdf_mc  (pdfmc.c_str(), pdfmc.c_str(), rset , rdh_mc  );

  float int_data = h_data -> Integral(h_data -> GetXaxis() -> FindBin(hmin), h_data -> GetXaxis() -> FindBin(hmax));
  float int_mc   = h_mc   -> Integral(h_mc   -> GetXaxis() -> FindBin(hmin), h_mc   -> GetXaxis() -> FindBin(hmax));

  RooRealVar  rrv_mc  (rrvmc.c_str(), rrvmc.c_str(), int_data, int_data * 0.5, int_data );

  RooArgList  pdfs    (pdf_mc); 
  RooArgList  coeff   (rrv_mc); 

  RooAddPdf   totPdf  (tot.c_str(), tot.c_str(), pdfs, coeff);
  totPdf.fitTo(rdh_data, RooFit::SumW2Error(false)); //, RooFit::Extended(), RooFit::PrintLevel(-1));

  return rrv_mc.getVal() / int_mc;

} 
 

//  ____________________________________________________________________________
void FakeRatio::singleFit(vector<float>& scales, string ext, TH1* h_data, TH1* h_mc, float hmin, float hmax){

  TH1* ch_data = (TH1*) h_data -> Clone();
  TH1* ch_mc   = (TH1*) h_mc   -> Clone();

  if(hmin == 0) hmin = h_data -> GetXaxis() -> GetXmin();
  if(hmax == 0) hmax = h_data -> GetXaxis() -> GetXmax();

  RooRealVar x("x", "x", hmin, hmax);
  RooArgList rlist(x);
  RooArgSet  rset (x);

  string rdhd  = "rdhs_data" + ext;
  string rdhmc = "rdhs_mc"   + ext;
  string pdfmc = "pdfs_mc"   + ext;
  string rrvmc = "rrvs_mc"   + ext;
  string tot   = "totPdfs"   + ext;

  RooDataHist rdh_data(rdhd .c_str(), rdhd .c_str(), rlist, ch_data );
  RooDataHist rdh_mc  (rdhmc.c_str(), rdhmc.c_str(), rlist, ch_mc   );
  RooHistPdf  pdf_mc  (pdfmc.c_str(), pdfmc.c_str(), rset , rdh_mc  );

  float int_data = h_data -> Integral(h_data -> GetXaxis() -> FindBin(hmin), h_data -> GetXaxis() -> FindBin(hmax));
  float int_mc   = h_mc   -> Integral(h_mc   -> GetXaxis() -> FindBin(hmin), h_mc   -> GetXaxis() -> FindBin(hmax));

  RooRealVar  rrv_mc  (rrvmc.c_str(), rrvmc.c_str(), int_data, int_data * 0.5, int_data );

  RooArgList  pdfs    (pdf_mc); 
  RooArgList  coeff   (rrv_mc); 

  RooAddPdf   totPdf  (tot.c_str(), tot.c_str(), pdfs, coeff);
  totPdf.fitTo(rdh_data, RooFit::SumW2Error(false), RooFit::Save(true)); //, RooFit::Extended(), RooFit::PrintLevel(-1));

  scales[0] = rrv_mc.getVal() / int_mc;

  // Giuseppe's error
  float dev = std::abs(scales[0] - 1.);
  scales[1] = scales[0] + dev;
  scales[2] = scales[0] + dev;

  // precise error
  //scales[1] = rrv_mc.getVal() / int_mc + rrv_mc.getErrorHi() / int_mc;
  //scales[2] = rrv_mc.getVal() / int_mc + rrv_mc.getErrorLo() / int_mc;

} 


//  ____________________________________________________________________________
void FakeRatio::subtractPlots(string lep, int idx, vector<float> scales, string postfix){
  //CH: only rescale ER and MR plots

  vector<string> obs = _hm -> getObservables(true);

  for(unsigned int i = 0; i < obs.size(); ++i) {

    if(obs[i].find("MR_")         == std::string::npos) continue;
    if(obs[i].find("MT0")         != std::string::npos) continue;
    if(obs[i].find("MT1")         != std::string::npos) continue;
    if(obs[i].find("HI")          != std::string::npos) continue;
    if(obs[i].find("LO")          != std::string::npos) continue;
    if(obs[i].find(lep)           == std::string::npos) continue;
    if(obs[i].find("_" + postfix) == std::string::npos) continue;

    string num = obs[i].substr(obs[i].find("_" + postfix) + 4);

    TH1 * d = _hm -> getHisto(obs[i], idx     );
    TH1 * e = _hm -> getHisto(obs[i], _idx_ewk);
    TH1 * q = _hm -> getHisto(obs[i], _idx_qcd);

    // only central value for ewk and qcd scales
    if(scales.size() == 2) {
      e -> Scale(scales[0]);
      q -> Scale(scales[1]); 
    
      d -> Add(e, -1);
    }

    // central value and errors for ewk scale only
    else if(scales.size() == 3) {

      // normal subtraction, no error propagation
      if(obs[i].find("MapPtCorr") == std::string::npos && obs[i].find("FR") == std::string::npos){
        e -> Scale(scales[0]);
        d -> Add(e, -1);
      }

      // subtraction including systematic variation of ewk fit
      else {
        // CH: if you want to have the systematic uncertainty from the ewk fit to replace
        // the statistical uncertainty of your histogram, then comment //1 and uncomment //2
        // if you want to have the up and down variations of your histograms in separate
        // HI and LO histograms, then uncomment //1 and comment //2!

        // CH: careful here: we apply the up variation of the scale factor of the electroweak (eHI)
        // to dHI, but since more of the ewk is subtracted, the fake ratio will be lower, so it is
        // the down variation, hence we fill it into the LO map!
        TH1 * dHI = _hm -> getHisto(obs[i].substr(0, obs[i].rfind("_")) + "LO_" + postfix + num, idx); // 1
        TH1 * dLO = _hm -> getHisto(obs[i].substr(0, obs[i].rfind("_")) + "HI_" + postfix + num, idx); // 1
		dHI -> Reset(); // 1
        dLO -> Reset(); // 1
		dHI -> Add(d); // 1
        dLO -> Add(d); // 1
        //TH1* dHI = (TH1*) d -> Clone(); // 2
        //TH1* dLO = (TH1*) d -> Clone(); // 2

        TH1* eHI = (TH1*) e -> Clone();
        TH1* eLO = (TH1*) e -> Clone();

        e   -> Scale(scales[0]);
        eHI -> Scale(scales[1]);
        eLO -> Scale(scales[2]);

        d   -> Add(e  , -1);
        dHI -> Add(eHI, -1);
        dLO -> Add(eLO, -1);

        //setBinErrors(d, dLO, dHI); // 2
      }
    }

    // central values and errors for ewk and qcd scales
    else if(scales.size() == 6) {
      d -> Add(e, -1);
    }
  }
}


//  ____________________________________________________________________________
void FakeRatio::setBinErrors(TH1* central, TH1* upper, TH1* lower){

  unsigned int nbins = central -> GetXaxis() -> GetNbins();
  if(central -> GetDimension() == 2) nbins *= central -> GetYaxis() -> GetNbins();

  for(unsigned int b = 1; b <= nbins; ++b) {
    float error = upper -> GetBinContent(b) - central -> GetBinContent(b);
    if(central -> GetBinContent(b) - lower -> GetBinContent(b)) 
      error = central -> GetBinContent(b) - lower -> GetBinContent(b);
    central -> SetBinError(b, error);
  }

}


//  ____________________________________________________________________________
void FakeRatio::subtractPlotsCERN(string lep, int idx, string postfix){


  int datas = _hm -> getHisto("QR_MET_small_" + postfix, _idx_data) -> GetEntries();
  int datal = _hm -> getHisto("QR_MET_large_" + postfix, _idx_data) -> GetEntries();

  int ewks  = _hm -> getHisto("QR_MET_small_" + postfix, _idx_ewk ) -> GetEntries();
  int ewkl  = _hm -> getHisto("QR_MET_large_" + postfix, _idx_ewk ) -> GetEntries();

  float rp = 0.;
  if( ewkl > 0 && datas > 0)
    rp = ( float(ewks) / float(ewkl) ) * ( float(datal) / float(datas) );
  float sp = float(1.0) / (1. - rp);

  vector<string> obs = _hm -> getObservables(true);
  vector<string> nobs;
  for(unsigned int i = 0; i < obs.size(); ++i)
    if(obs[i].find("MR_Rat" + lep + "Map") != std::string::npos && obs[i].find(postfix) != std::string::npos)
      nobs.push_back(obs[i]);

  for(unsigned int i = 0; i < nobs.size(); ++i) {

    size_t pos = nobs[i].find("Map") + 3;
    string ext = nobs[i].substr(pos, nobs[i].find("_", pos) - pos);

    TH1 * nums = _hm -> getHisto("QR_small_Num" + lep + "Map" + ext + "_" + postfix, _idx_data);
    TH1 * numl = _hm -> getHisto("QR_large_Num" + lep + "Map" + ext + "_" + postfix, _idx_data);

    TH1 * frs = (TH1*) nums -> Clone();
    TH1 * frl = (TH1*) numl -> Clone();

    frs -> Divide(_hm -> getHisto("QR_small_Den" + lep + "Map" + ext + "_" + postfix, _idx_data)); 
    frl -> Divide(_hm -> getHisto("QR_large_Den" + lep + "Map" + ext + "_" + postfix, _idx_data)); 

    TH1 * r = _hm -> getHisto(nobs[i], _idx_datacorrCERN);

    r -> Reset();
    r -> Add(frs);
    r -> Add(frl, rp * (-1));
    r -> Scale(sp);
  }

}



//  ____________________________________________________________________________
void FakeRatio::subtractPromptsCERN(){

  if(!_doEwkSubCERN) return;

  subtractPlotsCERN("El", _idx_datacorrCERN, "iso");
  subtractPlotsCERN("Mu", _idx_datacorrCERN, "iso");
  subtractPlotsCERN("El", _idx_datacorrCERN, "non");
  subtractPlotsCERN("Mu", _idx_datacorrCERN, "non");

}


//____________________________________________________________________________
void FakeRatio::subtractPromptsETH(){
  /*
    implements subtraction of prompt contamination (ETH method)
  */

  if(!_doEwkSubETH) return; 

  subtractPlots("El", _idx_datacorrETH, getScalesETH("ER_NumElMT_iso", 1.0), "iso");
  subtractPlots("Mu", _idx_datacorrETH, getScalesETH("ER_NumMuMT_iso", 1.0), "iso");
  subtractPlots("El", _idx_datacorrETH, getScalesETH("ER_NumElMT_non", 1.0), "non");
  subtractPlots("Mu", _idx_datacorrETH, getScalesETH("ER_NumMuMT_non", 1.0), "non");

}   


//____________________________________________________________________________
void FakeRatio::subtractPromptsUCSX(){
  /*
    implements subtraction of prompt contamination (UCSX method)
  */

  if(!_doEwkSubUCSX) return; 

  // only with PS reweighting !

  // CH: the good way using shape information
  //subtractPlots("El", _idx_datacorrUCSX, getScalesUCSX    ("UR_NumElMT_iso", 1.0), "iso");
  //subtractPlots("Mu", _idx_datacorrUCSX, getScalesUCSX    ("UR_NumMuMT_iso", 1.0), "iso");
  //subtractPlots("El", _idx_datacorrUCSX, getScalesUCSX    ("UR_NumElMT_non", 1.0), "non");
  //subtractPlots("Mu", _idx_datacorrUCSX, getScalesUCSX    ("UR_NumMuMT_non", 1.0), "non");

  // CH: the bad way discarding shape information, just scaling one number to the other
  subtractPlots("El", _idx_datacorrUCSX, getScalesUCSXlite("UR_NumElMT_iso", 1.0), "iso");
  subtractPlots("Mu", _idx_datacorrUCSX, getScalesUCSXlite("UR_NumMuMT_iso", 1.0), "iso");
  subtractPlots("El", _idx_datacorrUCSX, getScalesUCSXlite("UR_NumElMT_non", 1.0), "non");
  subtractPlots("Mu", _idx_datacorrUCSX, getScalesUCSXlite("UR_NumMuMT_non", 1.0), "non");

  return;

}   


//____________________________________________________________________________
void FakeRatio::sumMaps(){
  /*
    adds weighted numerators and denominators for different samples with same prefix
  */


  vector<string> obs = _hm -> getObservables(true);
  vector<float> ints_data;
  ints_data.resize(obs.size(), 0);
 
  for(int i = 0; i < _numDS; ++i){
    if(_datasets[i]->getName().find("data") != std::string::npos) {
      for(unsigned int j = 0; j < obs.size(); ++j) {
        ints_data[j] += _hm -> getHisto(obs[j], i)->Integral();
      }
    }
  } 

  for(int i = 0; i < _numDS; ++i){
    
    string sname = _datasets[i]->getName();
    bool normalize = false;

    int nevts = _datasets[i]->getNProcEvents();
	float xs = 0;
	vector<unsigned int> idxs;
    if     (sname.find("data") != std::string::npos) {
	  idxs.push_back(_idx_data        ); 
	  idxs.push_back(_idx_datacorrETH );
	  idxs.push_back(_idx_datacorrCERN);
	  idxs.push_back(_idx_datacorrUCSX);
    } 
    else if(sname.find("ewk")  != std::string::npos) {
	  idxs.push_back(_idx_ewk);
	  sname.erase(sname.find("ewk"), 3);
      if     (sname.find("DYJets") != std::string::npos) idxs.push_back(_idx_ewk_dy);
	  else if(sname.find("TTJets") != std::string::npos) idxs.push_back(_idx_ewk_tt);
	  else if(sname.find("WJets" ) != std::string::npos) idxs.push_back(_idx_ewk_wj);
      xs = _dbm -> getDBValue("XS", sname); 
      normalize = true;
	} 
    else if(sname.find("qcd")  != std::string::npos) {
	  idxs.push_back(_idx_qcd);
	  sname.erase(sname.find("qcd"), 3);
      if     (sname.find("bcToE"     ) != std::string::npos) idxs.push_back(_idx_qcd_bc);
      else if(sname.find("EMEnriched") != std::string::npos) idxs.push_back(_idx_qcd_em);
      else if(sname.find("Mu15"      ) != std::string::npos) idxs.push_back(_idx_qcd_mu15);
      else if(sname.find("Mu5"       ) != std::string::npos) idxs.push_back(_idx_qcd_mu5);
      xs = _dbm -> getDBValue("XS", sname); 
      normalize = true;
	} 

    for(unsigned int j = 0; j < obs.size(); ++j) {
      TH1 * sobs = _hm -> getHisto(obs[j], i);
      TH1 * cobs = (TH1*) sobs -> Clone();

      float factor = 1;

      if(normalize){
        if(_norm == "lumi"){
          unsigned int idx = atoi(obs[j].substr(obs[j].rfind("_")+3).c_str());
          bool iso         = (obs[j].substr(obs[j].rfind("_"),3) == "iso");    
 
          vector<string> linesel = _vTR_lines_el_non[0]; 
          vector<float>  effsel  = _vTR_efflum_el_non; 
          vector<float>  effsmu  = _vTR_efflum_mu_non; 
          if(iso) {
            linesel = _vTR_lines_el_iso[0];
            effsel  = _vTR_efflum_el_iso;
            effsmu  = _vTR_efflum_mu_iso;
          }

          float lumi = 1;
          if(linesel.size() <= idx) lumi = effsmu[idx];
          else                      lumi = effsel[idx];
          //factor = lumi;
          factor = xs * lumi / nevts;
        }        
      }
 
      for(unsigned int k = 0; k < idxs.size(); ++k) {
        TH1 * robs = _hm -> getHisto(obs[j], idxs[k]);
        robs -> Add(cobs, factor);
      }
    }
  }

  if(_norm == "data"){
    for(unsigned int j = 0; j < obs.size(); ++j) {
      float factor = 0.;
      for(unsigned int i = 0; i < _idxsmc.size(); ++i){
        TH1 * sobs = _hm -> getHisto(obs[j], _idxsmc[i]);
        factor += sobs->Integral();
      }

      if(ints_data[j] > 0 && factor > 0){
        factor /= float(ints_data[j]);
        factor = 1. / factor;
      }
      else {
        factor = 1.;
      }

      for(unsigned int i = 0; i < _idxsmc.size(); ++i){
        TH1 * sobs = _hm -> getHisto(obs[j], _idxsmc[i]);
        sobs -> Scale(factor);
      }
    }
  }
 
}


//____________________________________________________________________________
void FakeRatio::sumTriggers(){

  // get all observables
  vector<string> obs = _hm -> getObservables(true);

  // get observables which exists multiply for different triggers
  vector<string> tobs;
  for(unsigned int j = 0; j < obs.size(); ++j) {
    string oname = obs[j];
    if(oname.find("non") != std::string::npos) oname = oname.substr(0, oname.find("non"));
    if(oname.find("iso") != std::string::npos) oname = oname.substr(0, oname.find("iso"));

    if(find(tobs.begin(), tobs.end(), oname) == tobs.end())
      tobs.push_back(oname);
  }

  // sum trigger maps
  for(unsigned int i = 0; i < _numDS + _idxs.size(); ++i){
    for(unsigned int j = 0; j < tobs.size(); ++j) {
      sumTriggerPlots(tobs[j], i, "non");
      sumTriggerPlots(tobs[j], i, "iso");
    }
  }
}


//____________________________________________________________________________
void FakeRatio::sumTriggerPlots(string obs, int ds, string ext){

  // CH: note: electrons and muons are in different observables
  // so for an electron observable, the muon trigger plots will be empty
  // we can sum over them as it doesn't make a difference
  unsigned int num = _vTR_lines_el_non[0].size() + _vTR_lines_mu_non[0].size();
  if(ext == "iso")
    num = _vTR_lines_el_iso[0].size() + _vTR_lines_mu_iso[0].size();

  TH1* sum = _hm -> getHisto(obs + ext, ds);
  for(unsigned int i = 0; i < num; ++i)
    sum -> Add(_hm -> getHisto(obs + ext + Tools::intToString(i), ds));


}
    

//  ____________________________________________________________________________
void FakeRatio::writeOutput(){
  /*
    writes all output of this class to the disk
    paramters: none
    return: none
  */

  // decide if we need EWK subtraction or not (if not, jump to divideFRMaps call)
  doEwkSub();
 
  // sum numerators and denominators in categories 'data', 'datacorr', 'ewk', 'qcd' 
  sumMaps();

  // sum the plots for the different triggers
  sumTriggers();

  // subtract prompt contamination
  //testEwkSub();
  //subtractPromptsCERN();
  //subtractPromptsETH();    // only use either of the three methods as they reweight the EWK and QCD stuffs
  subtractPromptsUCSX();

  // make FR maps from numerators and denominators
  divideFRMaps();

}




/*****************************************************************************
******************************************************************************
** OBJECT SELECTIONS                                                        **
******************************************************************************
*****************************************************************************/


//____________________________________________________________________________
void FakeRatio::collectKinematicObjects(){
  /*
    collects all kinematic objects needed in the code, i.e. applies all object selections
    parameters: none
    return: none
  */
  
  for(int i = 0; i < _vc->get("n" + _leps); ++i){

    // electrons
    if(std::abs(_vc->get(_leps + "_pdgId", i)) == 11){		  
      if(denominatorElectronSelection(i)) {
        _denEls.push_back( Candidate::create(_vc->get(_leps + "_pt", i),
					  _vc->get(_leps + "_eta", i),
					  _vc->get(_leps + "_phi", i),
					  _vc->get(_leps + "_pdgId", i),
					  _vc->get(_leps + "_charge", i),
					  0.0005) );
        _denElsIdx    .push_back(i);
        _denLeps      .push_back( _denEls[ _denEls.size()-1 ] );
        _denLepsIdx   .push_back(i);
        if(_susyMod -> elHLTEmulSelIso(i)) _isoLepsIdx.push_back(i);
      }
      if(numeratorElectronSelection(i)) {
        _numEls.push_back( Candidate::create(_vc->get(_leps + "_pt", i),
					  _vc->get(_leps + "_eta", i),
					  _vc->get(_leps + "_phi", i),
					  _vc->get(_leps + "_pdgId", i),
					  _vc->get(_leps + "_charge", i),
					  0.0005) );
        _numElsIdx    .push_back(i);
        _numLeps      .push_back( _numEls[ _numEls.size()-1 ] );
        _numLepsIdx   .push_back(i);
      }
    }

    // muons
    else if(std::abs(_vc->get(_leps + "_pdgId", i)) == 13){
      if(denominatorMuonSelection(i)) {
        _denMus.push_back( Candidate::create(_vc->get(_leps + "_pt", i),
					  _vc->get(_leps + "_eta", i),
					  _vc->get(_leps + "_phi", i),
					  _vc->get(_leps + "_pdgId", i),
					  _vc->get(_leps + "_charge", i),
					  0.105) );
        _denMusIdx .push_back(i);
        _denLeps   .push_back( _denMus[ _denMus.size()-1 ] );
        _denLepsIdx.push_back(i);
        _isoLepsIdx.push_back(i);
      }
      if(numeratorMuonSelection(i)) {
        _numMus.push_back( Candidate::create(_vc->get(_leps + "_pt", i),
					  _vc->get(_leps + "_eta", i),
					  _vc->get(_leps + "_phi", i),
					  _vc->get(_leps + "_pdgId", i),
					  _vc->get(_leps + "_charge", i),
					  0.105) );
        _numMusIdx .push_back(i);
        _numLeps   .push_back( _numMus[ _numMus.size()-1 ] );
        _numLepsIdx.push_back(i);
      }
    }
  }

  _nDenEls  = _denEls .size();
  _nDenLeps = _denLeps.size();
  _nDenMus  = _denMus .size();
  _nNumEls  = _numEls .size();
  _nNumLeps = _numLeps.size();
  _nNumMus  = _numMus .size();
 
  _susyMod -> cleanJets( &_denLeps, _goodJets, _goodJetsIdx, _bJets, _bJetsIdx, 40, 25);
  _susyMod -> awayJets ( &_denLeps, _goodJets, _goodJetsIdx, 1.0);

  _nJets  = _goodJets.size();
  _HT     = _susyMod -> HT( &(_goodJets) );
  _met    = Candidate::create(_vc->get(_mets + "_pt"), _vc->get(_mets + "_phi") );

}


//____________________________________________________________________________
bool FakeRatio::denominatorElectronSelection(int elIdx){

  counter("DenominatorElectrons", kDenEls);

  if(!makeCut( _susyMod -> elIdSel     (elIdx, SusyModule::kTight, SusyModule::kLoose), "electron id", "=", kDenEls)) return false;
  if(!makeCut( _susyMod -> elHLTEmulSel(elIdx, false                                 ), "non-iso emu", "=", kDenEls)) return false;

  return true;

}


//____________________________________________________________________________
bool FakeRatio::denominatorMuonSelection(int muIdx){

  counter("DenominatorMuons", kDenMus);

  if(!makeCut( _susyMod -> muIdSel(muIdx, SusyModule::kTight), "muon id", "=", kDenMus)) return false;

  return true;

}


//____________________________________________________________________________
bool FakeRatio::numeratorElectronSelection(int elIdx){

  counter("NumeratorElectrons", kNumEls);

  if(!makeCut( _susyMod -> elIdSel     (elIdx, SusyModule::kTight, SusyModule::kLoose), "electron id ", "=", kNumEls)) return false;
  if(!makeCut( _susyMod -> elMvaSel    (elIdx, SusyModule::kTight)                    , "electron mva", "=", kNumEls)) return false;    
  if(!makeCut( _susyMod -> multiIsoSel (elIdx, SusyModule::kTight)                    , "isolation"   , "=", kNumEls)) return false; 
  if(!makeCut( _susyMod -> elHLTEmulSel(elIdx, false)                                 , "electron emu", "=", kNumEls)) return false;

  return true;

}


//____________________________________________________________________________
bool FakeRatio::numeratorMuonSelection(int muIdx){

  counter("NumeratorMuons", kNumMus);

  if(!makeCut( _susyMod -> muIdSel    (muIdx, SusyModule::kTight ), "muon id  ", "=", kNumMus)) return false;
  if(!makeCut( _susyMod -> multiIsoSel(muIdx, SusyModule::kMedium), "isolation", "=", kNumMus)) return false;
 
  return true;

}



/*****************************************************************************
******************************************************************************
** KINEMATIC REGION DEFINITIONS                                             **
******************************************************************************
*****************************************************************************/


//____________________________________________________________________________
void FakeRatio::setCut(std::string var, float valCut, std::string cType, float upValCut) {
  /*
    sets the parameters (valCut, cType, upValCut) for a specific cut on a variable (var)
    parameters: var, valCut (the cut value), cType (the cut type), upValCut (the upper value
    in case one cuts the variable in a range)
    return: none
  */


  // measurement region

  if(var == "NBJetsMR") {
    _valCutNBJetsMR   = valCut;
    _cTypeNBJetsMR    = cType;
    _upValCutNBJetsMR = upValCut;
  } 
	

}



//____________________________________________________________________________
void FakeRatio::setMeasurementRegion() {
  /*
    sets the cuts of the measurement region (MR)
    parameters: none
    return: none
  */


  // CH: currently not used, keep it for later
  //if(_MR == "all") {
  //  setCut("NBJetsMR", 0, ">=");
  //} 
  //else if(_MR == "MR00") {
  //  setCut("NBJetsMR", 0, "=");
  //}
  //else if(_MR == "MR01") {
  //  setCut("NBJetsMR", 1, "=");
  //}
  //else if(_MR == "MR02") {
  //  setCut("NBJetsMR", 2, "=");
  //}
  //else if(_MR == "MR03") {
  //  setCut("NBJetsMR", 3, ">=");
  //}


}



/*****************************************************************************
******************************************************************************
** EVENT SELECTIONS                                                         **
******************************************************************************
*****************************************************************************/


//____________________________________________________________________________
bool FakeRatio::baseSelection(){

  // lepton multiplicity
  if(!makeCut<int>( _nDenLeps            , 1 , "=" , "lepton multiplicity and flavor"  )) return false;
  if     (_sampleName.find("datael") != std::string::npos){ 
    if(!makeCut<int>( _nDenEls           , 1 , "=" , "electron multiplicity and flavor")) return false;
  }
  else if(_sampleName.find("datamu") != std::string::npos){ 
    if(!makeCut<int>( _nDenMus           , 1 , "=" , "muon multiplicity and flavor"    )) return false;
  }

  //QCD muon samples
  if     (_sampleName.find("Mu15") != std::string::npos && 
    !makeCut<float>(_denLeps[0] -> pt(), 15, ">" , "qcd mu15 binning (pt > 15)"    )) return false; 
  else if(_sampleName.find("Mu5")  != std::string::npos &&
    !makeCut<float>(_denLeps[0] -> pt(), 15, "<" , "qcd mu5 binning (pt < 15)"     )) return false; 

  // jet multiplicity
  if(!makeCut<int>( _nJets             ,  1, ">=", "jet multiplicity"              )) return false; 

  // triggers per FO
  if(!makeCut(triggerSelection()    , "trigger selection"     , "="                )) return false; 
  //if(!makeCut(triggerSelectionLite(), "trigger selection lite", "="                )) return false; 

  return true;
}


//____________________________________________________________________________
bool FakeRatio::ewkSelection(){

  // MET INVERTED! 
  if(!makeCut<float>( _met->pt() , 20.0, ">" , "MET selection"                 , 0, kEwkSel)) return false;

  return true;

}


//____________________________________________________________________________
bool FakeRatio::ucsxEwkSelection(){

  // MET INVERTED! 
  if(!makeCut<float>( _met->pt() , 30.0, ">" , "MET selection"                 , 0, kUcsSel)) return false;

  return true;

}


//____________________________________________________________________________
bool FakeRatio::mrSelection(){
  // CH: fake ratio measurement for RA5 sync exercise May 2015

  // MET 
  if(!makeCut<float>( _met -> pt(), 20.0, "<", "MET selection"   )) return false;

  // MT
  float MT = _susyMod -> coneMt(_denLepsIdx[0], _denLeps[0], _met);
  if(!makeCut<float>( MT          , 20.0, "<", "coneMT selection")) return false;




return true;


// sync stuff
  int isotr = -1;
  int nontr = -1;
  for(int i = 0; i < _TR_lines.size(); ++i){
    if(_TR_lines[i].find("IsoVVL") != std::string::npos || _TR_lines[i].find("IsoVL") != std::string::npos)
      isotr = i;
    else
      nontr = i;
  }

  //if(_iso){
    //if(_nDenEls == 1)
    //  cout << Form("%d %6.2f %6.2f %5.2f %.2f %.2f %5.2f %5.2f %5.2f %1.2f %1i %6.0f", int(_vc->get("evt")), _denLeps[0]->pt(), _susyMod->conePt(_denLepsIdx[0], SusyModule::kTight), _denLeps[0]->eta(), _vc->get("LepGood_miniRelIso", _denLepsIdx[0]), _vc->get("LepGood_jetPtRatiov2", _denLepsIdx[0]), _vc->get("LepGood_jetPtRelv2", _denLepsIdx[0]), _vc->get("LepGood_mvaIdSpring15", _denLepsIdx[0]), _met->pt(), _met->phi(), _nNumLeps, _trws[isotr]) << endl;
      //cout << Form("Electron RunNb=%8.f LumiSc=%4.f EventNb=%10.f Trigger=%s PS=%5.f FO raw pt=%6.2f corr pt=%6.2f eta=%5.2f miniiso=%.2f ptratio=%.2f ptrel=%5.2f mva=%5.2f MET=%5.2f MT=%5.2f isNum=%1i", _vc->get("run"), _vc->get("lumi"), _vc->get("evt"), _TR_lines[isotr].c_str(), _trws[isotr], _denLeps[0]->pt(), _susyMod->conePt(_denLepsIdx[0], SusyModule::kTight), _denLeps[0]->eta(), _vc->get("LepGood_miniRelIso", _denLepsIdx[0]), _vc->get("LepGood_jetPtRatiov2", _denLepsIdx[0]), _vc->get("LepGood_jetPtRelv2", _denLepsIdx[0]), _vc->get("LepGood_mvaIdSpring15", _denLepsIdx[0]), _met->pt(), MT, _nNumLeps) << endl;
    //if(_nDenMus == 1)
    //  cout << Form("%d %6.2f %6.2f %5.2f %.2f %.2f %5.2f %5.2f %1.2f %1i %6.0f", int(_vc->get("evt")), _denLeps[0]->pt(), _susyMod->conePt(_denLepsIdx[0], SusyModule::kMedium), _denLeps[0]->eta(), _vc->get("LepGood_miniRelIso", _denLepsIdx[0]), _vc->get("LepGood_jetPtRatiov2", _denLepsIdx[0]), _vc->get("LepGood_jetPtRelv2", _denLepsIdx[0]), _met->pt(), _met->phi(), _nNumLeps, _trws[isotr]) << endl;
  //    cout << Form("Muon RunNb=%8.f LumiSc=%4.f EventNb=%10.f Trigger=%s PS=%5.f FO raw pt=%6.2f corr pt=%6.2f eta=%5.2f miniiso=%.2f ptratio=%.2f ptrel=%5.2f MET=%5.2f MT=%5.2f isNum=%1i", _vc->get("run"), _vc->get("lumi"), _vc->get("evt"), _TR_lines[isotr].c_str(), _trws[isotr], _denLeps[0]->pt(), _susyMod->conePt(_denLepsIdx[0], SusyModule::kMedium), _denLeps[0]->eta(), _vc->get("LepGood_miniRelIso", _denLepsIdx[0]), _vc->get("LepGood_jetPtRatiov2", _denLepsIdx[0]), _vc->get("LepGood_jetPtRelv2", _denLepsIdx[0]), _met->pt(), MT, _nNumLeps) << endl;
  //}

  if(!_iso || _TR_lines.size() > 1){
    //if(_nDenEls == 1)
    //  cout << Form("%d %6.2f %6.2f %5.2f %.2f %.2f %5.2f %5.2f %5.2f %1.2f %1i %6.0f", int(_vc->get("evt")), _denLeps[0]->pt(), _susyMod->conePt(_denLepsIdx[0], SusyModule::kTight), _denLeps[0]->eta(), _vc->get("LepGood_miniRelIso", _denLepsIdx[0]), _vc->get("LepGood_jetPtRatiov2", _denLepsIdx[0]), _vc->get("LepGood_jetPtRelv2", _denLepsIdx[0]), _vc->get("LepGood_mvaIdSpring15", _denLepsIdx[0]), _met->pt(), _met->phi(), _nNumLeps, _trws[nontr]) << endl;
  //    cout << Form("Electron RunNb=%8.f LumiSc=%4.f EventNb=%10.f Trigger=%s PS=%5.f FO raw pt=%6.2f corr pt=%6.2f eta=%5.2f miniiso=%.2f ptratio=%.2f ptrel=%5.2f mva=%5.2f MET=%5.2f MT=%5.2f isNum=%1i", _vc->get("run"), _vc->get("lumi"), _vc->get("evt"), _TR_lines[nontr].c_str(), _trws[nontr], _denLeps[0]->pt(), _susyMod->conePt(_denLepsIdx[0], SusyModule::kTight), _denLeps[0]->eta(), _vc->get("LepGood_miniRelIso", _denLepsIdx[0]), _vc->get("LepGood_jetPtRatiov2", _denLepsIdx[0]), _vc->get("LepGood_jetPtRelv2", _denLepsIdx[0]), _vc->get("LepGood_mvaIdSpring15", _denLepsIdx[0]), _met->pt(), MT, _nNumLeps) << endl;
    if(_nDenMus == 1)
      cout << Form("%d %6.2f %6.2f %5.2f %.2f %.2f %5.2f %5.2f %1.2f %1i %6.0f", int(_vc->get("evt")), _denLeps[0]->pt(), _susyMod->conePt(_denLepsIdx[0], SusyModule::kMedium), _denLeps[0]->eta(), _vc->get("LepGood_miniRelIso", _denLepsIdx[0]), _vc->get("LepGood_jetPtRatiov2", _denLepsIdx[0]), _vc->get("LepGood_jetPtRelv2", _denLepsIdx[0]), _met->pt(), _met->phi(), _nNumLeps, _trws[nontr]) << endl;
  //    cout << Form("Muon RunNb=%8.f LumiSc=%4.f EventNb=%10.f Trigger=%s PS=%5.f FO raw pt=%6.2f corr pt=%6.2f eta=%5.2f miniiso=%.2f ptratio=%.2f ptrel=%5.2f MET=%5.2f MT=%5.2f isNum=%1i", _vc->get("run"), _vc->get("lumi"), _vc->get("evt"), _TR_lines[nontr].c_str(), _trws[nontr], _denLeps[0]->pt(), _susyMod->conePt(_denLepsIdx[0], SusyModule::kMedium), _denLeps[0]->eta(), _vc->get("LepGood_miniRelIso", _denLepsIdx[0]), _vc->get("LepGood_jetPtRatiov2", _denLepsIdx[0]), _vc->get("LepGood_jetPtRelv2", _denLepsIdx[0]),  _met->pt(), MT, _nNumLeps) << endl;
  }




//if(_iso){
//if(_nDenEls == 1)
//  cout << Form("%7.f %4.f %10.f %6.2f %6.2f %5.2f %.2f %.2f %5.2f %5.2f %5.2f %5.2f %1i", _vc->get("run"), _vc->get("lumi"), _vc->get("evt"), _denLeps[0]->pt(), _susyMod->conePt(_denLepsIdx[0], wp), _denLeps[0]->eta(), _vc->get("LepGood_miniRelIso", _denLepsIdx[0]), _vc->get("LepGood_jetPtRatiov2", _denLepsIdx[0]), _vc->get("LepGood_jetPtRelv2", _denLepsIdx[0]), _vc->get("LepGood_mvaIdSpring15", _denLepsIdx[0]), _met->pt(), MT->mass(), _nNumLeps) << endl;
  //cout << Form("Electron FO raw pt=%6.2f corr pt=%6.2f eta=%5.2f miniiso=%.2f ptratio=%.2f ptrel=%5.2f mva=%5.2f MET=%5.2f MT=%5.2f isNum=%1i evt=%12.f", _denLeps[0]->pt(), _susyMod->conePt(_denLepsIdx[0], wp), _denLeps[0]->eta(), _vc->get("LepGood_miniRelIso", _denLepsIdx[0]), _vc->get("LepGood_jetPtRatiov2", _denLepsIdx[0]), _vc->get("LepGood_jetPtRelv2", _denLepsIdx[0]), _vc->get("LepGood_mvaIdSpring15", _denLepsIdx[0]), _met->pt(), MT->mass(), _nNumLeps, _vc->get("evt")) << endl;
//if(_nDenMus == 1)
//  cout << Form("Muon FO raw pt=%6.2f corr pt=%6.2f eta=%5.2f miniiso=%.2f ptratio=%.2f ptrel=%5.2f MET=%5.2f MT=%5.2f isNum=%1i evt=%10.f", _denLeps[0]->pt(), _susyMod->conePt(_denLepsIdx[0], wp), _denLeps[0]->eta(), _vc->get("LepGood_miniRelIso", _denLepsIdx[0]), _vc->get("LepGood_jetPtRatiov2", _denLepsIdx[0]), _vc->get("LepGood_jetPtRelv2", _denLepsIdx[0]), _met->pt(), MT->mass(), _nNumLeps, _vc->get("evt")) << endl;
//}

//cout << Form("%9.0f %9.0f %12.f\t%5.2f\%5.2f\t%5.2f\t%5.2f\t%5.2f\t%5.2f\t%5.2f\t%5.2f\t%2d\t%5.2f\t%5.2f\t%2d", _vc->get("run"), _vc->get("lumi"), _vc->get("evt"), _denLeps[0]->pt(), _susyMod->conePt(_denLepsIdx[0], wp), _denLeps[0]->eta(), _denLeps[0]->phi(), _vc->get("LepGood_miniRelIso", _denLepsIdx[0]), _vc->get("LepGood_jetPtRatiov2", _denLepsIdx[0]), _vc->get("LepGood_jetPtRelv2", _denLepsIdx[0]), _vc->get("LepGood_mvaIdSpring15", _denLepsIdx[0]), _nNumLeps, MT->mass(), _met ->pt(), _nJets) << endl; 
//cout << _denLeps[0]->pt() << ":" << _denLeps[0]->eta() << ":" << _denLeps[0]->phi() << ":" << _vc->get("LepGood_miniRelIso", _denLepsIdx[0]) << ":" <<  MT->mass() << ":" << _vc->get("LepGood_mediumMuonId", _denLepsIdx[0]) << ":" << _vc->get("LepGood_tightCharge", _denLepsIdx[0]) << ":" << _met ->pt() << ":" << _nJets << endl;

  //fillTriggerTestPlots();

  return true;

}


//____________________________________________________________________________
bool FakeRatio::qcdSelection(){

  return true;

}


//____________________________________________________________________________
bool FakeRatio::skimSelection(){

  if(!makeCut<int>( _nDenLeps, 1, "=" , "lepton multiplicity and flavor") ) return false;
  if(!makeCut<int>( _nJets   , 1, ">=", "jet multiplicity"              ) ) return false; 

  return true;

}


//____________________________________________________________________________
bool FakeRatio::triggerSelection(){ 
  // trigger selection: combination of triggers per fake ratio bin of pt


  counter("TriggerSelection", kTrigger);

  bool any = false; //passed at least one of any trigger
      _iso = false; //passed at least one of the isolated triggers
  vector<string> trlines; 

  if     (std::abs(_denLeps[0]->pdgId()) == 11) {
    //int etabin = findBin(_vFR_bins_eta_el, std::abs(_denLeps[0]->eta()));
    //int ptbin  = findBin(_vFR_bins_pt_el ,          _denLeps[0]->pt ()) ;
    //if(etabin > -1 && ptbin > -1){
    //  vector<string> lines_non = _vTR_lineperpteta_el_non[etabin][ptbin];
    //  vector<string> lines_iso = _vTR_lineperpteta_el_iso[etabin][ptbin];
    //  trlines = Tools::insertIntoVectorS(trlines, lines_non);
    //  trlines = Tools::insertIntoVectorS(trlines, lines_iso);
    //}
    trlines.push_back("HLT_FR_Ele12_CaloIdM_TrackIdM_PFJet30");
    trlines.push_back("HLT_FR_Ele12_CaloIdL_TrackIdL_IsoVL_PFJet30");
  }
  else if(std::abs(_denLeps[0]->pdgId()) == 13) {
    int etabin = findBin(_vFR_bins_eta_mu, std::abs(_denLeps[0]->eta()));
    int ptbin  = findBin(_vFR_bins_pt_mu ,          _denLeps[0]->pt ()) ;
    if(etabin > -1 && ptbin > -1){
      vector<string> lines_non = _vTR_lineperpteta_mu_non[findBin(_vFR_bins_eta_mu, std::abs(_denLeps[0]->eta()))]
                                                         [findBin(_vFR_bins_pt_mu ,          _denLeps[0]->pt ()) ];
      vector<string> lines_iso = _vTR_lineperpteta_mu_iso[findBin(_vFR_bins_eta_mu, std::abs(_denLeps[0]->eta()))]
                                                         [findBin(_vFR_bins_pt_mu ,          _denLeps[0]->pt ()) ];
      trlines = Tools::insertIntoVectorS(trlines, lines_non);
      trlines = Tools::insertIntoVectorS(trlines, lines_iso);
    }
  }

  if(trlines.size() > 0){
    for(unsigned int i = 0; i < trlines.size(); ++i){

      if(Tools::trim(trlines[i]) != ""){
        bool testiso = false;
        if(trlines[i].find("IsoVL") != std::string::npos || trlines[i].find("IsoVVL") != std::string::npos) 
          testiso = true;

        if(_vc->get(Tools::trim(trlines[i])) == 1) {
          if(testiso){
           if(_isoLepsIdx.size() == 0 || _isoLepsIdx[0] != _denLepsIdx[0]) 
             continue;
           _iso = true;
          }
          any = true;
          _TR_lines.push_back(Tools::trim(trlines[i]));
        }
      }
    }

  }


  // at least one auxiliary trigger per pt bin to be passed
  if(!makeCut(any, "at least one trigger path", "=", kTrigger)) return false;

  // trigger extensions
  findTriggerExts();

  return true;

}


//____________________________________________________________________________
bool FakeRatio::triggerSelectionLite(){ 
  // lite: only one trigger for the entire map

  counter("TriggerSelection", kTrigger);

  bool any  = false; //passed at least one of any trigger
      _iso  = false; //passed at least one of the isolated triggers
      _trel = false;
      _trmu = false;
  _exts.clear();

  // electrons
  if(_nDenEls == 1){
    if(_vc->get("HLT_FR_Ele12_CaloIdL_TrackIdL_IsoVL_PFJet30") == 1) {
      any   = true; 
      _trel = true;
      _iso  = true; 
      _exts.push_back("iso0");
    }
    if(_vc->get("HLT_FR_Ele12_CaloIdM_TrackIdM_PFJet30") == 1) {
      any = true;
      _trel = true;
      _exts.push_back("non1");
    }
  }

  if(_nDenMus == 1){
    if(_vc->get("HLT_FR_Mu17_TrkIsoVVL") == 1) {
      any = true; 
      _trmu = true;
      _iso = true; 
      _exts.push_back("iso4");
    }
    if(_vc->get("HLT_FR_Mu17") == 1) {
      any = true;
      _trmu = true;
      _exts.push_back("non5");
    }
  }



  // at least one auxiliary trigger per pt bin to be passed
  if(!makeCut(any, "at least one trigger path", "=", kTrigger)) return false;

  return true;

}




/*****************************************************************************
******************************************************************************
** EXECUTING TASKS                                                          **
******************************************************************************
*****************************************************************************/


//____________________________________________________________________________
void FakeRatio::fillEventPlots(){
  /*
    fills the control plots for event quantities
    parameters: none
    return: none
  */

  for(unsigned int i = 0; i < _exts.size(); ++i){
    fill("MR_HT_"         + _exts[i], _HT                 , _weight * _trws[i]);
    fill("MR_MET_"        + _exts[i], _met->pt()          , _weight * _trws[i]);
    fill("MR_NumBJets_"   + _exts[i], _vc->get(_bvar)     , _weight * _trws[i]);
    fill("MR_NumDenEls_"  + _exts[i], _denEls .size()     , _weight * _trws[i]);
    fill("MR_NumDenLeps_" + _exts[i], _denLeps.size()     , _weight * _trws[i]);
    fill("MR_NumDenMus_"  + _exts[i], _denMus .size()     , _weight * _trws[i]);
    fill("MR_NumJets_"    + _exts[i], _nJets              , _weight * _trws[i]);
    fill("MR_NumNumEls_"  + _exts[i], _numEls .size()     , _weight * _trws[i]);
    fill("MR_NumNumLeps_" + _exts[i], _numLeps.size()     , _weight * _trws[i]);
    fill("MR_NumNumMus_"  + _exts[i], _numMus .size()     , _weight * _trws[i]);
    fill("MR_NumVrtx_"    + _exts[i], _vc->get(_nvert)    , _weight * _trws[i]);
  }

}


//____________________________________________________________________________
void FakeRatio::fillEwkEventPlots(){
  /*
    fills the control plots for event quantities
    parameters: none
    return: none
  */

  for(unsigned int i = 0; i < _exts.size(); ++i){
    fill("ER_HT_"  + _exts[i], _HT                 , _weight * _trws[i]);
    fill("ER_MET_" + _exts[i], _met->pt()          , _weight * _trws[i]);
  }

}


//____________________________________________________________________________
void FakeRatio::fillEwkLepPlots(string prepend, Candidate * lep, int lepIdx, int wp){
  /*
    fills the control plots for leptons PER LEPTON
  */  

  //Candidate * mt = Candidate::create( lep, _met);0
  float MT = _susyMod -> coneMt(lepIdx, lep, _met);

  for(unsigned int i = 0; i < _exts.size(); ++i){
    //fill(prepend + "Pt_"      + _exts[i],          _vc->get(_leps + "_pt"        , lepIdx) , _weight * _trws[i]);
    //fill(prepend + "Eta_"     + _exts[i], std::abs(_vc->get(_leps + "_eta"       , lepIdx)), _weight * _trws[i]);
    //fill(prepend + "DXY_"     + _exts[i], std::abs(_vc->get(_leps + "_dxy"       , lepIdx)), _weight * _trws[i]);
    //fill(prepend + "DZ_"      + _exts[i], std::abs(_vc->get(_leps + "_dz"        , lepIdx)), _weight * _trws[i]);
    //fill(prepend + "SIP_"     + _exts[i],          _vc->get(_leps + "_sip3d"     , lepIdx) , _weight * _trws[i]);
    //fill(prepend + "MiniIso_" + _exts[i],          _vc->get(_leps + "_miniRelIso", lepIdx) , _weight * _trws[i]);
    //fill(prepend + "MT_"      + _exts[i], mt -> mass()                                     , _weight * _trws[i]);
    //fill(prepend + "MT0_"     + _exts[i], mt -> mass()                                     , _weight * _trws[i]);
    //fill(prepend + "MT1_"     + _exts[i], mt -> mass()                                     , _weight * _trws[i]);
    fill(prepend + "MT_"      + _exts[i], MT                                                , _weight * _trws[i]);
    fill(prepend + "MT0_"     + _exts[i], MT                                                , _weight * _trws[i]);
    fill(prepend + "MT1_"     + _exts[i], MT                                                , _weight * _trws[i]);
  }

}


//____________________________________________________________________________
void FakeRatio::fillEwkLeptonPlots(){
  /*
    fills the control plots for leptons
    parameters: none
    return: none
  */

  for(unsigned int i = 0; i < _denEls.size(); ++i) { fillEwkLepPlots("ER_DenEl"  , _denEls[i]  , _denElsIdx[i], SusyModule::kTight ); }
  for(unsigned int i = 0; i < _denMus.size(); ++i) { fillEwkLepPlots("ER_DenMu"  , _denMus[i]  , _denMusIdx[i], SusyModule::kMedium); }
  for(unsigned int i = 0; i < _numEls.size(); ++i) { fillEwkLepPlots("ER_NumEl"  , _numEls[i]  , _numElsIdx[i], SusyModule::kTight ); }
  for(unsigned int i = 0; i < _numMus.size(); ++i) { fillEwkLepPlots("ER_NumMu"  , _numMus[i]  , _numMusIdx[i], SusyModule::kMedium); }

}


//____________________________________________________________________________
void FakeRatio::fillFRMaps(string prepend, Candidate * lep, int lepIdx, int wp){

  for(unsigned int i = 0; i < _exts.size(); ++i){
    fill(prepend + "MapPt_"     + _exts[i], overflowPt(_vc->get(_leps + "_pt", lepIdx)) , std::abs(_vc->get(_leps + "_eta", lepIdx)), _weight * _trws[i]);
    fill(prepend + "MapPtJet_"  + _exts[i], overflowPt(_susyMod -> closestJetPt(lepIdx)), std::abs(_vc->get(_leps + "_eta", lepIdx)), _weight * _trws[i]);
    fill(prepend + "MapPtCorr_" + _exts[i], overflowPt(_susyMod -> conePt(lepIdx, wp))  , std::abs(_vc->get(_leps + "_eta", lepIdx)), _weight * _trws[i]);
  }

}


//____________________________________________________________________________
void FakeRatio::fillFakeRatioMaps(){
  /*
    fills the fake ratio maps for leptons
    parameters: none
    return: none
  */

  for(unsigned int i = 0; i < _denEls.size(); ++i) { fillFRMaps("MR_DenEl", _denEls[i]  , _denElsIdx[i], SusyModule::kTight ); }
  for(unsigned int i = 0; i < _denMus.size(); ++i) { fillFRMaps("MR_DenMu", _denMus[i]  , _denMusIdx[i], SusyModule::kMedium); }
  for(unsigned int i = 0; i < _numEls.size(); ++i) { fillFRMaps("MR_NumEl", _numEls[i]  , _numElsIdx[i], SusyModule::kTight );
                                                     fillFRMaps("MR_RatEl", _numEls[i]  , _numElsIdx[i], SusyModule::kTight ); }
  for(unsigned int i = 0; i < _numMus.size(); ++i) { fillFRMaps("MR_NumMu", _numMus[i]  , _numMusIdx[i], SusyModule::kMedium);
                                                     fillFRMaps("MR_RatMu", _numMus[i]  , _numMusIdx[i], SusyModule::kMedium); }

}


//____________________________________________________________________________
void FakeRatio::fillLepPlots(string prepend, Candidate * lep, int lepIdx, int wp){
  /*
    fills the control plots for leptons PER LEPTON
  */  

  float MT = _susyMod -> coneMt(lepIdx, lep, _met);
  for(unsigned int i = 0; i < _exts.size(); ++i){
    fill(prepend + "Pt_"       + _exts[i],          _vc->get(_leps + "_pt"        , lepIdx) , _weight * _trws[i]);
    fill(prepend + "Eta_"      + _exts[i], std::abs(_vc->get(_leps + "_eta"       , lepIdx)), _weight * _trws[i]);
    fill(prepend + "EtaFR_"    + _exts[i], std::abs(_vc->get(_leps + "_eta"       , lepIdx)), _weight * _trws[i]);
    fill(prepend + "DXY_"      + _exts[i], std::abs(_vc->get(_leps + "_dxy"       , lepIdx)), _weight * _trws[i]);
    fill(prepend + "DZ_"       + _exts[i], std::abs(_vc->get(_leps + "_dz"        , lepIdx)), _weight * _trws[i]);
    fill(prepend + "SIP_"      + _exts[i],          _vc->get(_leps + "_sip3d"     , lepIdx) , _weight * _trws[i]);
    fill(prepend + "RelIso_"   + _exts[i],          _vc->get(_leps + "_relIso03"  , lepIdx) , _weight * _trws[i]);
    fill(prepend + "MiniIso_"  + _exts[i],          _vc->get(_leps + "_miniRelIso", lepIdx) , _weight * _trws[i]);
    fill(prepend + "PtRel_"    + _exts[i],          _vc->get(_leps + "_jetPtRelv2", lepIdx) , _weight * _trws[i]);
    fill(prepend + "MT_"       + _exts[i], MT                                               , _weight * _trws[i]);
    //fill(prepend + "MT_"       + _exts[i], Candidate::create( lep, _met) -> mass()          , _weight * _trws[i]);
    fill(prepend + "PtJet_"    + _exts[i], _susyMod -> closestJetPt(lepIdx)                 , _weight * _trws[i]);
    fill(prepend + "PtCorr_"   + _exts[i], _susyMod -> conePt(lepIdx, wp)                   , _weight * _trws[i]);
    fill(prepend + "PtCorrFR_" + _exts[i], overflowPt(_susyMod -> conePt(lepIdx, wp))       , _weight * _trws[i]);
  }

}


//____________________________________________________________________________
void FakeRatio::fillLeptonPlots(){
  /*
    fills the control plots for leptons
    parameters: none
    return: none
  */

  for(unsigned int i = 0; i < _denEls.size(); ++i) { fillLepPlots("MR_DenEl"  , _denEls[i]  , _denElsIdx[i], SusyModule::kTight ); }
  for(unsigned int i = 0; i < _denMus.size(); ++i) { fillLepPlots("MR_DenMu"  , _denMus[i]  , _denMusIdx[i], SusyModule::kMedium); }
  for(unsigned int i = 0; i < _numEls.size(); ++i) { fillLepPlots("MR_NumEl"  , _numEls[i]  , _numElsIdx[i], SusyModule::kTight ); }
  for(unsigned int i = 0; i < _numMus.size(); ++i) { fillLepPlots("MR_NumMu"  , _numMus[i]  , _numMusIdx[i], SusyModule::kMedium); }

}


//____________________________________________________________________________
void FakeRatio::fillJetPlots(){
  /*
    fills the control plots for jets
    parameters: none
    return: none
  */

  for(unsigned int i = 0; i < _exts.size(); ++i){
    for(unsigned int j = 0; j < _goodJets.size(); ++j){
      fill("MR_JetPt_" + _exts[i], _goodJets[j]->pt(), _weight * _trws[i]);
    }
  }
}


//____________________________________________________________________________
void FakeRatio::fillQcdEventPlots(){
  /*
    fills the control plots for event quantities
    parameters: none
    return: none
  */

  //vector<string> exts = _findTriggerExts();
  for(unsigned int i = 0; i < _exts.size(); ++i){
    fill("QR_HT_"  + _exts[i]        , _HT       , _weight * _trws[i]);
    fill("QR_MET_" + _exts[i]        , _met->pt(), _weight * _trws[i]);
    if(_met -> pt() < 20) 
      fill("QR_MET_small_" + _exts[i], 1         , _weight * _trws[i]);
    if(_met -> pt() < 80 && _met -> pt() > 45) 
      fill("QR_MET_large_" + _exts[i], 1         , _weight * _trws[i]);
  }

}


//____________________________________________________________________________
void FakeRatio::fillQcdFakeRatioMaps(){
  /*
    fills the fake ratio maps for leptons
    parameters: none
    return: none
  */

  if(_met -> pt() < 20){
    for(unsigned int i = 0; i < _denEls.size(); ++i)   fillFRMaps("QR_small_DenEl", _denEls[i]  , _denElsIdx[i], SusyModule::kTight );
    for(unsigned int i = 0; i < _denMus.size(); ++i)   fillFRMaps("QR_small_DenMu", _denMus[i]  , _denMusIdx[i], SusyModule::kMedium);
    for(unsigned int i = 0; i < _numEls.size(); ++i) { fillFRMaps("QR_small_NumEl", _numEls[i]  , _numElsIdx[i], SusyModule::kTight );
                                                       fillFRMaps("QR_small_RatEl", _numEls[i]  , _numElsIdx[i], SusyModule::kTight ); }
    for(unsigned int i = 0; i < _numMus.size(); ++i) { fillFRMaps("QR_small_NumMu", _numMus[i]  , _numMusIdx[i], SusyModule::kMedium);
                                                       fillFRMaps("QR_small_RatMu", _numMus[i]  , _numMusIdx[i], SusyModule::kMedium); }
  }

  if(_met -> pt() > 45 && _met -> pt() < 80){
    for(unsigned int i = 0; i < _denEls.size(); ++i)   fillFRMaps("QR_large_DenEl", _denEls[i]  , _denElsIdx[i], SusyModule::kTight );
    for(unsigned int i = 0; i < _denMus.size(); ++i)   fillFRMaps("QR_large_DenMu", _denMus[i]  , _denMusIdx[i], SusyModule::kMedium);
    for(unsigned int i = 0; i < _numEls.size(); ++i) { fillFRMaps("QR_large_NumEl", _numEls[i]  , _numElsIdx[i], SusyModule::kTight );
                                                       fillFRMaps("QR_large_RatEl", _numEls[i]  , _numElsIdx[i], SusyModule::kTight ); }
    for(unsigned int i = 0; i < _numMus.size(); ++i) { fillFRMaps("QR_large_NumMu", _numMus[i]  , _numMusIdx[i], SusyModule::kMedium);
                                                       fillFRMaps("QR_large_RatMu", _numMus[i]  , _numMusIdx[i], SusyModule::kMedium); }
  }

}


//____________________________________________________________________________
void FakeRatio::fillUcsxEwkEventPlots(){
  /*
    fills the control plots for event quantities
    parameters: none
    return: none
  */

  for(unsigned int i = 0; i < _exts.size(); ++i){
    fill("UR_HT_"  + _exts[i], _HT                 , _weight * _trws[i]);
    fill("UR_MET_" + _exts[i], _met->pt()          , _weight * _trws[i]);
  }

}


//____________________________________________________________________________
void FakeRatio::fillUcsxEwkLepPlots(string prepend, Candidate * lep, int lepIdx, int wp){
  /*
    fills the control plots for leptons PER LEPTON
  */  

  //Candidate * mt = Candidate::create( lep, _met);0
  float MT = _susyMod -> coneMt(lepIdx, lep, _met);

  for(unsigned int i = 0; i < _exts.size(); ++i){
    fill(prepend + "MT_"      + _exts[i], MT                                                , _weight * _trws[i]);
    fill(prepend + "MT0_"     + _exts[i], MT                                                , _weight * _trws[i]);
    fill(prepend + "MT1_"     + _exts[i], MT                                                , _weight * _trws[i]);
  }

}


//____________________________________________________________________________
void FakeRatio::fillUcsxEwkLeptonPlots(){
  /*
    fills the control plots for leptons
    parameters: none
    return: none
  */

  for(unsigned int i = 0; i < _denEls.size(); ++i) { fillUcsxEwkLepPlots("UR_DenEl"  , _denEls[i]  , _denElsIdx[i], SusyModule::kTight ); }
  for(unsigned int i = 0; i < _denMus.size(); ++i) { fillUcsxEwkLepPlots("UR_DenMu"  , _denMus[i]  , _denMusIdx[i], SusyModule::kMedium); }
  for(unsigned int i = 0; i < _numEls.size(); ++i) { fillUcsxEwkLepPlots("UR_NumEl"  , _numEls[i]  , _numElsIdx[i], SusyModule::kTight ); }
  for(unsigned int i = 0; i < _numMus.size(); ++i) { fillUcsxEwkLepPlots("UR_NumMu"  , _numMus[i]  , _numMusIdx[i], SusyModule::kMedium); }

}


//____________________________________________________________________________
void FakeRatio::findTriggerExts(){


  for(unsigned int i = 0; i < _TR_lines.size(); ++i){

    vector<string>::iterator pos = find(_vTR_lines_el_non[0].begin(), _vTR_lines_el_non[0].end(), _TR_lines[i]);
    if(pos != _vTR_lines_el_non[0].end()){
      _exts.push_back("non" + Tools::intToString(pos - _vTR_lines_el_non[0].begin()));
      if(_isData)
        _trws.push_back(_vc->get(_vTR_psbh_el_non[pos - _vTR_lines_el_non[0].begin()]) * \
                        _vc->get(_vTR_psbl_el_non[pos - _vTR_lines_el_non[0].begin()]) );
      else
        _trws.push_back(1.0);
    }

    pos = find(_vTR_lines_mu_non[0].begin(), _vTR_lines_mu_non[0].end(), _TR_lines[i]);
    if(pos != _vTR_lines_mu_non[0].end()){
      _exts.push_back("non" + Tools::intToString(pos - _vTR_lines_mu_non[0].begin() + _vTR_lines_el_non[0].size()));
      if(_isData)
        _trws.push_back(_vc->get(_vTR_psbh_mu_non[pos - _vTR_lines_mu_non[0].begin()]) * \
                        _vc->get(_vTR_psbl_mu_non[pos - _vTR_lines_mu_non[0].begin()]) );
      else
        _trws.push_back(1.0);
    }
  }

  if(_iso){
    for(unsigned int i = 0; i < _TR_lines.size(); ++i){

      vector<string>::iterator pos = find(_vTR_lines_el_iso[0].begin(), _vTR_lines_el_iso[0].end(), _TR_lines[i]);
      if(pos != _vTR_lines_el_iso[0].end()){
        _exts.push_back("iso" + Tools::intToString(pos - _vTR_lines_el_iso[0].begin()));
        if(_isData)
          _trws.push_back(_vc->get(_vTR_psbh_el_iso[pos - _vTR_lines_el_iso[0].begin()]) * \
                          _vc->get(_vTR_psbl_el_iso[pos - _vTR_lines_el_iso[0].begin()]) );
        else
          _trws.push_back(1.0);
      }

      pos = find(_vTR_lines_mu_iso[0].begin(), _vTR_lines_mu_iso[0].end(), _TR_lines[i]);
      if(pos != _vTR_lines_mu_iso[0].end()){
        _exts.push_back("iso" + Tools::intToString(pos - _vTR_lines_mu_iso[0].begin() + _vTR_lines_el_iso[0].size()));
        if(_isData){
          _trws.push_back(_vc->get(_vTR_psbh_mu_iso[pos - _vTR_lines_mu_iso[0].begin()]) * \
                          _vc->get(_vTR_psbl_mu_iso[pos - _vTR_lines_mu_iso[0].begin()]) );
          }
        else
          _trws.push_back(1.0);
      }
    }
  }

}


//____________________________________________________________________________
float FakeRatio::overflowPt(float rawPt){

	if(rawPt >= 70.) return 60.;

	return rawPt;
}

