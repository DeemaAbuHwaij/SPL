#include "Party.h"
#include <vector>
#include "Agent.h"
#include "Graph.h"
#include "JoinPolicy.h"
#include <iostream>
#include "Simulation.h"

using namespace std;

Party::Party(int id, string name, int mandates, JoinPolicy *jp) : mId(id), mName(name), mMandates(mandates), mJoinPolicy(jp), mState(Waiting), timer(0), coalitionId(-1), receivedOffers(vector<Agent>())
{
    // You can change the implementation of the constructor, but not the signature!
}
Party::Party(const Party& other):mId(other.mId), mName(other.mName), mMandates(other.mMandates), mJoinPolicy(other.mJoinPolicy->clone()), mState(other.mState), timer(other.timer), coalitionId(other.coalitionId), receivedOffers(other.receivedOffers) {

 } // copy constructor

Party::Party(Party&& other):mId(other.mId), mName(other.mName), mMandates(other.mMandates), mJoinPolicy(other.mJoinPolicy), mState(other.mState), timer(other.timer), coalitionId(other.coalitionId), receivedOffers(other.receivedOffers) {
    other.mJoinPolicy = nullptr;
} // move constructor

const Party& Party::operator=(const Party& other){
    if(this!=&other){
        mId=other.mId;
        mMandates=other.mMandates;
        mName = other.mName;
        if(mJoinPolicy)
            delete mJoinPolicy;
        mJoinPolicy = other.mJoinPolicy->clone();
    }
    return *this;
} // copy assignment

Party& Party::operator=(Party&& other){
    mId=other.mId;
    mMandates=other.mMandates;
    mName = other.mName;
    if(mJoinPolicy)
        delete mJoinPolicy;
    mJoinPolicy = other.mJoinPolicy; 
    other.mJoinPolicy = nullptr; 
    return *this;
}; // move assignment





Party::~Party(){
   if(mJoinPolicy)
       delete(mJoinPolicy); 
}

State Party::getState() const
{
    return mState;
}

void Party::setState(State state)
{
    mState = state;
}

int Party::getMandates() const
{
    return mMandates;
}

const string & Party::getName() const
{
    return mName;
}
    // TODO: implement this method
void Party::step(Simulation &s)
{
    if(mState == CollectingOffers && s.getTimer(mId) < 3 ){
        s.Tick(mId);
        if(s.getTimer(mId) == 3 ){
            s.setState(mId, Joined);
            mJoinPolicy->join(s,*this);
        }
    }
}

void Party::recieveOffers(Agent &agentOffer){
    receivedOffers.push_back(agentOffer);
}

bool Party::sameOffers(int coalitionId){
    for(unsigned int i=0; i < receivedOffers.size(); i++)
        if(receivedOffers[i].getcoalitionId() == coalitionId)
            return true;
    return false;
}
int Party::getcoalitionId (){
    return coalitionId;
}


vector<Agent> &Party::getreceivedOffers(){
    return receivedOffers;
}


int Party::getId(){
    return mId;
}