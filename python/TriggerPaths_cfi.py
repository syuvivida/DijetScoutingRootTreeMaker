############define here the L1 trigger paths to monitor: Alias, TriggerSelection and Duplicates

L1Info  = ['L1_HTT200','L1_HTT240','L1_HTT270','L1_HTT280','L1_HTT300','L1_HTT320','L1_ZeroBias']

############define here the HLT trigger paths to monitor: Alias, TriggerSelection and Duplicates
           #Scouting
HLTInfo = [['CaloJet40_CaloScouting_PFScouting', 'DST_CaloJet40_CaloScouting_PFScouting_v*', 0],
           ['L1HTT_CaloScouting_PFScouting',     'DST_L1HTT_CaloScouting_PFScouting_v*',     0],
           ['CaloScoutingHT250',                 'DST_HT250_CaloScouting_v*',                0],
           #RECO
           ['PFHT900',                           'HLT_PFHT900_v*',                           0],
           ['PFHT800',                           'HLT_PFHT800_v*',                           0],
           ['PFHT650',                           'HLT_PFHT650_v*',                           0], 
           ['PFHT600',                           'HLT_PFHT600_v*',                           0],
           ['PFHT475',                           'HLT_PFHT475_v*',                           0],
           ['PFHT400',                           'HLT_PFHT400_v*',                           0],
           ['PFHT350',                           'HLT_PFHT350_v*',                           0],
           ['PFHT300',                           'HLT_PFHT300_v*',                           0],
           ['PFHT250',                           'HLT_PFHT250_v*',                           0],
           ['PFHT200',                           'HLT_PFHT200_v*',                           0],
           ['PFHT650MJJ950',                     'HLT_PFHT650_WideJetMJJ950DEtaJJ1p5_v*',    0],
           ['PFHT650MJJ900',                     'HLT_PFHT650_WideJetMJJ900DEtaJJ1p5_v*',    0],
           ['PFJET500',                          'HLT_PFJet500_v*',                          0],
           ['PFJET450',                          'HLT_PFJet450_v*',                          0],
           ['PFJET200',                          'HLT_PFJet200_v*',                          0],
           ['HT2000',                            'HLT_HT2000_v*',                            0],
           ['HT2500',                            'HLT_HT2500_v*',                            0],
           ['Mu45Eta2p1',                        'HLT_Mu45_eta2p1_v*',                       0],
           ['AK8PFHT700TriMass50',               'HLT_AK8PFHT700_TrimR0p1PT0p03Mass50_v*',   0],
           ['AK8PFJet360TrimMass50',             'HLT_AK8PFJet360_TrimMass30_v*',            0],
           ['CaloJet500NoJetID',                 'HLT_CaloJet500_NoJetID_v*',                0],
           ['DiPFJetAve300HFJEC',                'HLT_DiPFJetAve300_HFJEC_v*',               0],
           ['DiPFJetAve500',                     'HLT_DiPFJetAve500_v*',                     0],
           ['PFHT750FourJetPt50',                'HLT_PFHT750_4JetPt50_v*',                  0],
           ['QuadPFJetVBF',                      'HLT_QuadPFJet_VBF_v*',                     0]]

def getHLTConf(index):
    myList = []
    for path in HLTInfo:
        myList.append(path[index])
    return myList


def getL1Conf():
    return L1Info
