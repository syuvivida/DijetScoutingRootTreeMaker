import FWCore.ParameterSet.Config as cms 

process = cms.Process('jetToolbox')

process.load('PhysicsTools.PatAlgos.producersLayer1.patCandidates_cff')
process.load('Configuration.EventContent.EventContent_cff')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff')

## ----------------- Global Tag ------------------
#process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')

#process.GlobalTag.globaltag = THISGLOBALTAG


#--------------------- Report and output ---------------------------

process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(100))

process.load('FWCore.MessageService.MessageLogger_cfi')
process.MessageLogger.cerr.FwkReport.reportEvery = 1

THISROOTFILE = "tree.root"
process.TFileService=cms.Service("TFileService",
                                 fileName=cms.string(THISROOTFILE),
                                 closeFileFast = cms.untracked.bool(True)
                                 )

process.options = cms.untracked.PSet(
        allowUnscheduled = cms.untracked.bool(True),
        wantSummary = cms.untracked.bool(False),
)

############## output  edm format ###############
process.out = cms.OutputModule('PoolOutputModule',
                               fileName = cms.untracked.string('jettoolbox.root'),
                               outputCommands = cms.untracked.vstring([
                                                                      'keep *_slimmedJets_*_*',
                                                                      'keep *_slimmedJetsAK8_*_*',
                                                                      ])
                               )

#### NOT RUNNING OUTPUT MODULE ######
# process.endpath = cms.EndPath(process.out)


##-------------------- Define the source  ----------------------------



process.source = cms.Source(
    "PoolSource",
    fileNames = cms.untracked.vstring('root://cmsxrootd-site.fnal.gov//store/user/dgsheffi/ScoutingPFHT/ScoutingPFHT_E037F887-2678-E511-8C8F-02163E0136BE.root')
)



##-------------------- User analyzer  --------------------------------


process.dijetscouting = cms.EDAnalyzer(
    'DijetScoutingTreeProducer',
    ## JETS/MET ########################################
    jetsAK4  = cms.InputTag('hltScoutingPFPacker'),
    ptMinAK4 = cms.double(10),
    rho      = cms.InputTag('hltScoutingPFPacker:rho'),
    met      = cms.InputTag('hltScoutingPFPacker:pfMetPt'),
    vtx      = cms.InputTag('hltScoutingPFPacker'),

    ## JECs ################
    doJECs = cms.bool(False)
)


# ------------------ path --------------------------

process.p = cms.Path(process.dijetscouting)
