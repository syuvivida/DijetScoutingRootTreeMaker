#! /usr/bin/env python

import os
import sys
import argparse
import datetime
import re

usage = "usage: python createAndSubmitCrab.py -d Output_ScoutingPFHT/ -v Run2015D_ScoutingPFHT -i Inputs_ScoutingPFHT/InputList_Run2015D_ScoutingPFHT.txt -t Inputs_MC2014_Test/crab3_template.py -c ../flat-data_cfg.py --submit"
print usage

parser = argparse.ArgumentParser(description='Process options.')

parser.add_argument("-i", "--inputList", type=str, dest="inputList", default="",
        help="input datasets with global tag",
	    )
parser.add_argument("-d", "--storageDir", type=str, dest="storagedir", default="",
        help="working directory",
	    )
parser.add_argument("-t", type=str, dest="template_crab", default="",
        help="template of crab configuration file",
	    )
parser.add_argument("-c",  type=str, dest="template_cmssw", default="",
        help="template of cmssw file to execute",
	    )
parser.add_argument("-p",  type=str, dest="publish_name", default="",
        help="name for publication",
	    )
parser.add_argument("-v",  type=str, dest="tagname", default="",
        help="template of cmssw file to execute",
	    )
parser.add_argument("-n",  type=str, dest="username", default="",
        help="username",
	    )
parser.add_argument("--submit",  action="store_true", dest="submit", default=False,
        help="submit jobs with CRAB",
	    )


args = parser.parse_args()
print args

inputList = args.inputList
storagedir = args.storagedir
template_crab = args.template_crab
template_cmssw = args.template_cmssw
publish_name = args.publish_name
tagname = args.tagname
username = args.username
submit = args.submit
#######################

#read input file
ins = open(args.inputList,"r")
#datasets = []
#list_processedevents = []
#list_lumisperjob = []
#globaltags = []

current_time = datetime.datetime.now()
namedir = tagname+"_%04d%02d%02d_%02d%02d%02d" % (current_time.year,current_time.month,current_time.day,current_time.hour,current_time.minute,current_time.second)
os.system("mkdir -p "+storagedir)
os.system("mkdir "+storagedir+"/"+namedir)
os.system("mkdir "+storagedir+"/"+namedir+"/cfg")
os.system("mkdir "+storagedir+"/"+namedir+"/workdir")


for line in ins:
    dataset = line.split()[0]
    processedevents = line.split()[1]
    lumisperjob = line.split()[2]
    globaltag = line.split()[3]
    globaltag = globaltag.strip()
    secondarydataset = ""
    if len(line.split()) > 4:
        secondarydataset = line.split()[4]

    #datasets.append(dataset)
    #list_processedevents.append(processedevents)
    #list_lumisperjob.append(lumisperjob)
    #globaltags.append(globaltag)

    if (line.startswith("#")):
        continue

    print "line : "+line
    print "dataset : "+dataset
    print "processedevents : "+processedevents
    print "lumisperjob : "+lumisperjob
    print "globaltag : "+globaltag
    if secondarydataset != "":
        print "secondarydataset: "+secondarydataset

    sample = dataset.split("/")[1]
    print sample


    dict = {
        "THISROOTFILE":"\""+sample+"__"+dataset.split("/")[2]+"__"+dataset.split("/")[3]+".root"+"\"",
        "THISGLOBALTAG":"\""+globaltag+"\"",
        "LUMISPERJOB":lumisperjob,
        "OUTPUTFOLDER":namedir,
        "WORKINGAREA":storagedir+"/"+namedir+"/workdir",
        "WORKINGDIR":sample+"__"+dataset.split("/")[2]+"__"+dataset.split("/")[3],
        "CMSSWCFG":storagedir+"/"+namedir+"/cfg/"+sample+"_cmssw.py",
        "OUTFILENAME":sample+"__"+dataset.split("/")[2]+"__"+dataset.split("/")[3]+".root",
        "INPUTDATASET":dataset
      }
    if secondarydataset != "":
        dict["SECONDARYDATASET"] = secondarydataset
    ##create cmssw configuration file
    cmssw_cfgfile = storagedir+"/"+namedir+"/cfg/"+sample+"_cmssw.py"
    with open(cmssw_cfgfile, "wt") as fout:
        with open(template_cmssw, "rt") as fin:
            for line_ in fin:
	        #fout.write(line_)
	        #line_=line_.strip() #Francesco (keep same python file indentation)
                for k,v in dict.items():
                    line_ = re.sub(k,v,line_)
                fout.write(line_)


    ##create crab 3 configuration file
    crab_cfgfile = storagedir+"/"+namedir+"/cfg/"+sample+"_crab.py"
    with open(crab_cfgfile, "wt") as fout:
        with open(template_crab, "rt") as fin:
            for line_ in fin:
                line_=line_.strip()
                for k,v in dict.items():
                    line_ = re.sub(k,v,line_)
                fout.write(line_+"\n")

    if submit:
        print "Submitting jobs....."
        #print crab_cfgfile
        os.system("crab submit -c "+crab_cfgfile)


