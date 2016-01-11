#include "CMSDIJET/DijetScoutingRootTreeMaker/plugins/DijetScoutingTreeProducer.h"

using namespace std;
using namespace edm;


DijetScoutingTreeProducer::DijetScoutingTreeProducer(const ParameterSet& cfg):
    doJECs_(cfg.getParameter<bool>("doJECs")),
    ptMinAK4_(cfg.getParameter<double>("ptMinAK4")),
    srcJetsAK4_(consumes<ScoutingPFJetCollection>(
                    cfg.getParameter<InputTag>("jetsAK4"))),
    srcVrtx_(consumes<ScoutingVertexCollection>(
                 cfg.getParameter<InputTag>("vtx"))),
    srcRho_(consumes<double>(cfg.getParameter<InputTag>("rho"))),
    srcMET_(consumes<double>(cfg.getParameter<InputTag>("met"))),
    srcCandidates_(consumes<ScoutingParticleCollection>(
                       cfg.getParameter<InputTag>("candidates"))),
    doRECO_(cfg.getParameter<bool>("doRECO")),
    triggerCache_(triggerExpression::Data(
                      cfg.getParameterSet("triggerConfiguration"),
                      consumesCollector())),
    vtriggerAlias_(cfg.getParameter<vector<string>>("triggerAlias")),
    vtriggerSelection_(cfg.getParameter<vector<string>>("triggerSelection")),
    vtriggerDuplicates_(cfg.getParameter<vector<int>>("triggerDuplicates"))
{
    if (vtriggerAlias_.size() != vtriggerSelection_.size()) {
        cout << "ERROR: The number of trigger aliases does not match the number of trigger names!!!"
             << endl;
        return;
    }
    if (vtriggerDuplicates_.size() != vtriggerSelection_.size()) {
        cout << "ERROR: The size of trigger duplicates vector does not match the number of trigger names."
             << endl;
        return;
    }
    for (unsigned i=0; i<vtriggerSelection_.size(); ++i) {
        vtriggerSelector_.push_back(triggerExpression::parse(
                                        vtriggerSelection_[i]));
    }

    if (doJECs_) {
        L1corrAK4_DATA_ = cfg.getParameter<FileInPath>("L1corrAK4_DATA");
        L2corrAK4_DATA_ = cfg.getParameter<FileInPath>("L2corrAK4_DATA");
        L3corrAK4_DATA_ = cfg.getParameter<FileInPath>("L3corrAK4_DATA");

        L1ParAK4_DATA = new JetCorrectorParameters(L1corrAK4_DATA_.fullPath());
        L2ParAK4_DATA = new JetCorrectorParameters(L2corrAK4_DATA_.fullPath());
        L3ParAK4_DATA = new JetCorrectorParameters(L3corrAK4_DATA_.fullPath());

        vector<JetCorrectorParameters> vParAK4_DATA;
        vParAK4_DATA.push_back(*L1ParAK4_DATA);
        vParAK4_DATA.push_back(*L2ParAK4_DATA);
        vParAK4_DATA.push_back(*L3ParAK4_DATA);

        JetCorrectorAK4_DATA = new FactorizedJetCorrector(vParAK4_DATA);

        if (doRECO_) {
            L1corrAK4reco_DATA_ = cfg.getParameter<FileInPath>("L1corrAK4reco_DATA");
            L2corrAK4reco_DATA_ = cfg.getParameter<FileInPath>("L2corrAK4reco_DATA");
            L3corrAK4reco_DATA_ = cfg.getParameter<FileInPath>("L3corrAK4reco_DATA");
            ResCorrAK4reco_DATA_ = cfg.getParameter<FileInPath>("ResCorrAK4reco_DATA");

            L1ParAK4reco_DATA = new JetCorrectorParameters(L1corrAK4reco_DATA_.fullPath());
            L2ParAK4reco_DATA = new JetCorrectorParameters(L2corrAK4reco_DATA_.fullPath());
            L3ParAK4reco_DATA = new JetCorrectorParameters(L3corrAK4reco_DATA_.fullPath());
            L2L3ResAK4reco_DATA = new JetCorrectorParameters(ResCorrAK4reco_DATA_.fullPath());

            vector<JetCorrectorParameters> vParAK4reco_DATA;
            vParAK4reco_DATA.push_back(*L1ParAK4reco_DATA);
            vParAK4reco_DATA.push_back(*L2ParAK4reco_DATA);
            vParAK4reco_DATA.push_back(*L3ParAK4reco_DATA);
            vParAK4reco_DATA.push_back(*L2L3ResAK4reco_DATA);

            JetCorrectorAK4reco_DATA = new FactorizedJetCorrector(vParAK4reco_DATA);
        }
    }

    if (doRECO_) {
        srcJetsAK4reco_ = consumes<pat::JetCollection>(
            cfg.getParameter<InputTag>("jetsAK4reco"));
        srcVrtxreco_ = consumes<reco::VertexCollection>(
            cfg.getParameter<InputTag>("vtxreco"));
        srcRhoreco_ = consumes<double>(cfg.getParameter<InputTag>("rhoreco"));
        srcMETreco_ = consumes<pat::METCollection>(
            cfg.getParameter<InputTag>("metreco"));
    }
}

DijetScoutingTreeProducer::~DijetScoutingTreeProducer()
{
}


void DijetScoutingTreeProducer::beginJob()
{
    //--- book the trigger histograms ---------
    triggerNamesHisto_ = fs_->make<TH1F>("TriggerNames", "TriggerNames", 1, 0, 1);
    triggerNamesHisto_->SetBit(TH1::kCanRebin);
    for (unsigned i=0; i<vtriggerSelection_.size(); ++i) {
        triggerNamesHisto_->Fill(vtriggerSelection_[i].c_str(), 1);
    }
    triggerPassHisto_ = fs_->make<TH1F>("TriggerPass", "TriggerPass", 1, 0, 1);
    triggerPassHisto_->SetBit(TH1::kCanRebin);
    triggerPassHisto_->Fill("totalEvents", 0.0);
    for (unsigned i=0; i<vtriggerAlias_.size(); ++i) {
        triggerPassHisto_->Fill(vtriggerAlias_[i].c_str(), 0.0);
    }

    //--- book the tree -----------------------
    outTree_ = fs_->make<TTree>("events","events");
    outTree_->Branch("runNo",     &run_,       "run_/I");
    outTree_->Branch("evtNo",     &evt_,       "evt_/I");
    outTree_->Branch("lumi",      &lumi_,      "lumi_/I");
    outTree_->Branch("nvtx",      &nVtx_,      "nVtx_/I");
    outTree_->Branch("rho",       &rho_,       "rho_/F");
    outTree_->Branch("met",       &met_,       "met_/F");
    outTree_->Branch("metSig",    &metSig_,    "metSig_/F");
    outTree_->Branch("offMet",    &offMet_,    "offMet_/F");
    outTree_->Branch("offMetSig", &offMetSig_, "offMetSig_/F");
    outTree_->Branch("mhtAK4",    &mhtAK4_,    "mhtAK4_/F");
    outTree_->Branch("mhtAK4Sig", &mhtAK4Sig_, "mhtAK4Sig_/F");

    outTree_->Branch("nJetsAK4",  &nJetsAK4_,  "nJetsAK4_/I");
    outTree_->Branch("htAK4",     &htAK4_,     "htAK4_/F");
    outTree_->Branch("mjjAK4",    &mjjAK4_,    "mjjAK4_/F");
    outTree_->Branch("dEtajjAK4", &dEtajjAK4_, "dEtajjAK4_/F");
    outTree_->Branch("dPhijjAK4", &dPhijjAK4_, "dPhijjAK4_/F");

    ptAK4_        = new vector<float>;
    jecAK4_       = new vector<float>;
    etaAK4_       = new vector<float>;
    phiAK4_       = new vector<float>;
    massAK4_      = new vector<float>;
    energyAK4_    = new vector<float>;
    areaAK4_      = new vector<float>;
    csvAK4_       = new vector<float>;
    chfAK4_       = new vector<float>;
    nhfAK4_       = new vector<float>;
    phfAK4_       = new vector<float>;
    mufAK4_       = new vector<float>;
    elfAK4_       = new vector<float>;
    nemfAK4_      = new vector<float>;
    cemfAK4_      = new vector<float>;
    // Hadronic forward hadrons
    hf_hfAK4_     = new vector<float>;
    // Hadronic forward electromagnetic fraction
    hf_emfAK4_    = new vector<float>;
    hofAK4_       = new vector<float>;
    idLAK4_       = new vector<int>;
    idTAK4_       = new vector<int>;
    chHadMultAK4_ = new vector<int>;
    chMultAK4_    = new vector<int>;
    neHadMultAK4_ = new vector<int>;
    neMultAK4_    = new vector<int>;
    phoMultAK4_   = new vector<int>;

    outTree_->Branch("jetPtAK4",     "vector<float>", &ptAK4_);
    outTree_->Branch("jetJecAK4",    "vector<float>", &jecAK4_);
    outTree_->Branch("jetEtaAK4",    "vector<float>", &etaAK4_);
    outTree_->Branch("jetPhiAK4",    "vector<float>", &phiAK4_);
    outTree_->Branch("jetMassAK4",   "vector<float>", &massAK4_);
    outTree_->Branch("jetEnergyAK4", "vector<float>", &energyAK4_);
    outTree_->Branch("jetAreaAK4",   "vector<float>", &areaAK4_);
    outTree_->Branch("jetCSVAK4",    "vector<float>", &csvAK4_);
    outTree_->Branch("jetChfAK4",    "vector<float>", &chfAK4_);
    outTree_->Branch("jetNhfAK4",    "vector<float>", &nhfAK4_);
    outTree_->Branch("jetPhfAK4",    "vector<float>", &phfAK4_);
    outTree_->Branch("jetMufAK4",    "vector<float>", &mufAK4_);
    outTree_->Branch("jetElfAK4",    "vector<float>", &elfAK4_);
    outTree_->Branch("jetNemfAK4",   "vector<float>", &nemfAK4_);
    outTree_->Branch("jetCemfAK4",   "vector<float>", &cemfAK4_);
    outTree_->Branch("jetHf_hfAK4",  "vector<float>", &hf_hfAK4_);
    outTree_->Branch("jetHf_emfAK4", "vector<float>", &hf_emfAK4_);
    outTree_->Branch("jetHofAK4",    "vector<float>", &hofAK4_);
    outTree_->Branch("idLAK4",       "vector<int>",   &idLAK4_);
    outTree_->Branch("idTAK4",       "vector<int>",   &idTAK4_);
    outTree_->Branch("chHadMultAK4", "vector<int>",   &chHadMultAK4_);
    outTree_->Branch("chMultAK4",    "vector<int>",   &chMultAK4_);
    outTree_->Branch("neHadMultAK4", "vector<int>",   &neHadMultAK4_);
    outTree_->Branch("neMultAK4",    "vector<int>",   &neMultAK4_);
    outTree_->Branch("phoMultAK4",   "vector<int>",   &phoMultAK4_);

    if (doRECO_) {
        outTree_->Branch("nvtxreco",      &nVtxreco_,      "nVtxreco_/I");
        outTree_->Branch("rhoreco",       &rhoreco_,       "rhoreco_/F");
        outTree_->Branch("metreco",       &metreco_,       "metreco_/F");
        outTree_->Branch("metrecoSig",    &metrecoSig_,    "metrecoSig_/F");
        outTree_->Branch("mhtAK4reco",    &mhtAK4reco_,    "mhtAK4reco_/F");
        outTree_->Branch("mhtAK4recoSig", &mhtAK4recoSig_, "mhtAK4recoSig_/F");
        outTree_->Branch("nJetsAK4reco",  &nJetsAK4reco_,  "nJetsAK4reco_/I");
        outTree_->Branch("htAK4reco",     &htAK4reco_,     "htAK4reco_/F");
        outTree_->Branch("mjjAK4reco",    &mjjAK4reco_,    "mjjAK4reco_/F");
        outTree_->Branch("dEtajjAK4reco", &dEtajjAK4reco_, "dEtajjAK4reco_/F");
        outTree_->Branch("dPhijjAK4reco", &dPhijjAK4reco_, "dPhijjAK4reco_/F");

        ptAK4reco_        = new vector<float>;
        jecAK4reco_       = new vector<float>;
        etaAK4reco_       = new vector<float>;
        phiAK4reco_       = new vector<float>;
        massAK4reco_      = new vector<float>;
        energyAK4reco_    = new vector<float>;
        areaAK4reco_      = new vector<float>;
        csvAK4reco_       = new vector<float>;
        chfAK4reco_       = new vector<float>;
        nhfAK4reco_       = new vector<float>;
        phfAK4reco_       = new vector<float>;
        mufAK4reco_       = new vector<float>;
        elfAK4reco_       = new vector<float>;
        nemfAK4reco_      = new vector<float>;
        cemfAK4reco_      = new vector<float>;
        // Hadronic forward hadrons
        hf_hfAK4reco_     = new vector<float>;
        // Hadronic forward electromagnetic fraction
        hf_emfAK4reco_    = new vector<float>;
        hofAK4reco_       = new vector<float>;
        idLAK4reco_       = new vector<int>;
        idTAK4reco_       = new vector<int>;
        chHadMultAK4reco_ = new vector<int>;
        chMultAK4reco_    = new vector<int>;
        neHadMultAK4reco_ = new vector<int>;
        neMultAK4reco_    = new vector<int>;
        phoMultAK4reco_   = new vector<int>;

        outTree_->Branch("jetPtAK4reco",     "vector<float>", &ptAK4reco_);
        outTree_->Branch("jetJecAK4reco",    "vector<float>", &jecAK4reco_);
        outTree_->Branch("jetEtaAK4reco",    "vector<float>", &etaAK4reco_);
        outTree_->Branch("jetPhiAK4reco",    "vector<float>", &phiAK4reco_);
        outTree_->Branch("jetMassAK4reco",   "vector<float>", &massAK4reco_);
        outTree_->Branch("jetEnergyAK4reco", "vector<float>", &energyAK4reco_);
        outTree_->Branch("jetAreaAK4reco",   "vector<float>", &areaAK4reco_);
        outTree_->Branch("jetCSVAK4reco",    "vector<float>", &csvAK4reco_);
        outTree_->Branch("jetChfAK4reco",    "vector<float>", &chfAK4reco_);
        outTree_->Branch("jetNhfAK4reco",    "vector<float>", &nhfAK4reco_);
        outTree_->Branch("jetPhfAK4reco",    "vector<float>", &phfAK4reco_);
        outTree_->Branch("jetMufAK4reco",    "vector<float>", &mufAK4reco_);
        outTree_->Branch("jetElfAK4reco",    "vector<float>", &elfAK4reco_);
        outTree_->Branch("jetNemfAK4reco",   "vector<float>", &nemfAK4reco_);
        outTree_->Branch("jetCemfAK4reco",   "vector<float>", &cemfAK4reco_);
        outTree_->Branch("jetHf_hfAK4reco",  "vector<float>", &hf_hfAK4reco_);
        outTree_->Branch("jetHf_emfAK4reco", "vector<float>", &hf_emfAK4reco_);
        outTree_->Branch("jetHofAK4reco",    "vector<float>", &hofAK4reco_);
        outTree_->Branch("idLAK4reco",       "vector<int>",   &idLAK4reco_);
        outTree_->Branch("idTAK4reco",       "vector<int>",   &idTAK4reco_);
        outTree_->Branch("chHadMultAK4reco", "vector<int>",   &chHadMultAK4reco_);
        outTree_->Branch("chMultAK4reco",    "vector<int>",   &chMultAK4reco_);
        outTree_->Branch("neHadMultAK4reco", "vector<int>",   &neHadMultAK4reco_);
        outTree_->Branch("neMultAK4reco",    "vector<int>",   &neMultAK4reco_);
        outTree_->Branch("phoMultAK4reco",   "vector<int>",   &phoMultAK4reco_);
    }

    //------------------------------------------------------------------
    triggerResult_ = new vector<bool>;
    outTree_->Branch("triggerResult", "vector<bool>", &triggerResult_);
}


void DijetScoutingTreeProducer::endJob()
{
    delete triggerResult_;

    delete ptAK4_;
    delete jecAK4_;
    delete etaAK4_;
    delete phiAK4_;
    delete massAK4_;
    delete energyAK4_;
    delete areaAK4_;
    delete csvAK4_;
    delete chfAK4_;
    delete nhfAK4_;
    delete phfAK4_;
    delete mufAK4_;
    delete elfAK4_;
    delete nemfAK4_;
    delete cemfAK4_;
    delete hf_hfAK4_;
    delete hf_emfAK4_;
    delete hofAK4_;
    delete idLAK4_;
    delete idTAK4_;
    delete chHadMultAK4_;
    delete chMultAK4_;
    delete neHadMultAK4_;
    delete neMultAK4_;
    delete phoMultAK4_;

    if (doRECO_) {
        delete ptAK4reco_;
        delete jecAK4reco_;
        delete etaAK4reco_;
        delete phiAK4reco_;
        delete massAK4reco_;
        delete energyAK4reco_;
        delete areaAK4reco_;
        delete csvAK4reco_;
        delete chfAK4reco_;
        delete nhfAK4reco_;
        delete phfAK4reco_;
        delete mufAK4reco_;
        delete elfAK4reco_;
        delete nemfAK4reco_;
        delete cemfAK4reco_;
        delete hf_hfAK4reco_;
        delete hf_emfAK4reco_;
        delete hofAK4reco_;
        delete idLAK4reco_;
        delete idTAK4reco_;
        delete chHadMultAK4reco_;
        delete chMultAK4reco_;
        delete neHadMultAK4reco_;
        delete neMultAK4reco_;
        delete phoMultAK4reco_;
    }

    for(unsigned i=0; i<vtriggerSelector_.size(); ++i) {
        delete vtriggerSelector_[i];
    }
}


void DijetScoutingTreeProducer::analyze(const Event& iEvent,
                                        const EventSetup& iSetup)
{
    initialize();

    // Get collections

    Handle<ScoutingPFJetCollection> jetsAK4;
    iEvent.getByToken(srcJetsAK4_, jetsAK4);
    if (!jetsAK4.isValid()) {
        throw Exception(errors::ProductNotFound)
            << "Could not find ScoutingPFJetCollection." << endl;
        return;
    }

    Handle<ScoutingVertexCollection> vertices;
    iEvent.getByToken(srcVrtx_, vertices);
    if (!vertices.isValid()) {
        throw edm::Exception(edm::errors::ProductNotFound)
            << "Could not find ScoutingVertexCollection." << endl;
        return;
    }

    Handle<double> rho;
    iEvent.getByToken(srcRho_, rho);
    if (!rho.isValid()) {
        throw Exception(errors::ProductNotFound)
            << "Could not find rho." << endl;
        return;
    }

    Handle<double> met;
    iEvent.getByToken(srcMET_, met);
    if (!met.isValid()) {
        throw Exception(errors::ProductNotFound)
            << "Could not find met." << endl;
        return;
    }

    Handle<ScoutingParticleCollection> candidates;
    iEvent.getByToken(srcCandidates_, candidates);
    if (!candidates.isValid()) {
        throw edm::Exception(edm::errors::ProductNotFound)
            << "Could not find ScoutingParticleCollection." << endl;
        return;
    }

    Handle<pat::JetCollection> jetsAK4reco;
    Handle<reco::VertexCollection> verticesreco;
    Handle<double> rhoreco;
    Handle<pat::METCollection> metreco;
    if (doRECO_) {
        iEvent.getByToken(srcJetsAK4reco_, jetsAK4reco);
        if (!jetsAK4reco.isValid()) {
            throw Exception(errors::ProductNotFound)
                << "Could not find pat::JetCollection." << endl;
            return;
        }

        iEvent.getByToken(srcVrtxreco_, verticesreco);
        if (!verticesreco.isValid()) {
            throw Exception(errors::ProductNotFound)
                << "Could not find reco::VertexCollection." << endl;
            return;
        }

        iEvent.getByToken(srcRhoreco_, rhoreco);
        if (!rhoreco.isValid()) {
            throw Exception(errors::ProductNotFound)
                << "Could not find fixedGridRhoFastjetAll." << endl;
            return;
        }


        iEvent.getByToken(srcMETreco_, metreco);
        if (!metreco.isValid()) {
            throw Exception(errors::ProductNotFound)
                << "Could not find pat::MetCollection." << endl;
            return;
        }
    }

    //-------------- Event Info -----------------------------------
    rho_  = *rho;
    met_  = *met;
    nVtx_ = vertices->size();
    run_  = iEvent.id().run();
    evt_  = iEvent.id().event();
    lumi_ = iEvent.id().luminosityBlock();

    double sumEt = 0.0;
    TLorentzVector offline_met(0.0, 0.0, 0.0, 0.0);
    for (auto &candidate: *candidates) {
        sumEt += candidate.pt();
        TLorentzVector vector;
        vector.SetPtEtaPhiM(candidate.pt(), candidate.eta(), candidate.phi(),
                            candidate.m());
        offline_met -= vector;
    }
    offMet_ = offline_met.Pt();
    if (sumEt > 0.0) {
        metSig_ = *met/sumEt;
        offMetSig_ = offMet_/sumEt;
    }

    if (doRECO_) {
        rhoreco_ = *rhoreco;
        metreco_ = (*metreco)[0].et();
        if ((*metreco)[0].sumEt() > 0.0) {
            metrecoSig_ = (*metreco)[0].et()/(*metreco)[0].sumEt();
        }
        nVtxreco_ = verticesreco->size();
    }

    //-------------- Trigger Info -----------------------------------
    triggerPassHisto_->Fill("totalEvents", 1);
    if (triggerCache_.setEvent(iEvent, iSetup)) {
        for(unsigned itrig=0; itrig<vtriggerSelector_.size(); ++itrig) {
            bool result = false;
            if (vtriggerSelector_[itrig]) {
                if (triggerCache_.configurationUpdated()) {
                    vtriggerSelector_[itrig]->init(triggerCache_);
                }
                result = (*(vtriggerSelector_[itrig]))(triggerCache_);
            }
            if (result) {
                triggerPassHisto_->Fill(vtriggerAlias_[itrig].c_str(), 1);
            }
            if (!vtriggerDuplicates_[itrig]) {
                triggerResult_->push_back(result);
            } else {
                // If trigger is a duplicate, OR result with previous trigger
                triggerResult_->back() = triggerResult_->back() || result;
            }
        }
    }

    //-------------- Jets -----------------------------------------
    vector<double> jecFactorsAK4;
    vector<unsigned> sortedAK4JetIdx;
    vector<double> jecFactorsAK4reco;
    vector<unsigned> sortedAK4recoJetIdx;
    if (doJECs_) {
        // Sort AK4 jets by increasing pT
        multimap<double, unsigned> sortedAK4Jets;
        for (ScoutingPFJetCollection::const_iterator ijet=jetsAK4->begin();
             ijet!=jetsAK4->end(); ++ijet) {
            double correction = 1.0;
            JetCorrectorAK4_DATA->setJetEta(ijet->eta());
            JetCorrectorAK4_DATA->setJetPt(ijet->pt());
            JetCorrectorAK4_DATA->setJetA(ijet->jetArea());
            JetCorrectorAK4_DATA->setRho(rho_);
            correction = JetCorrectorAK4_DATA->getCorrection();

            jecFactorsAK4.push_back(correction);
            sortedAK4Jets.insert(make_pair(ijet->pt()*correction,
                                           ijet - jetsAK4->begin()));
        }
        // Get jet indices in decreasing pT order
        for (multimap<double, unsigned>::const_reverse_iterator it=sortedAK4Jets.rbegin();
             it!=sortedAK4Jets.rend(); ++it) {
            sortedAK4JetIdx.push_back(it->second);
        }

        if (doRECO_) {
            multimap<double, unsigned> sortedAK4recoJets;
            for (pat::JetCollection::const_iterator ijet=jetsAK4reco->begin();
                 ijet!=jetsAK4reco->end(); ++ijet) {
                double correction = 1.0;
                JetCorrectorAK4reco_DATA->setJetEta(ijet->eta());
                JetCorrectorAK4reco_DATA->setJetPt(ijet->correctedJet(0).pt());
                JetCorrectorAK4reco_DATA->setJetA(ijet->jetArea());
                JetCorrectorAK4reco_DATA->setRho(rhoreco_);
                correction = JetCorrectorAK4reco_DATA->getCorrection();

                jecFactorsAK4reco.push_back(correction);
                sortedAK4recoJets.insert(make_pair(ijet->pt()*correction,
                                                   ijet - jetsAK4reco->begin()));
            }
            for (multimap<double, unsigned>::const_reverse_iterator it=sortedAK4recoJets.rbegin();
                 it!=sortedAK4recoJets.rend(); ++it) {
                sortedAK4recoJetIdx.push_back(it->second);
            }
        }
    } else {
        for (ScoutingPFJetCollection::const_iterator ijet=jetsAK4->begin();
             ijet!=jetsAK4->end(); ++ijet) {
            jecFactorsAK4.push_back(1.0);
            sortedAK4JetIdx.push_back(ijet - jetsAK4->begin());
        }

        if (doRECO_) {
            for (pat::JetCollection::const_iterator ijet=jetsAK4reco->begin();
                 ijet!=jetsAK4reco->end(); ++ijet) {
                jecFactorsAK4reco.push_back(1.0/ijet->jecFactor(0));
                sortedAK4recoJetIdx.push_back(ijet - jetsAK4reco->begin());
            }
        }
    }

    nJetsAK4_ = 0;
    float htAK4 = 0.0;
    TLorentzVector mhtAK4(0.0, 0.0, 0.0, 0.0);
    vector<TLorentzVector> vP4AK4;
    for (vector<unsigned>::const_iterator i=sortedAK4JetIdx.begin();
         i!=sortedAK4JetIdx.end(); ++i) {
        ScoutingPFJetCollection::const_iterator ijet = (jetsAK4->begin() + *i);
        double jet_energy = ijet->photonEnergy() + ijet->chargedHadronEnergy()
                          + ijet->neutralHadronEnergy() + ijet->electronEnergy()
                          + ijet->muonEnergy();

        double chf = ijet->chargedHadronEnergy()/jet_energy;
        double nhf = ijet->neutralHadronEnergy()/jet_energy;
        double phf = ijet->photonEnergy()/jet_energy;
        double elf = ijet->electronEnergy()/jet_energy;
        double muf = ijet->muonEnergy()/jet_energy;

        double hf_hf = ijet->HFHadronEnergy()/jet_energy;
        double hf_emf= ijet->HFEMEnergy()/jet_energy;
        double hof   = ijet->HOEnergy()/jet_energy;

        int chm    = ijet->chargedHadronMultiplicity();

        int chMult = ijet->chargedHadronMultiplicity()
                   + ijet->electronMultiplicity() + ijet->muonMultiplicity();
        int neMult = ijet->neutralHadronMultiplicity()
                   + ijet->photonMultiplicity();
        int npr    = chMult + neMult;

        int chHadMult = chm;
        int neHadMult = ijet->neutralHadronMultiplicity();
        int phoMult = ijet->photonMultiplicity();

        // Juska's added fractions for identical JetID with recommendations
        double nemf = ijet->photonEnergy()/jet_energy;
        double cemf = (ijet->electronEnergy() + ijet->muonEnergy())/jet_energy;
        int NumConst = npr;

        float eta  = ijet->eta();
        float pt   = ijet->pt()*jecFactorsAK4.at(*i);

        // https://twiki.cern.ch/twiki/bin/viewauth/CMS/JetID
        int idL = (nhf < 0.99 && nemf < 0.99 && NumConst > 1 && muf < 0.8)
            && ((fabs(eta) <= 2.4 && chf > 0 && chMult > 0 && cemf < 0.99)
                || fabs(eta) > 2.4);
        int idT = (nhf < 0.90 && nemf < 0.90 && NumConst > 1 && muf < 0.8)
            && ((fabs(eta) <= 2.4 && chf > 0 && chMult > 0 && cemf < 0.90)
                || fabs(eta) > 2.4);

        if (pt > ptMinAK4_) {
            htAK4 += pt;
            TLorentzVector jet;
            jet.SetPtEtaPhiM(ijet->pt()*jecFactorsAK4.at(*i), ijet->eta(),
                             ijet->phi(), ijet->m()*jecFactorsAK4.at(*i));
            mhtAK4 -= jet;
            ++nJetsAK4_;

            vP4AK4.push_back(jet);
            chfAK4_           ->push_back(chf);
            nhfAK4_           ->push_back(nhf);
            phfAK4_           ->push_back(phf);
            elfAK4_           ->push_back(elf);
            mufAK4_           ->push_back(muf);
            nemfAK4_          ->push_back(nemf);
            cemfAK4_          ->push_back(cemf);
            hf_hfAK4_         ->push_back(hf_hf);
            hf_emfAK4_        ->push_back(hf_emf);
            hofAK4_           ->push_back(hof);
            jecAK4_           ->push_back(jecFactorsAK4.at(*i));
            ptAK4_            ->push_back(pt);
            phiAK4_           ->push_back(ijet->phi());
            etaAK4_           ->push_back(ijet->eta());
            massAK4_          ->push_back(ijet->m()*jecFactorsAK4.at(*i));
            energyAK4_        ->push_back(jet_energy*jecFactorsAK4.at(*i));
            areaAK4_          ->push_back(ijet->jetArea());
            csvAK4_           ->push_back(ijet->csv());
            idLAK4_           ->push_back(idL);
            idTAK4_           ->push_back(idT);
            chHadMultAK4_     ->push_back(chHadMult);
            chMultAK4_        ->push_back(chMult);
            neHadMultAK4_     ->push_back(neHadMult);
            neMultAK4_        ->push_back(neMult);
            phoMultAK4_       ->push_back(phoMult);
        }
    }
    htAK4_ = htAK4;
    if (nJetsAK4_ > 1) { // Assuming jets are ordered by pt
        mjjAK4_    = (vP4AK4[0] + vP4AK4[1]).M();
        dEtajjAK4_ = fabs(etaAK4_->at(0) - etaAK4_->at(1));
        dPhijjAK4_ = fabs(deltaPhi(phiAK4_->at(0), phiAK4_->at(1)));
    }

    mhtAK4_ = mhtAK4.Pt();
    if (htAK4 > 0.0)
        mhtAK4Sig_ = mhtAK4_/htAK4;

    if (doRECO_) {
        nJetsAK4reco_ = 0;
        float htAK4reco = 0.0;
        TLorentzVector mhtAK4reco(0.0, 0.0, 0.0, 0.0);
        vector<TLorentzVector> vP4AK4reco;
        for (vector<unsigned>::const_iterator i=sortedAK4recoJetIdx.begin();
             i!=sortedAK4recoJetIdx.end(); ++i) {
            pat::JetCollection::const_iterator ijet = (jetsAK4reco->begin() + *i);
            double chf = ijet->chargedHadronEnergyFraction();
            double nhf = ijet->neutralHadronEnergyFraction();
            double phf = ijet->photonEnergy()/(ijet->jecFactor(0)*ijet->energy());
            double elf = ijet->electronEnergy()/(ijet->jecFactor(0)*ijet->energy());
            double muf = ijet->muonEnergyFraction();

            double hf_hf = ijet->HFHadronEnergyFraction();
            double hf_emf= ijet->HFEMEnergyFraction();
            double hof   = ijet->hoEnergyFraction();

            int chm    = ijet->chargedHadronMultiplicity();

            int chMult = ijet->chargedMultiplicity();
            int neMult = ijet->neutralMultiplicity();
            int npr    = chMult + neMult;

            int chHadMult = chm;
            int neHadMult = ijet->neutralHadronMultiplicity();
            int phoMult = ijet->photonMultiplicity();

            double nemf = ijet->neutralEmEnergyFraction();
            double cemf = ijet->chargedEmEnergyFraction();
            int NumConst = npr;

            float eta  = ijet->eta();
            float pt   = ijet->correctedJet(0).pt()*jecFactorsAK4reco.at(*i);

            int idL = (nhf < 0.99 && nemf < 0.99 && NumConst > 1 && muf < 0.8)
                && ((fabs(eta) <= 2.4 && chf > 0 && chMult > 0 && cemf < 0.99)
                    || fabs(eta) > 2.4);
            int idT = (nhf < 0.90 && nemf < 0.90 && NumConst > 1 && muf < 0.8)
                && ((fabs(eta) <= 2.4 && chf > 0 && chMult > 0 && cemf < 0.90)
                    || fabs(eta) > 2.4);

            if (pt > ptMinAK4_) {
                htAK4reco += pt;
                ++nJetsAK4reco_;

                vP4AK4reco.emplace_back(ijet->correctedJet(0).px()*jecFactorsAK4reco.at(*i),
                                        ijet->correctedJet(0).py()*jecFactorsAK4reco.at(*i),
                                        ijet->correctedJet(0).pz()*jecFactorsAK4reco.at(*i),
                                        ijet->correctedJet(0).energy()*jecFactorsAK4reco.at(*i));
                mhtAK4reco -= vP4AK4reco[nJetsAK4reco_-1];
                chfAK4reco_      ->push_back(chf);
                nhfAK4reco_      ->push_back(nhf);
                phfAK4reco_      ->push_back(phf);
                elfAK4reco_      ->push_back(elf);
                mufAK4reco_      ->push_back(muf);
                nemfAK4reco_     ->push_back(nemf);
                cemfAK4reco_     ->push_back(cemf);
                hf_hfAK4reco_    ->push_back(hf_hf);
                hf_emfAK4reco_   ->push_back(hf_emf);
                hofAK4reco_      ->push_back(hof);
                jecAK4reco_      ->push_back(jecFactorsAK4reco.at(*i));
                ptAK4reco_       ->push_back(pt);
                phiAK4reco_      ->push_back(ijet->phi());
                etaAK4reco_      ->push_back(ijet->eta());
                massAK4reco_     ->push_back(ijet->correctedJet(0).mass()
                                             *jecFactorsAK4reco.at(*i));
                energyAK4reco_   ->push_back(ijet->correctedJet(0).energy()
                                             *jecFactorsAK4reco.at(*i));
                areaAK4reco_     ->push_back(ijet->jetArea());
                csvAK4reco_     ->push_back(ijet->bDiscriminator(
                               "pfCombinedInclusiveSecondaryVertexV2BJetTags"));
                idLAK4reco_      ->push_back(idL);
                idTAK4reco_      ->push_back(idT);
                chHadMultAK4reco_->push_back(chHadMult);
                chMultAK4reco_   ->push_back(chMult);
                neHadMultAK4reco_->push_back(neHadMult);
                neMultAK4reco_   ->push_back(neMult);
                phoMultAK4reco_  ->push_back(phoMult);
            }
        }
        htAK4reco_ = htAK4reco;
        if (nJetsAK4reco_ > 1) { // Assuming jets are ordered by pt
            mjjAK4reco_    = (vP4AK4reco[0] + vP4AK4reco[1]).M();
            dEtajjAK4reco_ = fabs(etaAK4reco_->at(0) - etaAK4reco_->at(1));
            dPhijjAK4reco_ = fabs(deltaPhi(phiAK4reco_->at(0),
                                           phiAK4reco_->at(1)));
        }
        mhtAK4reco_ = mhtAK4reco.Pt();
        if (htAK4reco > 0.0) {
            metrecoSig_ = metreco_/htAK4reco;
            mhtAK4recoSig_ = mhtAK4reco_/htAK4reco;
        }

    }

    //---- Fill Tree ---
    outTree_->Fill();
    //------------------
}


void DijetScoutingTreeProducer::initialize()
{
    run_          = -999;
    evt_          = -999;
    lumi_         = -999;
    nVtx_         = -999;
    rho_          = -999;
    met_          = -999;
    metSig_       = -999;
    offMet_       = -999;
    offMetSig_    = -999;
    mhtAK4_       = -999;
    mhtAK4Sig_    = -999;
    nJetsAK4_     = -999;
    htAK4_        = -999;
    mjjAK4_       = -999;
    dEtajjAK4_    = -999;
    dPhijjAK4_    = -999;
    ptAK4_        ->clear();
    etaAK4_       ->clear();
    phiAK4_       ->clear();
    massAK4_      ->clear();
    energyAK4_    ->clear();
    areaAK4_      ->clear();
    csvAK4_       ->clear();
    chfAK4_       ->clear();
    nhfAK4_       ->clear();
    phfAK4_       ->clear();
    elfAK4_       ->clear();
    mufAK4_       ->clear();
    nemfAK4_      ->clear();
    cemfAK4_      ->clear();
    hf_hfAK4_     ->clear();
    hf_emfAK4_    ->clear();
    hofAK4_       ->clear();
    jecAK4_       ->clear();
    jecAK4_       ->clear();
    idLAK4_       ->clear();
    idTAK4_       ->clear();
    // Juska's fix
    chHadMultAK4_ ->clear();
    chMultAK4_    ->clear();
    neHadMultAK4_ ->clear();
    neMultAK4_    ->clear();
    phoMultAK4_   ->clear();

    if (doRECO_) {
        rhoreco_         = -999;
        metreco_         = -999;
        metrecoSig_      = -999;
        mhtAK4reco_      = -999;
        mhtAK4recoSig_   = -999;
        nJetsAK4reco_    = -999;
        htAK4reco_       = -999;
        mjjAK4reco_      = -999;
        dEtajjAK4reco_   = -999;
        dPhijjAK4reco_   = -999;
        ptAK4reco_       ->clear();
        etaAK4reco_      ->clear();
        phiAK4reco_      ->clear();
        massAK4reco_     ->clear();
        energyAK4reco_   ->clear();
        areaAK4reco_     ->clear();
        csvAK4reco_      ->clear();
        chfAK4reco_      ->clear();
        nhfAK4reco_      ->clear();
        phfAK4reco_      ->clear();
        elfAK4reco_      ->clear();
        mufAK4reco_      ->clear();
        nemfAK4reco_     ->clear();
        cemfAK4reco_     ->clear();
        hf_hfAK4reco_    ->clear();
        hf_emfAK4reco_   ->clear();
        hofAK4reco_      ->clear();
        jecAK4reco_      ->clear();
        jecAK4reco_      ->clear();
        idLAK4reco_      ->clear();
        idTAK4reco_      ->clear();
        chHadMultAK4reco_->clear();
        chMultAK4reco_   ->clear();
        neHadMultAK4reco_->clear();
        neMultAK4reco_   ->clear();
        phoMultAK4reco_  ->clear();
    }

    triggerResult_->clear();
}


DEFINE_FWK_MODULE(DijetScoutingTreeProducer);
