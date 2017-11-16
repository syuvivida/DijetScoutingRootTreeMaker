############define here the L1 trigger paths to monitor: Alias, TriggerSelection and Duplicates

L1Info  = ['L1_HTT120er','L1_HTT160er','L1_HTT200er','L1_HTT220er','L1_HTT240er','L1_HTT255er',
           'L1_HTT270er','L1_HTT280er','L1_HTT300er','L1_HTT320er','L1_HTT340er','L1_HTT380er',
           'L1_HTT400er','L1_HTT450er','L1_HTT500er','L1_ZeroBias']

############define here the HLT trigger paths to monitor: Alias, TriggerSelection and Duplicates
           #Scouting
HLTInfo = [['CaloJet40_CaloScouting_PFScouting', 'DST_CaloJet40_CaloScouting_PFScouting_v*',      0],
           ['CaloJet40_CaloBTagScouting',        'DST_CaloJet40_CaloBTagScouting_v*',             0],
           ['CaloJet40_BTagScouting',            'DST_CaloJet40_BTagScouting_v*',                 0],
           ['L1HTT_CaloScouting_PFScouting',     'DST_L1HTT_CaloScouting_PFScouting_v*',          0],
           ['L1HTT_BTagScouting',                'DST_L1HTT_BTagScouting_v*',                     0],
           ['CaloScoutingHT250',                 'DST_HT250_CaloScouting_v*',                     0],
           ['CaloBTagScoutingHT250',             'DST_HT250_CaloBTagScouting_v*',                 0],
           ['BTagScoutingHT410',                 'DST_HT410_BTagScouting_v*',                     0],
           ['PFScoutingHT410',                   'DST_HT410_PFScouting_v*',                       0],
           ['ZeroBias',                          'DST_ZeroBias_*',                                0],
           ['ZeroBias_CaloScouting_PFScouting',  'DST_ZeroBias_CaloScouting_PFScouting_v*',       0],
           ['ZeroBias_BTagScouting',             'DST_ZeroBias_BTagScouting_v*',                  0],
           ['L1DoubleMu_BTagScouting',           'DST_L1DoubleMu_BTagScouting_v*',                0],
           ['L1DoubleMu_CaloScouting_PFScouting','DST_L1DoubleMu_CaloScouting_PFScouting_v*',     0],
           ['DoubleMu3_noVtx',                   'DST_DoubleMu3_noVtx_CaloScouting_v*',           0],
           ['DoubleMu3_noVtx_Monitoring',        'DST_DoubleMu3_noVtx_CaloScouting_Monitoring_v*',0],
           #RECO
           ['PFHT1050',                          'HLT_PFHT1050_v*',                               0],           
           ['PFHT890',                           'HLT_PFHT890_v*',                                0],
           ['PFHT780',                           'HLT_PFHT780_v*',                                0],
           ['PFHT680',                           'HLT_PFHT680_v*',                                0], 
           ['PFHT590',                           'HLT_PFHT590_v*',                                0],
           ['PFHT510',                           'HLT_PFHT510_v*',                                0],
           ['PFHT430',                           'HLT_PFHT430_v*',                                0],
           ['PFHT370',                           'HLT_PFHT370_v*',                                0],
           ['PFHT250',                           'HLT_PFHT250_v*',                                0],
           ['PFHT180',                           'HLT_PFHT180_v*',                                0],
           ['PFJet550',                          'HLT_PFJet550_v*',                               0],
           ['PFJet500',                          'HLT_PFJet500_v*',                               0],
           ['PFJet450',                          'HLT_PFJet450_v*',                               0],
           ['PFJet400',                          'HLT_PFJet400_v*',                               0],
           ['PFJet320',                          'HLT_PFJet320_v*',                               0],
           ['PFJet260',                          'HLT_PFJet260_v*',                               0],
           ['PFJet200',                          'HLT_PFJet200_v*',                               0],
           ['PFJet140',                          'HLT_PFJet140_v*',                               0],
           ['PFJet80',                           'HLT_PFJet80_v*',                                0],
           ['PFJet60',                           'HLT_PFJet60_v*',                                0],
           ['PFJet40',                           'HLT_PFJet40_v*',                                0],
           ['AK8PFJet550',                       'HLT_AK8PFJet550_v*',                            0],
           ['AK8PFJet500',                       'HLT_AK8PFJet500_v*',                            0],
           ['AK8PFJet450',                       'HLT_AK8PFJet450_v*',                            0],
           ['AK8PFJet400',                       'HLT_AK8PFJet400_v*',                            0],
           ['AK8PFJet320',                       'HLT_AK8PFJet320_v*',                            0],
           ['AK8PFJet260',                       'HLT_AK8PFJet260_v*',                            0],
           ['AK8PFJet200',                       'HLT_AK8PFJet200_v*',                            0],
           ['AK8PFJet140',                       'HLT_AK8PFJet140_v*',                            0],
           ['AK8PFJet80',                        'HLT_AK8PFJet80_v*',                             0],
           ['AK8PFJet60',                        'HLT_AK8PFJet60_v*',                             0],
           ['AK8PFJet40',                        'HLT_AK8PFJet40_v*',                             0],
           ['AK8PFJet420TrimMass30',             'HLT_AK8PFJet420_TrimMass30_v*',                 0],
           ['AK8PFJet400TrimMass30',             'HLT_AK8PFJet400_TrimMass30_v*',                 0],
           ['AK8PFJet380TrimMass30',             'HLT_AK8PFJet380_TrimMass30_v*',                 0],
           ['AK8PFJet360TrimMass30',             'HLT_AK8PFJet360_TrimMass30_v*',                 0],
           ['AK8PFHT900TrimMass50',              'HLT_AK8PFHT900_TrimMass50_v*',                  0],
           ['AK8PFHT850TrimMass50',              'HLT_AK8PFHT850_TrimMass50_v*',                  0],
           ['AK8PFHT800TrimMass50',              'HLT_AK8PFHT800_TrimMass50_v*',                  0],
           ['AK8PFHT750TrimMass50',              'HLT_AK8PFHT750_TrimMass50_v*',                  0],
           ['Mu50',                              'HLT_Mu50_v*',                                   0],
           ['CaloJet550NoJetID',                 'HLT_CaloJet550_NoJetID_v*',                     0],
           ['CaloJet500NoJetID',                 'HLT_CaloJet500_NoJetID_v*',                     0],
           ['DiPFJetAve300HFJEC',                'HLT_DiPFJetAve300_HFJEC_v*',                    0],
           ['DiPFJetAve500',                     'HLT_DiPFJetAve500_v*',                          0],
           ]

def getHLTConf(index):
    myList = []
    for path in HLTInfo:
        myList.append(path[index])
    return myList


def getL1Conf():
    return L1Info
