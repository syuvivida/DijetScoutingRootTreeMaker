from WMCore.Configuration import Configuration
config = Configuration()

config.section_('General')
config.General.transferOutputs = True
config.General.transferLogs = True
config.General.workArea = 'WORKINGAREA'
config.General.requestName = 'WORKINGDIR'

config.section_('JobType')
config.JobType.psetName = 'CMSSWCFG'
config.JobType.pluginName = 'Analysis'
config.JobType.outputFiles = ['OUTFILENAME']

config.section_('Data')
config.Data.inputDataset = 'INPUTDATASET'
config.Data.inputDBS = 'global'
config.Data.unitsPerJob = LUMISPERJOB #without '' since it must be an int
config.Data.splitting = 'LumiBased'
config.Data.publication = False
#config.Data.lumiMask = '/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions16/13TeV/Cert_271036-274421_13TeV_PromptReco_Collisions16_JSON.txt'
#config.Data.lumiMask = 'https://cms-service-dqm.web.cern.ch/cms-service-dqm/CAF/certification/Collisions16/13TeV/DCSOnly/json_DCSONLY.txt'
config.Data.lumiMask = '/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions17/13TeV/Final/Cert_294927-306462_13TeV_PromptReco_Collisions17_JSON.txt'
#config.Data.outLFNDirBase = '/store/group/phys_exotica/dijet/Dijet13TeVScouting/rootTrees_big/2016/OUTPUTFOLDER/' #keep this last string (with capital letters) at the end of every path (it's overwritten by the submission script)
config.Data.outLFNDirBase = '/store/group/lpcjj/Dijet13TeVScouting/rootTrees_big/2017/OUTPUTFOLDER/' #keep this last string (with capital letters) at the end of every path (it's overwritten by the submission script)
config.Data.ignoreLocality = True

config.section_('User')

config.section_('Site')
config.Site.storageSite = 'T3_US_FNALLPC'
#config.Site.whitelist = ['T2_CH_CERN']
