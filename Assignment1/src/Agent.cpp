#include "Agent.h"
#include "Simulation.h"
#include <iostream>
using namespace std;




Agent::Agent(int agentId, int partyId, SelectionPolicy *selectionPolicy) : mAgentId(agentId), mPartyId(partyId), mSelectionPolicy(selectionPolicy), coalitionId(-1)
{
    // You can change the implementation of the constructor, but not the signature!

}

Agent::Agent(const Agent& other): mAgentId(other.mAgentId), mPartyId(other.mPartyId), mSelectionPolicy(other.mSelectionPolicy->clone()), coalitionId(other.coalitionId){
 }; // copy constructor

Agent::Agent(Agent&& other): mAgentId(other.mAgentId), mPartyId(other.mPartyId), mSelectionPolicy(other.mSelectionPolicy), coalitionId(other.coalitionId){
    other.mSelectionPolicy = nullptr;
}; // move constructor

const Agent& Agent::operator=(const Agent& other){
    if(this!=&other){
        mAgentId=other.mAgentId;
        mPartyId=other.mPartyId;
        if(mSelectionPolicy)
            delete mSelectionPolicy;
        mSelectionPolicy = other.mSelectionPolicy->clone();
        coalitionId = other.coalitionId;
    }
    return *this;
}; // copy assignment

Agent& Agent::operator=(Agent&& other){
    mAgentId=other.mAgentId;
    mPartyId=other.mPartyId;
    if(mSelectionPolicy)
        delete mSelectionPolicy;
    mSelectionPolicy = other.mSelectionPolicy;
    other.mSelectionPolicy = nullptr;   
    return *this;
}; // move assignment

Agent::~Agent(){
    if(mSelectionPolicy)
        delete mSelectionPolicy;
};//destructor


int Agent::getId() const
{
    return mAgentId;
}

void Agent::setCoalitionId(int Id){
    coalitionId = Id;
}

int Agent::getPartyId() const
{
    return mPartyId;
}

void Agent::step(Simulation &sim)
{
    // TODO: implement this method
    mSelectionPolicy->select(sim.getGraph2(), *this);
}

void Agent::setAgentId(int agentId)
{
    mAgentId=agentId;  
}
void Agent::setPartyId(int partyId)
{
    mPartyId=partyId;
}
int Agent::getcoalitionId (){
    return coalitionId;
}