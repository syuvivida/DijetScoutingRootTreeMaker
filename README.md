DijetScoutingRootTreeMaker
==========================

Information: https://twiki.cern.ch/twiki/bin/viewauth/CMS/ExoDijet13TeV#Analysis_code_data_scouting_anal

Setup release:

```
cmsrel CMSSW_8_0_8
cd CMSSW_8_0_8/src/
cmsenv
```

Clone and compile repository:

```
git clone https://github.com/CMSDIJET/DijetScoutingRootTreeMaker.git CMSDIJET/DijetScoutingRootTreeMaker
scram b -j4
```

Edit the configuration file `CMSDIJET/DijetScoutingRootTreeMaker/prod/flat-data_cfg.py` and then run with

```
cd CMSDIJET/DijetScoutingRootTreeMaker/prod/
cmsRun flat-data_cfg.py
```

Use the configuration file `flat-data-monitor_cfg.py` to run over the ParkingScoutingMonitor datasets.
Use the configuration file `flat-data-calo_cfg.py` to run over the CaloScouting datasets.




In order to keep running in 74x replace the instructions above with:

```
cmsrel CMSSW_7_4_15
cd CMSSW_7_4_15/src
cmsenv
git clone https://github.com/CMSDIJET/DijetScoutingRootTreeMaker.git CMSDIJET/DijetScoutingRootTreeMaker
git fetch origin branch_74x:branch_74x
git checkout branch_74x
scram b -j4
```
