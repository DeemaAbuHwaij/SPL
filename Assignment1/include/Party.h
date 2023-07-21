#pragma once
#include <string>
#include <vector>

class JoinPolicy;
class Agent;
using std::string;
using std::vector;
class Simulation;
enum State
{
    Waiting,
    CollectingOffers,
    Joined
};

class Party{

public:
    Party(int id, string name, int mandates, JoinPolicy *jp); 
    ~Party();
    Party(const Party& other); // copy constructor
    Party(Party&& other); // move constructor
    const Party& operator=(const Party& other); // copy assignment
    Party& operator=(Party&& other); // move assignment
  

    int getMandates() const;
    void step(Simulation &s);
    void setState(State state);
    State getState() const;
    const string &getName() const;
    bool sameOffers(int coalitionId);
    void recieveOffers(Agent &agentOffer);
    int getcoalitionId ();
    vector<Agent> &getreceivedOffers();
    int getId();

private:
    int mId;
    string mName;
    int mMandates;
    JoinPolicy *mJoinPolicy;
    State mState;
    int timer;
    int coalitionId; 
    vector<Agent> receivedOffers; 
};




