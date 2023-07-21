#pragma once
#include <vector>
#include "Graph.h"
//#include "Simulation.h"
#include "SelectionPolicy.h"
class Simulation;

class Agent
{
public:
    Agent(int agentId, int partyId, SelectionPolicy *selectionPolicy);
    ~Agent();
    Agent(const Agent& other); // copy constructor
    Agent(Agent&& other); // move constructor
    const Agent& operator=(const Agent& other); // copy assignment
    Agent& operator=(Agent&& other); // move assignment
    int getPartyId() const;
    int getId() const;
    void step(Simulation &sim);
    void setAgentId(int);
    void setPartyId(int);
    int getcoalitionId();
    void setCoalitionId(int Id);



private:
    int mAgentId;
    int mPartyId;
    SelectionPolicy *mSelectionPolicy;
    int coalitionId; 
};


