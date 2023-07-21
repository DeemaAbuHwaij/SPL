#pragma once
class Simulation;
#include "Party.h"

class Party;

class JoinPolicy {
public:
    JoinPolicy();
    virtual ~JoinPolicy();
    virtual void join(Simulation &p ,Party &pp)=0;
    virtual char checkType()=0;
    virtual JoinPolicy* clone()=0;
};

class MandatesJoinPolicy : public JoinPolicy {
public:
    MandatesJoinPolicy();
    ~MandatesJoinPolicy();
    void join(Simulation &p,Party &pp);
    char checkType();
    JoinPolicy* clone();
};

class LastOfferJoinPolicy : public JoinPolicy {
public:
    LastOfferJoinPolicy();
    ~LastOfferJoinPolicy();
    void join(Simulation &p,Party &pp);
    char checkType();
    JoinPolicy* clone();
};