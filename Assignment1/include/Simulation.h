#pragma once
#include <vector>
#include "Graph.h"
#include "Agent.h"

using std::string;
using std::vector;
class Simulation
{
public:
    Simulation(Graph g, vector<Agent> agents);
    void step();
    bool shouldTerminate() const;

    const Graph &getGraph() const;
    const vector<Agent> &getAgents() const;
    const Party &getParty(int partyId) const;
    const vector<vector<int>> getPartiesByCoalitions() const;
    vector <int> mandatesSum (vector<Agent> conVector);
    int getCoalitionId();
    int getSizeOfAgents();
    Graph &getGraph2();
    void add (int coalitionId, int partyId);
    void addAgent(Agent toCopy);
    int getTimer(int partyId);
    void Tick(int partyId);
    void setState(int partyId, State state);


private:
    Graph mGraph;
    vector<Agent> mAgents;
    vector<vector<int>> coalitions;
    vector<int> timers;
    //int coalitionId;
};
