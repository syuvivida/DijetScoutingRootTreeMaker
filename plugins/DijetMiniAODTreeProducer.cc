#include "CMSDIJET/DijetScoutingRootTreeMaker/plugins/DijetMiniAODTreeProducer.h"

using namespace std;
using namespace edm;


DijetMiniAODTreeProducer::DijetMiniAODTreeProducer(const ParameterSet& cfg):
    redoJECs_(cfg.getParameter<bool>("redoJECs")),
    ptMinAK4_(cfg.getParameter<double>("ptMinAK4")),
    srcJetsAK4_(consumes<pat::JetCollection>(
                    cfg.getParameter<InputTag>("jetsAK4"))),
    srcGenJetsAK4_(consumes<View<reco::GenJet>>(
                       cfg.getParameter<InputTag>("genJetsAK4"))),
    srcVrtx_(consumes<reco::VertexCollection>(
                 cfg.getParameter<InputTag>("vtx"))),
    srcRho_(consumes<double>(cfg.getParameter<InputTag>("rho"))),
    srcMET_(consumes<pat::METCollection>(cfg.getParameter<InputTag>("met"))),
    srcCandidates_(consumes<pat::PackedCandidateCollection>(
                       cfg.getParameter<InputTag>("candidates"))),
    srcPU_(consumes<std::vector<PileupSummaryInfo>>(
               cfg.getUntrackedParameter<InputTag>("pu", InputTag("")))),
    srcGenInfo_(consumes<GenEventInfoProduct>(
                    cfg.getUntrackedParameter<InputTag>("ptHat", InputTag()))),
    srcPrunedGenParticles_(consumes<reco::GenParticleCollection>(
                               cfg.getParameter<InputTag>("genParticles"))),
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

    if (redoJECs_) {
        L1corrAK4_DATA_ = cfg.getParameter<FileInPath>("L1corrAK4reco_DATA");
        L2corrAK4_DATA_ = cfg.getParameter<FileInPath>("L2corrAK4reco_DATA");
        L3corrAK4_DATA_ = cfg.getParameter<FileInPath>("L3corrAK4reco_DATA");
        ResCorrAK4_DATA_ = cfg.getParameter<FileInPath>("ResCorrAK4reco_DATA");
        L1corrAK4_MC_ = cfg.getParameter<FileInPath>("L1corrAK4reco_MC");
        L2corrAK4_MC_ = cfg.getParameter<FileInPath>("L2corrAK4reco_MC");
        L3corrAK4_MC_ = cfg.getParameter<FileInPath>("L3corrAK4reco_MC");

        L1ParAK4_DATA = new JetCorrectorParameters(L1corrAK4_DATA_.fullPath());
        L2ParAK4_DATA = new JetCorrectorParameters(L2corrAK4_DATA_.fullPath());
        L3ParAK4_DATA = new JetCorrectorParameters(L3corrAK4_DATA_.fullPath());
        L2L3ResAK4_DATA = new JetCorrectorParameters(ResCorrAK4_DATA_.fullPath());
        L1ParAK4_MC = new JetCorrectorParameters(L1corrAK4_MC_.fullPath());
        L2ParAK4_MC = new JetCorrectorParameters(L2corrAK4_MC_.fullPath());
        L3ParAK4_MC = new JetCorrectorParameters(L3corrAK4_DATA_.fullPath());

        vector<JetCorrectorParameters> vParAK4_DATA;
        vParAK4_DATA.push_back(*L1ParAK4_DATA);
        vParAK4_DATA.push_back(*L2ParAK4_DATA);
        vParAK4_DATA.push_back(*L3ParAK4_DATA);
        vParAK4_DATA.push_back(*L2L3ResAK4_DATA);
        vector<JetCorrectorParameters> vParAK4_MC;
        vParAK4_MC.push_back(*L1ParAK4_MC);
        vParAK4_MC.push_back(*L2ParAK4_MC);
        vParAK4_MC.push_back(*L3ParAK4_MC);

        JetCorrectorAK4_DATA = new FactorizedJetCorrector(vParAK4_DATA);
        JetCorrectorAK4_MC = new FactorizedJetCorrector(vParAK4_MC);
    }
}

DijetMiniAODTreeProducer::~DijetMiniAODTreeProducer()
{
}


void DijetMiniAODTreeProducer::beginJob()
{
    //--- book the trigger histograms ---------
    triggerNamesHisto_ = fs_->make<TH1F>("TriggerNames", "TriggerNames", 1, 0, 1);
    triggerNamesHisto_->SetCanExtend(TH1::kAllAxes);
    for (unsigned i=0; i<vtriggerSelection_.size(); ++i) {
        triggerNamesHisto_->Fill(vtriggerSelection_[i].c_str(), 1);
    }
    triggerPassHisto_ = fs_->make<TH1F>("TriggerPass", "TriggerPass", 1, 0, 1);
    triggerPassHisto_->SetCanExtend(TH1::kAllAxes);
    triggerPassHisto_->Fill("totalEvents", 0.0);
    for (unsigned i=0; i<vtriggerAlias_.size(); ++i) {
        triggerPassHisto_->Fill(vtriggerAlias_[i].c_str(), 0.0);
    }

    //--- book the tree -----------------------
    outTree_ = fs_->make<TTree>("events","events");
    outTree_->Branch("isData",     &isData_,     "isData_/I");
    outTree_->Branch("runNo",      &run_,        "run_/I");
    outTree_->Branch("evtNo",      &evt_,        "evt_/I");
    outTree_->Branch("lumi",       &lumi_,       "lumi_/I");
    outTree_->Branch("nvtx",       &nVtx_,       "nVtx_/I");
    outTree_->Branch("rho",        &rho_,        "rho_/F");
    outTree_->Branch("met",        &met_,        "met_/F");
    outTree_->Branch("metSig",     &metSig_,     "metSig_/F");
    outTree_->Branch("metSigreco", &metSigreco_, "metSigreco_/F");
    outTree_->Branch("offMet",     &offMet_,     "offMet_/F");
    outTree_->Branch("offMetSig",  &offMetSig_,  "offMetSig_/F");
    outTree_->Branch("mhtAK4",     &mhtAK4_,     "mhtAK4_/F");
    outTree_->Branch("mhtAK4Sig",  &mhtAK4Sig_,  "mhtAK4Sig_/F");

    gen_eta         = new std::vector<float>;
    gen_phi         = new std::vector<float>;
    gen_p           = new std::vector<float>;
    gen_px          = new std::vector<float>;
    gen_py          = new std::vector<float>;
    gen_pz          = new std::vector<float>;
    gen_pt          = new std::vector<float>;
    gen_energy      = new std::vector<float>;
    gen_pdgId       = new std::vector<int>;
    gen_vx          = new std::vector<float>;
    gen_vy          = new std::vector<float>;
    gen_vz          = new std::vector<float>;
    gen_numDaught   = new std::vector<int>;
    gen_status      = new std::vector<int>;
    gen_index       = new std::vector<int>;
    gen_motherIndex = new std::vector<int>;

    outTree_->Branch("gen_eta",         "vector<float>", &gen_eta);
    outTree_->Branch("gen_phi",         "vector<float>", &gen_phi);
    outTree_->Branch("gen_p",           "vector<float>", &gen_p);
    outTree_->Branch("gen_px",          "vector<float>", &gen_px);
    outTree_->Branch("gen_py",          "vector<float>", &gen_py);
    outTree_->Branch("gen_pz",          "vector<float>", &gen_pz);
    outTree_->Branch("gen_pt",          "vector<float>", &gen_pt);
    outTree_->Branch("gen_energy",      "vector<float>", &gen_energy);
    outTree_->Branch("gen_pdgId",       "vector<int>",   &gen_pdgId);
    outTree_->Branch("gen_vx",          "vector<float>", &gen_vx);
    outTree_->Branch("gen_vy",          "vector<float>", &gen_vy);
    outTree_->Branch("gen_vz",          "vector<float>", &gen_vz);
    outTree_->Branch("gen_numDaught",   "vector<int>",   &gen_numDaught);
    outTree_->Branch("gen_status",      "vector<int>",   &gen_status);
    outTree_->Branch("gen_index",       "vector<int>",   &gen_index);
    outTree_->Branch("gen_motherIndex", "vector<int>",   &gen_motherIndex);

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

    //------------------------------------------------------------------
    triggerResult_ = new vector<bool>;
    outTree_->Branch("triggerResult", "vector<bool>", &triggerResult_);

    //------------------- MC ---------------------------------
    npu_                = new std::vector<float>;
    Number_interactions = new std::vector<int>;
    OriginBX            = new std::vector<int>;

    outTree_->Branch("npu",                "vector<float>", &npu_ );
    outTree_->Branch("PileupInteractions", "vector<int>",   &Number_interactions );
    outTree_->Branch("PileupOriginBX",     "vector<int>",   &OriginBX );
    outTree_->Branch("ptHat",              &ptHat_,         "ptHat_/F");
    outTree_->Branch("processID",          &processID_,     "processID_/I");
    outTree_->Branch("weight",             &weight_,        "weight_/F");

    outTree_->Branch("nGenJetsAK4",        &nGenJetsAK4_,   "nGenJetsAK4_/I");

    ptGenAK4_     = new std::vector<float>;
    etaGenAK4_    = new std::vector<float>;
    phiGenAK4_    = new std::vector<float>;
    massGenAK4_   = new std::vector<float>;
    energyGenAK4_ = new std::vector<float>;

    outTree_->Branch("jetPtGenAK4",     "vector<float>", &ptGenAK4_);
    outTree_->Branch("jetEtaGenAK4",    "vector<float>", &etaGenAK4_);
    outTree_->Branch("jetPhiGenAK4",    "vector<float>", &phiGenAK4_);
    outTree_->Branch("jetMassGenAK4",   "vector<float>", &massGenAK4_);
    outTree_->Branch("jetEnergyGenAK4", "vector<float>", &energyGenAK4_);
}


void DijetMiniAODTreeProducer::endJob()
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

    for(unsigned i=0; i<vtriggerSelector_.size(); ++i) {
        delete vtriggerSelector_[i];
    }
}


void DijetMiniAODTreeProducer::analyze(const Event& iEvent,
                                        const EventSetup& iSetup)
{
    initialize();

    // Get collections

    Handle<pat::JetCollection> jetsAK4;
    iEvent.getByToken(srcJetsAK4_, jetsAK4);
    if (!jetsAK4.isValid()) {
        throw Exception(errors::ProductNotFound)
            << "Could not find pat::JetCollection." << endl;
        return;
    }

    Handle<View<reco::GenJet>> handle_genJetsAK4;
    if (!iEvent.isRealData())
        iEvent.getByToken(srcGenJetsAK4_, handle_genJetsAK4);

    Handle<reco::VertexCollection> vertices;
    iEvent.getByToken(srcVrtx_, vertices);
    if (!vertices.isValid()) {
        throw edm::Exception(edm::errors::ProductNotFound)
            << "Could not find reco::VertexCollection." << endl;
        return;
    }

    Handle<double> rho;
    iEvent.getByToken(srcRho_, rho);
    if (!rho.isValid()) {
        throw Exception(errors::ProductNotFound)
            << "Could not find rho." << endl;
        return;
    }

    Handle<pat::METCollection> met;
    iEvent.getByToken(srcMET_, met);
    if (!met.isValid()) {
        throw Exception(errors::ProductNotFound)
            << "Could not find pat::MetCollection." << endl;
        return;
    }

    Handle<pat::PackedCandidateCollection> candidates;
    iEvent.getByToken(srcCandidates_, candidates);
    if (!candidates.isValid()) {
        throw edm::Exception(edm::errors::ProductNotFound)
            << "Could not find PackedCollection." << endl;
        return;
    }

    //-------------- Event Info -----------------------------------
    rho_  = *rho;
    met_  = (*met)[0].et();
    nVtx_ = vertices->size();
    isData_ = int(iEvent.isRealData());
    run_  = iEvent.id().run();
    evt_  = iEvent.id().event();
    lumi_ = iEvent.id().luminosityBlock();

    double sumEt = 0.0;
    math::XYZTLorentzVector offline_met(0.0, 0.0, 0.0, 0.0);
    for (auto &candidate: *candidates) {
        sumEt += candidate.pt();
        offline_met -= candidate.p4();
    }
    offMet_ = offline_met.Pt();
    if (sumEt > 0.0) {
        metSig_ = (*met)[0].et()/sumEt;
        offMetSig_ = offMet_/sumEt;
    }
    if ((*met)[0].sumEt() > 0.0) {
        metSigreco_ = (*met)[0].et()/(*met)[0].sumEt();
    }

    //---------- pu -----------------------
    Handle<vector<PileupSummaryInfo>> PupInfo;
    if (!iEvent.isRealData()) {
        iEvent.getByToken(srcPU_, PupInfo);
        if (PupInfo.isValid()) {
            for (vector<PileupSummaryInfo>::const_iterator it = PupInfo->begin();
                 it != PupInfo->end(); ++it ) {
                npu_->push_back(it->getTrueNumInteractions());
                Number_interactions->push_back(it->getPU_NumInteractions());
                OriginBX->push_back(it->getBunchCrossing());
            }
        } else {
            throw Exception(errors::ProductNotFound)
                << "Could not find PileupSummaryInfo." << endl;
            return;
        }
    }// if MC

    //-------------- Gen Event Info -----------------------------------
    if (!iEvent.isRealData()) {
        Handle<GenEventInfoProduct> genEvtInfo;
        iEvent.getByToken(srcGenInfo_, genEvtInfo);
        if (!genEvtInfo.isValid()) {
            LogInfo("GenEvtInfo") << "ERROR: genEvtInfo not valid! " << genEvtInfo;
        }

        if (genEvtInfo.isValid()) {
            LogInfo("GenEvtInfo") << "Successfully obtained " << genEvtInfo;
            ptHat_ = (genEvtInfo->hasBinningValues()
                      ? genEvtInfo->binningValues()[0] : -999.);
            processID_ = genEvtInfo->signalProcessID();
            weight_ = genEvtInfo->weight();
        }

        //------------------ Gen particles hard scattering -------------------
        Handle<reco::GenParticleCollection> prunedGenParticles;
        if (!iEvent.isRealData())
            iEvent.getByToken(srcPrunedGenParticles_, prunedGenParticles);
        if (prunedGenParticles.isValid()) {
            for (reco::GenParticleCollection::const_iterator it = prunedGenParticles->begin();
                 it != prunedGenParticles->end(); ++it ) {
                int idx = distance(prunedGenParticles->begin(), it);

                // fill in all the vectors
                gen_eta->push_back(it->eta());
                gen_phi->push_back(it->phi());
                gen_p->push_back(it->p());
                gen_px->push_back(it->px());
                gen_py->push_back(it->py());
                gen_pz->push_back(it->pz());
                gen_pt->push_back(it->pt());
                gen_energy->push_back(it->energy());
                gen_pdgId->push_back(it->pdgId());
                gen_vx->push_back(it->vx());
                gen_vy->push_back(it->vy());
                gen_vz->push_back(it->vz());
                gen_numDaught->push_back(it->numberOfDaughters());
                gen_status->push_back(it->status());
                gen_index->push_back(idx);

                int midx = -1;

                for (reco::GenParticleCollection::const_iterator mit = prunedGenParticles->begin();
                     mit != prunedGenParticles->end(); ++mit ) {
                    if (it->mother()==&(*mit)) {
                        midx = distance(prunedGenParticles->begin(),mit);
                        break;
                    }
                }
                gen_motherIndex->push_back(midx);
            }//loop over genParticles
        }
    }// if MC

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
    if (redoJECs_) {
        // Sort AK4 jets by increasing pT
        multimap<double, unsigned> sortedAK4Jets;
        for (pat::JetCollection::const_iterator ijet=jetsAK4->begin();
             ijet!=jetsAK4->end(); ++ijet) {
            double correction = 1.0;
            if (iEvent.isRealData()) {
                JetCorrectorAK4_DATA->setJetEta(ijet->eta());
                JetCorrectorAK4_DATA->setJetPt(ijet->correctedJet(0).pt());
                JetCorrectorAK4_DATA->setJetA(ijet->jetArea());
                JetCorrectorAK4_DATA->setRho(rho_);
                correction = JetCorrectorAK4_DATA->getCorrection();
            } else {
                JetCorrectorAK4_MC->setJetEta(ijet->eta());
                JetCorrectorAK4_MC->setJetPt(ijet->correctedJet(0).pt());
                JetCorrectorAK4_MC->setJetA(ijet->jetArea());
                JetCorrectorAK4_MC->setRho(rho_);
                correction = JetCorrectorAK4_MC->getCorrection();
            }

            jecFactorsAK4.push_back(correction);
            sortedAK4Jets.insert(make_pair(ijet->correctedJet(0).pt()*correction,
                                           ijet - jetsAK4->begin()));
        }
        // Get jet indices in decreasing pT order
        for (multimap<double, unsigned>::const_reverse_iterator it=sortedAK4Jets.rbegin();
             it!=sortedAK4Jets.rend(); ++it) {
            sortedAK4JetIdx.push_back(it->second);
        }
    } else {
        for (pat::JetCollection::const_iterator ijet=jetsAK4->begin();
             ijet!=jetsAK4->end(); ++ijet) {
            jecFactorsAK4.push_back(1.0/ijet->jecFactor(0));
            sortedAK4JetIdx.push_back(ijet - jetsAK4->begin());
        }
    }

    nJetsAK4_ = 0;
    float htAK4 = 0.0;
    TLorentzVector mhtAK4(0.0, 0.0, 0.0, 0.0);
    vector<TLorentzVector> vP4AK4;
    for (vector<unsigned>::const_iterator i=sortedAK4JetIdx.begin();
         i!=sortedAK4JetIdx.end(); ++i) {
        pat::JetCollection::const_iterator ijet = (jetsAK4->begin() + *i);
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

        // Juska's added fractions for identical JetID with recommendations
        double nemf = ijet->neutralEmEnergyFraction();
        double cemf = ijet->chargedEmEnergyFraction();
        int NumConst = npr;

        float eta  = ijet->eta();
        float pt   = ijet->correctedJet(0).pt()*jecFactorsAK4.at(*i);

        // https://twiki.cern.ch/twiki/bin/viewauth/CMS/JetID
        int idL = (nhf < 0.99 && nemf < 0.99 && NumConst > 1 && muf < 0.8)
            && ((fabs(eta) <= 2.4 && chf > 0 && chMult > 0 && cemf < 0.99)
                || fabs(eta) > 2.4);
        int idT = (nhf < 0.90 && nemf < 0.90 && NumConst > 1 && muf < 0.8)
            && ((fabs(eta) <= 2.4 && chf > 0 && chMult > 0 && cemf < 0.90)
                || fabs(eta) > 2.4);

        if (pt > ptMinAK4_) {
            htAK4 += pt;
            ++nJetsAK4_;
            vP4AK4.emplace_back(ijet->correctedJet(0).px()*jecFactorsAK4.at(*i),
                                ijet->correctedJet(0).py()*jecFactorsAK4.at(*i),
                                ijet->correctedJet(0).pz()*jecFactorsAK4.at(*i),
                                ijet->correctedJet(0).energy()*jecFactorsAK4.at(*i));
            mhtAK4 -= vP4AK4[nJetsAK4_-1];

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
            massAK4_          ->push_back(ijet->correctedJet(0).mass()
                                          * jecFactorsAK4.at(*i));
            energyAK4_        ->push_back(ijet->correctedJet(0).energy()
                                          * jecFactorsAK4.at(*i));
            areaAK4_          ->push_back(ijet->jetArea());
            csvAK4_           ->push_back(ijet->bDiscriminator(
                               "pfCombinedInclusiveSecondaryVertexV2BJetTags"));
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

    //-------------- Gen Jets Info -----------------------------------
    if (!iEvent.isRealData()) {
        nGenJetsAK4_ = 0;
        vector<TLorentzVector> vP4GenAK4;
        View<reco::GenJet> genJetsAK4 = *handle_genJetsAK4;
        for (View<reco::GenJet>::const_iterator ijet = genJetsAK4.begin();
             ijet != genJetsAK4.end(); ++ijet) {
            float pt   = ijet->pt();
            if (pt > ptMinAK4_) {
                nGenJetsAK4_++;
                vP4GenAK4.push_back(TLorentzVector(ijet->px(), ijet->py(),
                                                   ijet->pz(),ijet->energy()));
                ptGenAK4_    ->push_back(pt);
                phiGenAK4_   ->push_back(ijet->phi());
                etaGenAK4_   ->push_back(ijet->eta());
                massGenAK4_  ->push_back(ijet->mass());
                energyGenAK4_->push_back(ijet->energy());
            }
        }// jet loop
    }

    //---- Fill Tree ---
    outTree_->Fill();
    //------------------
}


void DijetMiniAODTreeProducer::initialize()
{
    isData_       = -999;
    run_          = -999;
    evt_          = -999;
    lumi_         = -999;
    nVtx_         = -999;
    rho_          = -999;
    met_          = -999;
    metSig_       = -999;
    metSigreco_   = -999;
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

    triggerResult_->clear();

    //----- MC -------
    npu_                ->clear();
    Number_interactions ->clear();
    OriginBX            -> clear();

    ptHat_       = -999;
    processID_   = -999;
    weight_      = -999;

    nGenJetsAK4_ = -999;

    ptGenAK4_      ->clear();
    phiGenAK4_     ->clear();
    etaGenAK4_     ->clear();
    massGenAK4_    ->clear();
    energyGenAK4_  ->clear();

    gen_eta        ->clear();
    gen_phi        ->clear();
    gen_p          ->clear();
    gen_px         ->clear();
    gen_py         ->clear();
    gen_pz         ->clear();
    gen_pt         ->clear();
    gen_energy     ->clear();
    gen_pdgId      ->clear();
    gen_vx         ->clear();
    gen_vy         ->clear();
    gen_vz         ->clear();
    gen_numDaught  ->clear();
    gen_status     ->clear();
    gen_index      ->clear();
    gen_motherIndex->clear();
}


DEFINE_FWK_MODULE(DijetMiniAODTreeProducer);
