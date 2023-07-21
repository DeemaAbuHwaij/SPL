#include "Graph.h"
#include "SelectionPolicy.h"
#include "Simulation.h"
#include <iostream>
using namespace std;

SelectionPolicy::~SelectionPolicy(){}
SelectionPolicy::SelectionPolicy(){}


MandatesSelectionPolicy::MandatesSelectionPolicy(){}
MandatesSelectionPolicy::~MandatesSelectionPolicy(){}


void MandatesSelectionPolicy:: select(Graph &g, Agent &a){ 
    int maxMandate = 0;
    int thePartyId = -1; 
    for(int i=0; i<g.getNumVertices(); i++){
        if(g.getEdgeWeight(a.getPartyId(),i)>0){
            if((g.getMandates(i) > maxMandate) & (g.getParty(i).getState() != Joined)){
                int checkCoalition = a.getcoalitionId();
                if(!(g.getParty2(i).sameOffers(checkCoalition))){
                    thePartyId = i;
                    maxMandate = g.getMandates(i);
                }
            }
        }  
    }
    if(thePartyId != -1){
        g.getParty2(thePartyId).setState(CollectingOffers);
        g.getParty2(thePartyId).recieveOffers(a);
    }
    

};

char MandatesSelectionPolicy::checkType(){
    return 'M';
};


EdgeWeightSelectionPolicy::EdgeWeightSelectionPolicy(){}
EdgeWeightSelectionPolicy::~EdgeWeightSelectionPolicy(){}

void EdgeWeightSelectionPolicy::select(Graph &g, Agent &a){
    int maxWeightEdge = 0;
    int thePartyId = -1; 
    for(int i=0; i<g.getNumVertices(); i++){
        if(g.getEdgeWeight(a.getPartyId(),i)>0){
            if((g.getEdgeWeight(a.getPartyId(), i) > maxWeightEdge) & (g.getParty(i).getState() != Joined)){
                int checkCoalition = a.getcoalitionId();
                if(!(g.getParty2(i).sameOffers(checkCoalition))){
                    thePartyId = i;
                    maxWeightEdge = g.getEdgeWeight(i, a.getPartyId());
                }
            }
        }
    }

    if(thePartyId != -1){
        g.getParty2(thePartyId).setState(CollectingOffers);
        g.getParty2(thePartyId).recieveOffers(a);
    }
};

char EdgeWeightSelectionPolicy::checkType(){
    return 'E';
};

SelectionPolicy* EdgeWeightSelectionPolicy::clone(){
    return new EdgeWeightSelectionPolicy();
}

SelectionPolicy* MandatesSelectionPolicy::clone(){
    return new MandatesSelectionPolicy();
}