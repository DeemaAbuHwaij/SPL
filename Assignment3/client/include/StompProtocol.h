#pragma once
#include "../include/ConnectionHandler.h"
#include "../include/Event.h"
#include <string>
#include <vector>
#include <map>

using namespace std;


class StompProtocol
{
private:


    int subId;
    int rId = 0;
    std::vector<std::string> splitedStr;
    map<int, std::string> subIdToTopic;
    map<std::string,int> topicToSubId;
    map <pair<string, string> , vector<Event>*> & pairToEventList;
    std::string host;
    bool shouldDisconnect;

    
public:
    std::string userName;
    std::string password;
    StompProtocol(map <pair<string, string> , vector<Event>*> & pairToEventList);
    bool shouldTerminate;
    ~StompProtocol();
    bool isConnected;
    ConnectionHandler* handler;
    ConnectionHandler* getHandler();
    StompProtocol(const StompProtocol&);
    StompProtocol& operator =(const StompProtocol&);
    std::map<int,std::string> receiptIdToCommand;
    void CONNECT();
    void DISCONNECT();
    void SUBSCRIBE();
    void UNSUBSCRIBE();
    void SENDREPORT();
    void SENDSUMMARY();
    void MESSAGE(vector<string> frameToLines);
    std::string createSendFrame(string opening, Event &event);
    Event createEvent(vector<string> frameToLines);
    void writeToFile(string toWrite, string fileName);
    vector< std::string> split( std::string s,  std::string delimiter);
    void commandToFrame(std::string command); 
    std::string frameToOutput( std::string frame);
    string stats(vector<string> &input, vector<Event> *eventList, string gameName);
    string firstOfCreateSendFrame(names_and_events &names_and_events);
    void sendFrame(string frame);
    void resetHandler();
};
