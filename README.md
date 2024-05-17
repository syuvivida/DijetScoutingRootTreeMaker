DijetScoutingRootTreeMaker
==========================

To run over data with Scouting content in 140x, setup release:

```
cmsrel CMSSW_14_0_6
cd CMSSW_14_0_6/src/
cmsenv
```

Clone and compile repository:

```
git clone git@github.com:syuvivida/DijetScoutingRootTreeMaker.git
scram b -j4
```

Run with:

```
cd CMSDIJET/DijetScoutingRootTreeMaker/prod/
cmsRun flat-data_cfg.py local=True
```

Use the configuration file `flat-data-monitor_cfg.py` to run over the ParkingScoutingMonitor datasets.
Use the configuration file `flat-data-calo_cfg.py` to run over the CaloScouting datasets.
Use the configuration file `flat-MC-calo_cfg.py` to run over the MC MINIAOD with Scouting content datasets.

To run crab jobs for the 2017 data for example:

```
python createAndSubmitCrab.py -d Output_ScoutingCaloHT -v ScoutingCaloHT_Run2017C-v1_Dec-31-2017 -i Inputs_ScoutingCaloHT/InputList_Run2017C-v1_ScoutingCaloHT.txt -t crab3_template_data.py -c ../flat-data-calo_cfg.py --submit
python createAndSubmitCrab.py -d Output_ScoutingCaloHT -v ScoutingCaloHT_Run2017D-v1_Dec-31-2017 -i Inputs_ScoutingCaloHT/InputList_Run2017D-v1_ScoutingCaloHT.txt -t crab3_template_data.py -c ../flat-data-calo_cfg.py --submit
python createAndSubmitCrab.py -d Output_ScoutingCaloHT -v ScoutingCaloHT_Run2017E-v1_Dec-31-2017 -i Inputs_ScoutingCaloHT/InputList_Run2017E-v1_ScoutingCaloHT.txt -t crab3_template_data.py -c ../flat-data-calo_cfg.py --submit
python createAndSubmitCrab.py -d Output_ScoutingCaloHT -v ScoutingCaloHT_Run2017F-v1_Dec-31-2017 -i Inputs_ScoutingCaloHT/InputList_Run2017F-v1_ScoutingCaloHT.txt -t crab3_template_data.py -c ../flat-data-calo_cfg.py --submit
```

In order to keep running in 80x, setup release:

```
cmsrel CMSSW_8_0_10
cd CMSSW_8_0_10/src/
cmsenv
git clone -b branch_80x https://github.com/CMSDIJET/DijetScoutingRootTreeMaker.git CMSDIJET/DijetScoutingRootTreeMaker
scram b -j4
cd CMSDIJET/DijetScoutingRootTreeMaker/prod/
cmsRun flat-data_cfg.py local=True
```

In order to keep running in 74x replace the instructions above with:

```
cmsrel CMSSW_7_4_15
cd CMSSW_7_4_15/src
cmsenv
git clone https://github.com/CMSDIJET/DijetScoutingRootTreeMaker.git CMSDIJET/DijetScoutingRootTreeMaker
git fetch origin branch_74x:branch_74x
git checkout branch_74x
scram b -j4

cmsRun flat-data_cfg.py
```
