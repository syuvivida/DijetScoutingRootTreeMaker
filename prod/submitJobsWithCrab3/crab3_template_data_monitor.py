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
config.Data.secondaryInputDataset = 'SECONDARYDATASET'
config.Data.unitsPerJob = LUMISPERJOB #without '' since it must be an int
config.Data.splitting = 'LumiBased'
config.Data.publication = False
config.Data.lumiMask = 'https://cms-service-dqm.web.cern.ch/cms-service-dqm/CAF/certification/Collisions15/13TeV/DCSOnly/json_DCSONLY.txt' #if you downloaded the file in the working directory
config.Data.outLFNDirBase = '/store/group/phys_exotica/dijet/Dijet13TeVScouting/rootTrees_big/OUTPUTFOLDER/' #keep this last string (with capital letters) at the end of every path (it's overwritten by the submission script)
config.section_('User')
config.section_('Site')
config.Site.storageSite = 'T2_CH_CERN'
