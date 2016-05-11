#ifndef DIJETSCOUTINGROOTTREEMAKER_DIJETCALOSCOUTINGTREEPRODUCER_H
#define DIJETSCOUTINGROOTTREEMAKER_DIJETCALOSCOUTINGTREEPRODUCER_H

// System include files
#include <iostream>
#include <vector>
#include <string>

// CMSSW include files
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"
#include "CondFormats/JetMETObjects/interface/FactorizedJetCorrector.h"
#include "DataFormats/Math/interface/deltaPhi.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/MET.h"
#include "DataFormats/Scouting/interface/ScoutingCaloJet.h"
#include "DataFormats/Scouting/interface/ScoutingVertex.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/FileInPath.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "HLTrigger/HLTcore/interface/TriggerExpressionData.h"
#include "HLTrigger/HLTcore/interface/TriggerExpressionEvaluator.h"
#include "HLTrigger/HLTcore/interface/TriggerExpressionParser.h"
#include "L1Trigger/L1TGlobal/interface/L1TGlobalUtil.h"


// Root include files
#include "TLorentzVector.h"
#include "TTree.h"


class DijetCaloScoutingTreeProducer : public edm::EDAnalyzer
{
public:
    explicit DijetCaloScoutingTreeProducer(edm::ParameterSet const& cfg);
    virtual ~DijetCaloScoutingTreeProducer();

private:
    virtual void beginJob();
    virtual void beginRun(edm::Run const& iRun, edm::EventSetup const& iSetup);
    virtual void analyze(edm::Event const& iEvent, edm::EventSetup const& iSetup);
    virtual void endJob();

    void initialize();
    // For JECs
    bool doJECs_;
    edm::FileInPath L1corrAK4_DATA_, L2corrAK4_DATA_, L3corrAK4_DATA_,
        ResCorrAK4_DATA_;
    edm::FileInPath L1corrAK4reco_DATA_, L2corrAK4reco_DATA_,
        L3corrAK4reco_DATA_, ResCorrAK4reco_DATA_;
    JetCorrectorParameters *L1ParAK4_DATA;
    JetCorrectorParameters *L2ParAK4_DATA;
    JetCorrectorParameters *L3ParAK4_DATA;
    JetCorrectorParameters *L2L3ResAK4_DATA;
    FactorizedJetCorrector *JetCorrectorAK4_DATA;
    JetCorrectorParameters *L1ParAK4reco_DATA;
    JetCorrectorParameters *L2ParAK4reco_DATA;
    JetCorrectorParameters *L3ParAK4reco_DATA;
    JetCorrectorParameters *L2L3ResAK4reco_DATA;
    FactorizedJetCorrector *JetCorrectorAK4reco_DATA;
    //---- configurable parameters --------
    double ptMinAK4_;
    edm::EDGetTokenT<ScoutingCaloJetCollection> srcJetsAK4_;
    edm::EDGetTokenT<ScoutingVertexCollection> srcVrtx_;
    edm::EDGetTokenT<double> srcRho_, srcMET_;
    bool doRECO_;
    edm::EDGetTokenT<pat::JetCollection> srcJetsAK4reco_;
    edm::EDGetTokenT<reco::VertexCollection> srcVrtxreco_;
    edm::EDGetTokenT<double> srcRhoreco_;
    edm::EDGetTokenT<pat::METCollection> srcMETreco_;
    edm::Service<TFileService> fs_;
    TTree *outTree_;

    //---- TRIGGER -------------------------
    triggerExpression::Data triggerCache_;
    std::vector<triggerExpression::Evaluator*> vtriggerSelector_;
    std::vector<std::string> vtriggerAlias_, vtriggerSelection_;
    std::vector<int> vtriggerDuplicates_;
    TH1F *triggerPassHisto_, *triggerNamesHisto_;
    //---- output TREE variables ------
    //---- global event variables -----
    int   isData_, run_, evt_, nVtx_, lumi_;
    int nVtxreco_;
    int   nJetsAK4_, nJetsAK4reco_;
    float rho_, met_, mhtAK4_, mhtAK4Sig_;
    float rhoreco_, metreco_, metrecoSig_, mhtAK4reco_, mhtAK4recoSig_;
    float htAK4_, mjjAK4_, dEtajjAK4_, dPhijjAK4_;
    float htAK4reco_, mjjAK4reco_, dEtajjAK4reco_, dPhijjAK4reco_;
    std::vector<bool> *triggerResult_;
    //---- L1 ----
    bool doL1_;
    edm::EDGetToken algToken_;
    l1t::L1TGlobalUtil *l1GtUtils_;
    std::vector<std::string> l1Seeds_;
    std::vector<bool> *l1Result_;

    //---- jet and genJet variables --------------
    std::vector<float> *ptAK4_, *jecAK4_, *etaAK4_, *phiAK4_, *massAK4_,
        *energyAK4_, *areaAK4_, *csvAK4_, *hadfAK4_, *emfAK4_;
    std::vector<int> *idAK4_;
    std::vector<float> *hf_hfAK4_, *hf_emfAK4_;
    std::vector<float> *ptAK4reco_, *jecAK4reco_, *etaAK4reco_, *phiAK4reco_,
        *massAK4reco_, *energyAK4reco_, *areaAK4reco_, *csvAK4reco_,
        *chfAK4reco_, *nhfAK4reco_, *phfAK4reco_, *elfAK4reco_, *mufAK4reco_,
        *nemfAK4reco_, *cemfAK4reco_;
    std::vector<int> *idLAK4reco_, *idTAK4reco_, *chHadMultAK4reco_,
        *chMultAK4reco_, *neHadMultAK4reco_, *neMultAK4reco_, *phoMultAK4reco_;
    std::vector<float> *hf_hfAK4reco_, *hf_emfAK4reco_, *hofAK4reco_;
};

#endif
