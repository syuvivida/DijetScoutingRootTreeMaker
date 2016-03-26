#ifndef DIJETMINIAODROOTTREEMAKER_DIJETSCOUTINGTREEPRODUCER_H
#define DIJETMINIAODROOTTREEMAKER_DIJETSCOUTINGTREEPRODUCER_H

// System include files
#include <iostream>
#include <vector>

// CMSSW include files
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"
#include "CondFormats/JetMETObjects/interface/FactorizedJetCorrector.h"
#include "DataFormats/Math/interface/deltaPhi.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/MET.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "DataFormats/Scouting/interface/ScoutingElectron.h"
#include "DataFormats/Scouting/interface/ScoutingMuon.h"
#include "DataFormats/Scouting/interface/ScoutingParticle.h"
#include "DataFormats/Scouting/interface/ScoutingPFJet.h"
#include "DataFormats/Scouting/interface/ScoutingPhoton.h"
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
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"

// Root include files
#include "TLorentzVector.h"
#include "TTree.h"


class DijetMiniAODTreeProducer : public edm::EDAnalyzer
{
public:
    explicit DijetMiniAODTreeProducer(edm::ParameterSet const& cfg);
    virtual ~DijetMiniAODTreeProducer();

private:
    virtual void beginJob();
    virtual void analyze(edm::Event const& iEvent, edm::EventSetup const& iSetup);
    virtual void endJob();

    void initialize();
    // For JECs
    bool doJECs_;
    edm::FileInPath L1corrAK4_DATA_, L2corrAK4_DATA_, L3corrAK4_DATA_,
        ResCorrAK4_DATA_;
    edm::FileInPath L1corrAK4_MC_, L2corrAK4_MC_, L3corrAK4_MC_;
    JetCorrectorParameters *L1ParAK4_DATA;
    JetCorrectorParameters *L2ParAK4_DATA;
    JetCorrectorParameters *L3ParAK4_DATA;
    JetCorrectorParameters *L2L3ResAK4_DATA;
    JetCorrectorParameters *L1ParAK4_MC;
    JetCorrectorParameters *L2ParAK4_MC;
    JetCorrectorParameters *L3ParAK4_MC;
    FactorizedJetCorrector *JetCorrectorAK4_DATA;
    FactorizedJetCorrector *JetCorrectorAK4_MC;
    //---- configurable parameters --------
    double ptMinAK4_;
    edm::EDGetTokenT<pat::JetCollection> srcJetsAK4_;
    edm::EDGetTokenT<edm::View<reco::GenJet>> srcGenJetsAK4_;
    edm::EDGetTokenT<reco::VertexCollection> srcVrtx_;
    edm::EDGetTokenT<double> srcRho_;
    edm::EDGetTokenT<pat::METCollection> srcMET_;
    edm::EDGetTokenT<pat::PackedCandidateCollection> srcCandidates_;
    edm::EDGetTokenT<std::vector<PileupSummaryInfo>> srcPU_;
    edm::EDGetTokenT<GenEventInfoProduct> srcGenInfo_;
    edm::EDGetTokenT<reco::GenParticleCollection> srcPrunedGenParticles_;
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
    int   nJetsAK4_, nGenJetsAK4_;
    float rho_, met_, metSig_, metSigreco_, offMet_, offMetSig_, mhtAK4_,
        mhtAK4Sig_;
    float htAK4_, mjjAK4_, dEtajjAK4_, dPhijjAK4_;
    std::vector<bool> *triggerResult_;

    //---- jet and genJet variables --------------
    std::vector<float> *ptAK4_, *jecAK4_, *etaAK4_, *phiAK4_, *massAK4_,
        *energyAK4_, *areaAK4_, *csvAK4_, *chfAK4_, *nhfAK4_, *phfAK4_,
        *elfAK4_, *mufAK4_, *nemfAK4_, *cemfAK4_;
    std::vector<int> *idLAK4_, *idTAK4_, *chHadMultAK4_, *chMultAK4_,
        *neHadMultAK4_, *neMultAK4_, *phoMultAK4_;
    std::vector<float> *hf_hfAK4_, *hf_emfAK4_, *hofAK4_;
    std::vector<float> *ptGenAK4_, *etaGenAK4_, *phiGenAK4_, *massGenAK4_,
        *energyGenAK4_;

    //---- MC variables ---------------
    std::vector<float> *npu_;
    std::vector<int> *Number_interactions;
    std::vector <int> *OriginBX;

    //-----gen particles from hard scattering ------
    float ptHat_;
    int processID_;
    float weight_;
    std::vector<float> *gen_eta, *gen_phi, *gen_p, *gen_px, *gen_py, *gen_pz,
        *gen_pt, *gen_energy,  *gen_vx, *gen_vy, *gen_vz;
    std::vector<int> *gen_numDaught, *gen_status, *gen_index, *gen_motherIndex,
        *gen_pdgId;
};

#endif
