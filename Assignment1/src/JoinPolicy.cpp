#include "JoinPolicy.h"
#include "Party.h"
#include "Simulation.h"
#include "Agent.h"
#include <vector>
#include <iostream>
using namespace std;

using namespace std;

JoinPolicy::JoinPolicy(){}
JoinPolicy::~JoinPolicy(){}

MandatesJoinPolicy::MandatesJoinPolicy(){}
MandatesJoinPolicy::~MandatesJoinPolicy(){}

void MandatesJoinPolicy::join(Simulation &p, Party &pp){
    int max = 0 ;
    int maxIdCoalition, index = 0;
    vector <int> A = p.mandatesSum(pp.getreceivedOffers()); //change to agent
    for(unsigned int i=0; i < A.size(); i++){
        if(A[i] > max){
            max = A[i];
            index = i;
            maxIdCoalition = pp.getreceivedOffers()[i].getcoalitionId();
        }
    }
    p.add(maxIdCoalition,pp.getId()); // add the partyId to the coallition
    Agent toCopy(pp.getreceivedOffers()[index]);//add selection policy
    toCopy.setAgentId(p.getSizeOfAgents());
    toCopy.setPartyId(pp.getId());
    p.addAgent(toCopy);//add to agents vector
};


LastOfferJoinPolicy::LastOfferJoinPolicy(){}
LastOfferJoinPolicy::~LastOfferJoinPolicy(){}


void LastOfferJoinPolicy::join(Simulation &p, Party &pp){
    vector<Agent> ans = pp.getreceivedOffers();
    Agent last = ans[ans.size()-1];

    p.add(last.getcoalitionId(),pp.getId()); // add the partyId to the coallition
    Agent toCopy(last); //add selection policy
    toCopy.setAgentId(p.getSizeOfAgents());
    toCopy.setPartyId(pp.getId());
    p.addAgent(toCopy);//add to agents vector
};



char LastOfferJoinPolicy::checkType(){
    return 'L';
}

char MandatesJoinPolicy::checkType(){
    return 'M';
}

JoinPolicy* MandatesJoinPolicy::clone(){
    return new MandatesJoinPolicy();
}

JoinPolicy* LastOfferJoinPolicy::clone(){
    return new LastOfferJoinPolicy();
}