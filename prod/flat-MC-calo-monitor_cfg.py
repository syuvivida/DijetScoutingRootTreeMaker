import FWCore.ParameterSet.Config as cms

process = cms.Process('jetToolbox')

process.load('PhysicsTools.PatAlgos.producersLayer1.patCandidates_cff')
process.load('Configuration.EventContent.EventContent_cff')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff')

## ----------------- test GT and outName ------------------
testGT = "92X_dataRun2_HLT_v7"
testOutName = "dijetMonitoringNtuple_M500.root"

## ----------------- Global Tag ------------------
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
process.GlobalTag.globaltag = "THISGLOBALTAG"

#--------------------- Report and output ---------------------------

process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(1000))

process.load('FWCore.MessageService.MessageLogger_cfi')
process.MessageLogger.cerr.FwkReport.reportEvery = 100

process.TFileService=cms.Service("TFileService",
                                 fileName=cms.string("THISROOTFILE"),
                                 closeFileFast = cms.untracked.bool(True)
                                 )

process.options = cms.untracked.PSet(
        allowUnscheduled = cms.untracked.bool(True),
        wantSummary = cms.untracked.bool(False),
)

## ---------------- Interactive testing-----------------
import FWCore.ParameterSet.VarParsing as VarParsing
variables = VarParsing.VarParsing('analysis')
variables.register('local',
                   False,
                   VarParsing.VarParsing.multiplicity.singleton,
                   VarParsing.VarParsing.varType.bool,
                   "Local running")

variables.parseArguments()

if variables.local == True:
    process.GlobalTag.globaltag = testGT
    process.TFileService.fileName = cms.string(testOutName)


##-------------------- Define the source  ----------------------------

process.source = cms.Source(
    "PoolSource",
    fileNames = cms.untracked.vstring(
        #'file:ZEE_13TeV_TuneCUETP8M1_miniAODSIM.root' #MC with scouting
#        'file:/afs/cern.ch/work/f/fpreiato/private/Scouting_Dijet/CMSSW_9_4_0_patch1/src/RSGravitonToQuarkQuark_splittingJob/SplittedJobs/step3_1_inMINIAODSIM.root'
#        'file:/afs/cern.ch/work/f/fpreiato/private/Scouting_Dijet/CMSSW_9_4_0_patch1/src/RSGravitonToQuarkQuark_splittingJob/SplittedJobs/submit_crab3/step3_2_inMINIAODSIM.root' 
        'file:/afs/cern.ch/work/f/fpreiato/private/Scouting_Dijet/CMSSW_9_4_0_patch1/src/RSGravitonToQuarkQuark_splittingJob/SplittedJobs/Step3/M_500/step3_1_inMINIAODSIM.root', 
        'file:/afs/cern.ch/work/f/fpreiato/private/Scouting_Dijet/CMSSW_9_4_0_patch1/src/RSGravitonToQuarkQuark_splittingJob/SplittedJobs/Step3/M_500/step3_2_inMINIAODSIM.root', 
        'file:/afs/cern.ch/work/f/fpreiato/private/Scouting_Dijet/CMSSW_9_4_0_patch1/src/RSGravitonToQuarkQuark_splittingJob/SplittedJobs/Step3/M_500/step3_3_inMINIAODSIM.root', 
        'file:/afs/cern.ch/work/f/fpreiato/private/Scouting_Dijet/CMSSW_9_4_0_patch1/src/RSGravitonToQuarkQuark_splittingJob/SplittedJobs/Step3/M_500/step3_4_inMINIAODSIM.root', 
        'file:/afs/cern.ch/work/f/fpreiato/private/Scouting_Dijet/CMSSW_9_4_0_patch1/src/RSGravitonToQuarkQuark_splittingJob/SplittedJobs/Step3/M_500/step3_5_inMINIAODSIM.root', 
        'file:/afs/cern.ch/work/f/fpreiato/private/Scouting_Dijet/CMSSW_9_4_0_patch1/src/RSGravitonToQuarkQuark_splittingJob/SplittedJobs/Step3/M_500/step3_6_inMINIAODSIM.root', 
        'file:/afs/cern.ch/work/f/fpreiato/private/Scouting_Dijet/CMSSW_9_4_0_patch1/src/RSGravitonToQuarkQuark_splittingJob/SplittedJobs/Step3/M_500/step3_7_inMINIAODSIM.root', 
        'file:/afs/cern.ch/work/f/fpreiato/private/Scouting_Dijet/CMSSW_9_4_0_patch1/src/RSGravitonToQuarkQuark_splittingJob/SplittedJobs/Step3/M_500/step3_8_inMINIAODSIM.root', 
        'file:/afs/cern.ch/work/f/fpreiato/private/Scouting_Dijet/CMSSW_9_4_0_patch1/src/RSGravitonToQuarkQuark_splittingJob/SplittedJobs/Step3/M_500/step3_9_inMINIAODSIM.root', 
        'file:/afs/cern.ch/work/f/fpreiato/private/Scouting_Dijet/CMSSW_9_4_0_patch1/src/RSGravitonToQuarkQuark_splittingJob/SplittedJobs/Step3/M_500/step3_10_inMINIAODSIM.root' 
    ),
    secondaryFileNames = cms.untracked.vstring(
#        'file:ZEE_13TeV_TuneCUETP8M1_AODSIM.root' #MC with scouting
#        'file:/afs/cern.ch/work/f/fpreiato/private/Scouting_Dijet/CMSSW_9_4_0_patch1/src/RSGravitonToQuarkQuark_splittingJob/SplittedJobs/step3_1.root'
#        'file:/afs/cern.ch/work/f/fpreiato/private/Scouting_Dijet/CMSSW_9_4_0_patch1/src/RSGravitonToQuarkQuark_splittingJob/SplittedJobs/submit_crab3/step3_2.root' 
#        'file:/afs/cern.ch/work/f/fpreiato/private/Scouting_Dijet/CMSSW_9_4_0_patch1/src/RSGravitonToQuarkQuark_splittingJob/SplittedJobs/Step3/step3_10.root' 
        'file:/afs/cern.ch/work/f/fpreiato/private/Scouting_Dijet/CMSSW_9_4_0_patch1/src/RSGravitonToQuarkQuark_splittingJob/SplittedJobs/Step3/M_500/step3_1.root', 
        'file:/afs/cern.ch/work/f/fpreiato/private/Scouting_Dijet/CMSSW_9_4_0_patch1/src/RSGravitonToQuarkQuark_splittingJob/SplittedJobs/Step3/M_500/step3_2.root', 
        'file:/afs/cern.ch/work/f/fpreiato/private/Scouting_Dijet/CMSSW_9_4_0_patch1/src/RSGravitonToQuarkQuark_splittingJob/SplittedJobs/Step3/M_500/step3_3.root', 
        'file:/afs/cern.ch/work/f/fpreiato/private/Scouting_Dijet/CMSSW_9_4_0_patch1/src/RSGravitonToQuarkQuark_splittingJob/SplittedJobs/Step3/M_500/step3_4.root', 
        'file:/afs/cern.ch/work/f/fpreiato/private/Scouting_Dijet/CMSSW_9_4_0_patch1/src/RSGravitonToQuarkQuark_splittingJob/SplittedJobs/Step3/M_500/step3_5.root', 
        'file:/afs/cern.ch/work/f/fpreiato/private/Scouting_Dijet/CMSSW_9_4_0_patch1/src/RSGravitonToQuarkQuark_splittingJob/SplittedJobs/Step3/M_500/step3_6.root', 
        'file:/afs/cern.ch/work/f/fpreiato/private/Scouting_Dijet/CMSSW_9_4_0_patch1/src/RSGravitonToQuarkQuark_splittingJob/SplittedJobs/Step3/M_500/step3_7.root', 
        'file:/afs/cern.ch/work/f/fpreiato/private/Scouting_Dijet/CMSSW_9_4_0_patch1/src/RSGravitonToQuarkQuark_splittingJob/SplittedJobs/Step3/M_500/step3_8.root', 
        'file:/afs/cern.ch/work/f/fpreiato/private/Scouting_Dijet/CMSSW_9_4_0_patch1/src/RSGravitonToQuarkQuark_splittingJob/SplittedJobs/Step3/M_500/step3_9.root', 
        'file:/afs/cern.ch/work/f/fpreiato/private/Scouting_Dijet/CMSSW_9_4_0_patch1/src/RSGravitonToQuarkQuark_splittingJob/SplittedJobs/Step3/M_500/step3_10.root'
       )
    )

##------------------- l1 stage2 digis ------------------------------
process.load("EventFilter.L1TRawToDigi.gtStage2Digis_cfi")
process.gtStage2Digis.InputLabel = cms.InputTag( "hltFEDSelectorL1" )

##-------------------- User analyzer  --------------------------------
#import trigger conf
from CMSDIJET.DijetScoutingRootTreeMaker.TriggerPaths_cfi import getHLTConf, getL1Conf

process.dijetscouting = cms.EDAnalyzer(
    'DijetCaloScoutingTreeProducer',
    ## JETS/MET ########################################
    jetsAK4     = cms.InputTag('hltScoutingCaloPacker'),
    ptMinAK4    = cms.double(10),
    rho         = cms.InputTag('hltScoutingCaloPacker:rho'),
    met         = cms.InputTag('hltScoutingCaloPacker:caloMetPt'),
#    vtx         = cms.InputTag('hltScoutingCaloPacker'),
    vtx        = cms.InputTag('hltScoutingPrimaryVertexPacker:primaryVtx'),
    candidates  = cms.InputTag('hltScoutingCaloPacker'),
    # ParkingScoutingMonitor
    doRECO      = cms.bool(True),
    jetsAK4reco = cms.InputTag('slimmedJets'),
    rhoreco     = cms.InputTag('fixedGridRhoFastjetAll'),
    metreco     = cms.InputTag('slimmedMETs'),
    vtxreco     = cms.InputTag('offlineSlimmedPrimaryVertices'),

    ## trigger ###################################
    triggerAlias = cms.vstring(getHLTConf(0)),
    triggerSelection = cms.vstring(getHLTConf(1)),
    triggerDuplicates = cms.vint32(getHLTConf(2)),

    triggerConfiguration = cms.PSet(
        hltResults            = cms.InputTag('TriggerResults','','HLT'),
        l1tResults            = cms.InputTag(''),
        daqPartitions         = cms.uint32(1),
#        l1tIgnoreMask         = cms.bool(False),
#        l1techIgnorePrescales = cms.bool(False),
        l1tIgnoreMaskAndPrescale = cms.bool(False), # added
        throw                 = cms.bool(False)
    ),

    ## JECs ################
    doJECs = cms.bool(True),

    L1corrAK4_DATA = cms.FileInPath('CMSDIJET/DijetScoutingRootTreeMaker/data/80X_dataRun2_HLT_v12/80X_dataRun2_HLT_v12_L1FastJet_AK4CaloHLT.txt'),
    L2corrAK4_DATA = cms.FileInPath('CMSDIJET/DijetScoutingRootTreeMaker/data/80X_dataRun2_HLT_v12/80X_dataRun2_HLT_v12_L2Relative_AK4CaloHLT.txt'),
    L3corrAK4_DATA = cms.FileInPath('CMSDIJET/DijetScoutingRootTreeMaker/data/80X_dataRun2_HLT_v12/80X_dataRun2_HLT_v12_L3Absolute_AK4CaloHLT.txt'),

    L1corrAK4reco_DATA = cms.FileInPath('CMSDIJET/DijetScoutingRootTreeMaker/data/Summer16_07Aug2017_V2_MC/Summer16_07Aug2017_V2_MC_L1FastJet_AK4PFchs.txt'),
    L2corrAK4reco_DATA = cms.FileInPath('CMSDIJET/DijetScoutingRootTreeMaker/data/Summer16_07Aug2017_V2_MC/Summer16_07Aug2017_V2_MC_L2Relative_AK4PFchs.txt'),
    L3corrAK4reco_DATA = cms.FileInPath('CMSDIJET/DijetScoutingRootTreeMaker/data/Summer16_07Aug2017_V2_MC/Summer16_07Aug2017_V2_MC_L3Absolute_AK4PFchs.txt'),

    #L1 trigger info
    doL1 = cms.bool(True),
    AlgInputTag = cms.InputTag("gtStage2Digis"),
    l1tAlgBlkInputTag = cms.InputTag("gtStage2Digis"),
    l1tExtBlkInputTag = cms.InputTag("gtStage2Digis"),
    ReadPrescalesFromFile = cms.bool(False), # added
    l1Seeds = cms.vstring(getL1Conf())
)

# ------------------ path --------------------------

process.p = cms.Path(process.dijetscouting)
