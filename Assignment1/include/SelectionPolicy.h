#pragma once
//#include "Graph.h"
//#include "Agent.h"
class Agent;
class Graph;
class SelectionPolicy { 
public:
    virtual ~SelectionPolicy();
    SelectionPolicy();
    virtual void select(Graph &g , Agent &a)=0;
    virtual char checkType()=0;
    virtual SelectionPolicy* clone()=0;
};

class MandatesSelectionPolicy: public SelectionPolicy{ 
public:
    ~MandatesSelectionPolicy();
    MandatesSelectionPolicy();
    void select(Graph &g, Agent &a);
    char checkType();
    SelectionPolicy* clone();
};

class EdgeWeightSelectionPolicy: public SelectionPolicy{ 
public:
    EdgeWeightSelectionPolicy();
    ~EdgeWeightSelectionPolicy();
    void select(Graph &g, Agent &a);
    char checkType();
    SelectionPolicy* clone();
};