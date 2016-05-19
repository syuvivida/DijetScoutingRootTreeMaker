#define here the trigger paths to monitor: Alias, TriggerSelection and Duplicates


triggerInfo = [['CaloJet40_CaloScouting_PFScouting', 'DST_CaloJet40_CaloScouting_PFScouting_v*', 0],
               ['L1HTT_CaloScouting_PFScouting',     'DST_L1HTT_CaloScouting_PFScouting_v*',     0],
               ['CaloScoutingHT250',                 'DST_HT250_CaloScouting_v*',                0]]

def getConf(index):
    myList = []
    for path in triggerInfo:
        myList.append(path[index])
    return myList

