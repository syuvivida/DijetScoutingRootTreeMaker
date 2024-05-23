import FWCore.ParameterSet.Config as cms

process = cms.Process('jetToolbox')

process.load('PhysicsTools.PatAlgos.producersLayer1.patCandidates_cff')
process.load('Configuration.EventContent.EventContent_cff')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff')

## ----------------- test GT and outName -----------------
testGT = "140X_dataRun3_HLT_v3"
testOutName = "dijetNtuple.root"

## ----------------- Global Tag -----------------
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
#from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag.globaltag = "THISGLOBALTAG"
#process.GlobalTag = GlobalTag(process.GlobalTag, testGT, '')

#--------------------- Report and output ---------------------------   

#process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(527953))
process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(-1))

process.load('FWCore.MessageService.MessageLogger_cfi')
process.MessageLogger.cerr.FwkReport.reportEvery = 1000

process.TFileService=cms.Service("TFileService",
                                 fileName=cms.string("THISROOTFILE"),
                                 closeFileFast = cms.untracked.bool(True)
                                 )

process.options = cms.untracked.PSet(
        allowUnscheduled = cms.untracked.bool(True),
        wantSummary = cms.untracked.bool(False),
        TryToContinue = cms.untracked.vstring('ProductNotFound')
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
    process.GlobalTag.globaltag = "140X_dataRun3_HLT_v3"
#    process.GlobalTag = GlobalTag(process.GlobalTag, testGT, '')
    process.TFileService.fileName = cms.string(testOutName)


##-------------------- Define the source  ----------------------------

process.source = cms.Source(
    "PoolSource",
    fileNames = cms.untracked.vstring(
        # from /ScoutingPFRun3/Run2024D-v1/HLTSCOUT
#        '/store/data/Run2024D/ScoutingPFRun3/HLTSCOUT/v1/000/380/812/00000/c45f8053-4eb9-4874-8555-b794f4dee8b5.root'
        '/store/data/Run2024D/ScoutingPFRun3/HLTSCOUT/v1/000/380/470/00001/f5bfccdb-051f-4bf9-bffa-7999a81a445f.root'
        # from /ScoutingPFRun3/Run2022C-v1/RAW
#         '/store/data/Run2022C/ScoutingPFRun3/RAW/v1/000/357/479/00000/9f6a87ef-50db-4fc6-8b5f-88c72d51d434.root'
        # from /ScoutingPFMonitor/Run2024D-v1/RAW, /ScoutingPFMonitor/Run2024D-PromptReco-v1/MINIAOD
#        '/store/data/Run2024D/ScoutingPFMonitor/MINIAOD/PromptReco-v1/000/380/470/00000/58cd27e9-492b-4b9a-a42e-7c61cb700f47.root'
    )
)

##--- l1 stage2 digis ---
process.load("EventFilter.L1TRawToDigi.gtStage2Digis_cfi")
process.gtStage2Digis.InputLabel = cms.InputTag( "hltFEDSelectorL1" )


##-------------------- User analyzer  --------------------------------
#import trigger conf
from CMSDIJET.DijetScoutingRootTreeMaker.TriggerPaths_cfi import getHLTConf, getL1Conf

process.dijetscouting = cms.EDAnalyzer(
    'DijetScoutingTreeProducer',
    ### Print more information
    debug      = cms.bool(False),    
    ## JETS/MET ########################################
    jetsAK4    = cms.InputTag('hltScoutingPFPacker'),
    ptMinAK4   = cms.double(10),
    rho        = cms.InputTag('hltScoutingPFPacker:rho'),
    met        = cms.InputTag('hltScoutingPFPacker:pfMetPt'),
    vtx        = cms.InputTag('hltScoutingPrimaryVertexPacker:primaryVtx'),
    candidates = cms.InputTag('hltScoutingPFPacker'),
    # ParkingScoutingMonitor
    doRECO     = cms.bool(False),
    doCalo     = cms.bool(False),

    ## trigger ###################################
    triggerAlias = cms.vstring(getHLTConf(0)),
    triggerSelection = cms.vstring(getHLTConf(1)),
    triggerDuplicates = cms.vint32(getHLTConf(2)),

    triggerConfiguration = cms.PSet(
        hltResults            = cms.InputTag('TriggerResults','','HLT'),
        l1tResults            = cms.InputTag(''),
        daqPartitions         = cms.uint32(1),
        ###
        usePathStatus         = cms.bool( True ),
        l1tIgnoreMaskAndPrescale = cms.bool( False ),
        ###
        l1tIgnoreMask         = cms.bool(False),
        l1techIgnorePrescales = cms.bool(False),
        throw                 = cms.bool(False)
    ),

    ## JECs ################
    doJECs = cms.bool(True),

    L1corrAK4_DATA = cms.FileInPath('CMSDIJET/DijetScoutingRootTreeMaker/data/Summer22_22Sep2023_RunCD_V2_DATA/Summer22_22Sep2023_RunCD_V2_DATA_L1FastJet_AK4PFPuppi.txt'),
    L2corrAK4_DATA = cms.FileInPath('CMSDIJET/DijetScoutingRootTreeMaker/data/Summer22_22Sep2023_RunCD_V2_DATA/Summer22_22Sep2023_RunCD_V2_DATA_L2Relative_AK4PFPuppi.txt'),
    L3corrAK4_DATA = cms.FileInPath('CMSDIJET/DijetScoutingRootTreeMaker/data/Summer22_22Sep2023_RunCD_V2_DATA/Summer22_22Sep2023_RunCD_V2_DATA_L3Absolute_AK4PFPuppi.txt'),
    ResCorrAK4_DATA = cms.FileInPath('CMSDIJET/DijetScoutingRootTreeMaker/data/Summer22_22Sep2023_RunCD_V2_DATA/Summer22_22Sep2023_RunCD_V2_DATA_L2L3Residual_AK4PFPuppi.txt'),

    ## L1 trigger info ################
    ## only available for PFMonitoring dataset
    doL1 = cms.bool(False),
    AlgInputTag = cms.InputTag("gtStage2Digis"),
    l1tAlgBlkInputTag = cms.InputTag("gtStage2Digis"),
    l1tExtBlkInputTag = cms.InputTag("gtStage2Digis"),
    ReadPrescalesFromFile = cms.bool(False), # added by Eiko
    l1Seeds = cms.vstring(getL1Conf())
)


# ------------------ path --------------------------

process.p = cms.Path(process.dijetscouting)
