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
    triggerCache_(triggerExpression::Data(
                      cfg.getParameterSet("triggerConfiguration"),
                      consumesCollector())),
    vtriggerAlias_(cfg.getParameter<vector<string>>("triggerAlias")),
    vtriggerSelection_(cfg.getParameter<vector<string>>("triggerSelection"))
{
    if (vtriggerAlias_.size() != vtriggerSelection_.size()) {
        cout << "ERROR: The number of trigger aliases does not match the number of trigger names!!!"
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
    for(unsigned i=0; i<vtriggerSelection_.size(); ++i) {
        triggerNamesHisto_->Fill(vtriggerSelection_[i].c_str(), 1);
    }
    triggerPassHisto_ = fs_->make<TH1F>("TriggerPass", "TriggerPass", 1, 0, 1);
    triggerPassHisto_->SetBit(TH1::kCanRebin);

    //--- book the tree -----------------------
    outTree_ = fs_->make<TTree>("events","events");
    outTree_->Branch("runNo",  &run_,    "run_/I");
    outTree_->Branch("evtNo",  &evt_,    "evt_/I");
    outTree_->Branch("lumi",   &lumi_,   "lumi_/I");
    outTree_->Branch("nvtx",   &nVtx_,   "nVtx_/I");
    outTree_->Branch("rho",    &rho_,    "rho_/F");
    outTree_->Branch("met",    &met_,    "met_/F");
    outTree_->Branch("metSig", &metSig_, "metSig_/F");

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

    //-------------- Event Info -----------------------------------
    rho_  = *rho;
    met_  = *met;
    nVtx_ = vertices->size();
    run_  = iEvent.id().run();
    evt_  = iEvent.id().event();
    lumi_ = iEvent.id().luminosityBlock();

    double sumEt = 0.0;

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
            triggerResult_->push_back(result);
        }
    }

    //-------------- Jets -----------------------------------------
    vector<double> jecFactorsAK4;
    vector<unsigned> sortedAK4JetIdx;
    if(doJECs_) {
        // Sort AK4 jets by increasing pT
        multimap<double, unsigned> sortedAK4Jets;
        for(ScoutingPFJetCollection::const_iterator ijet=jetsAK4->begin();
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
             it!=sortedAK4Jets.rend(); ++it)
            sortedAK4JetIdx.push_back(it->second);
    } else {
        for(ScoutingPFJetCollection::const_iterator ijet=jetsAK4->begin();
            ijet!=jetsAK4->end(); ++ijet) {
            jecFactorsAK4.push_back(1.0);
            sortedAK4JetIdx.push_back(ijet - jetsAK4->begin());
        }
    }

    nJetsAK4_ = 0;
    float htAK4 = 0.0;
    vector<TLorentzVector> vP4AK4;
    for (vector<unsigned>::const_iterator i=sortedAK4JetIdx.begin();
         i!=sortedAK4JetIdx.end(); ++i) {
        ScoutingPFJetCollection::const_iterator ijet = (jetsAK4->begin() + *i);
        TLorentzVector jet;
        jet.SetPtEtaPhiM(ijet->pt(), ijet->eta(), ijet->phi(), ijet->m());

        double jet_energy = jet.E();
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
        sumEt += pt;

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

    if (sumEt > 0.0)
        metSig_ = *met/sumEt;

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
}


DEFINE_FWK_MODULE(DijetScoutingTreeProducer);
