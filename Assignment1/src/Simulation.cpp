#include "Simulation.h"
#include <vector>
#include "Party.h"
#include "Graph.h"
#include <iostream>
using namespace std;



Simulation::Simulation(Graph graph, vector<Agent> agents) : mGraph(graph), mAgents(agents), coalitions(vector<vector<int>>()), timers(vector<int>())
{

    for(unsigned int i = 0; i<agents.size(); i++){
        coalitions.push_back(vector<int>());
        mAgents[i].setCoalitionId(i);
        coalitions[i].push_back(agents[i].getPartyId());
    }
    for(int i=0; i<graph.getNumVertices(); i++)
        timers.push_back(0);
    // You can change the implementation of the constructor, but not the signature!
}

void Simulation::step()
{
    for(int i=0; i<getGraph().getNumVertices(); i++){
        Party par = getGraph2().getParty2(i);
        par.step(*this);
    }
    for(unsigned int i = 0; i < mAgents.size(); i++){
        mAgents[i].step(*this);
    }
}

bool Simulation::shouldTerminate() const
{

    // TODO implement this method
   // cout << "AA" << endl;
   bool ans1 = false; 
   size_t length = coalitions.size();
    for(size_t i=0; i< length && !ans1; i++ ){
        int sum = 0;
        size_t coalitionSize = coalitions[i].size();
        for(size_t j=0; j<coalitionSize && !ans1; j++){
            sum += mGraph.getParty(coalitions[i][j]).getMandates();
            if(sum >= 61)
                ans1 = true;
            }
        //cout << "coalition " << i << "'s size is: " << sum << endl;
    }
    bool ans2 = true;
    for (int i = 0; i < mGraph.getNumVertices() && ans2; i++){
        if (mGraph.getParty(i).getState() != Joined) //create getParty2 not const
            ans2 = false;      
    }
    bool ans = ans1 || ans2;
    return ans;
}

const Graph &Simulation::getGraph() const
{
    return mGraph;
}

Graph &Simulation::getGraph2() 
{
    return mGraph;
}

const vector<Agent> &Simulation::getAgents() const
{
    return mAgents;
}

const Party &Simulation::getParty(int partyId) const
{
    return mGraph.getParty(partyId);
}

/// This method returns a "coalition" vector, where each element is a vector of party IDs in the coalition.
/// At the simulation initialization - the result will be [[agent0.partyId], [agent1.partyId], ...]
const vector<vector<int>> Simulation::getPartiesByCoalitions() const
{
    // TODO: you MUST implement this method for getting proper output, read the documentation above.
    return coalitions;
}


vector <int> Simulation ::mandatesSum (vector<Agent> conVector){
    vector <int> answer ;
    for(unsigned int i=0 ;i<conVector.size() ;i++){
        answer.push_back(0);
        int helper2 = conVector[i].getcoalitionId();
        for(unsigned int j=0 ;j<coalitions[helper2].size() ; j++){
            Party p = mGraph.getParty2(coalitions[helper2][j]);
            answer[i] = answer[i] + p.getMandates();
        }
    }
    return answer;
}

int Simulation::getSizeOfAgents(){
    return mAgents.size();
}

void Simulation::add (int coalitionId, int partyId){
    coalitions[coalitionId].push_back(partyId);
}

void Simulation::addAgent(Agent toCopy){
    mAgents.push_back(toCopy);
}

int Simulation::getTimer(int partyId){
    return timers[partyId];
}

void Simulation::Tick(int partyId){
    timers[partyId]++;
}

void Simulation::setState(int partyId, State state){
    mGraph.setState(partyId, state);
}