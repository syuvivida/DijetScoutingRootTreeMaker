DijetScoutingRootTreeMaker
==========================

Information: https://twiki.cern.ch/twiki/bin/viewauth/CMS/ExoDijet13TeV#Analysis_code_data_scouting_anal

Setup release:

```
cmsrel CMSSW_7_4_15
cd CMSSW_7_4_15/src/
cmsenv
```

Clone and compile repository:

```
git clone https://github.com/CMSDIJET/DijetScoutingRootTreeMaker.git CMSDIJET/DijetScoutingRootTreeMaker
scram b
```

Edit the configuration file `CMSDIJET/DijetScoutingRootTreeMaker/prod/flat-data_cfg.py` and then run with

```
cd CMSDIJET/DijetScoutingRootTreeMaker/prod/
cmsRun flat-data_cfg.py
```

Use the configuration file `flat-data-monitor_cfg.py` to run over the ParkingScoutingMonitor datasets.
