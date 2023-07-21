#include <sstream>
#include "../include/StompProtocol.h"
#include <limits>
#include <fstream>
#include <iostream>
#include <thread>
using namespace std;



StompProtocol::StompProtocol(map<pair<string, string>, vector<Event>*> &pairToEventList) : userName(" "),subId(0),password(" "), handler(nullptr), topicToSubId(), shouldTerminate(false), pairToEventList(pairToEventList) {}


//int port = 7777;
std::vector<std::string> splitedStr;
//std::string frame = " ";



StompProtocol::~StompProtocol()
{
    if (handler != nullptr)
    {
        delete handler;
    }
}


ConnectionHandler *StompProtocol::getHandler()
{
    return handler;
}


void StompProtocol::commandToFrame(std::string command){ 
    //what the server should recieve
    vector< std::string> commandToWords = split(command , " ");
    splitedStr = commandToWords;
    bool shouldDisconnect = false;
    string frame = "";
    std::string commandType = commandToWords[0];// the type of command
    if(userName ==" " && commandType != "login" ){
        cout << "You must login first" << endl;
    }
    if(commandType == "login"){//CONNECT frame DONE 
        CONNECT();
    }

    else if (handler!= nullptr){ 
         if(commandType == "exit"){
            UNSUBSCRIBE();
        }
        else if(commandType == "join"){
            SUBSCRIBE();
        }
        else if(commandType == "logout"){
            shouldTerminate = true;
            DISCONNECT();
        }
        else if(commandType == "summary"){
            SENDSUMMARY();
        }
        else if(commandType == "report"){
            SENDREPORT();
        }
        else{
            cout << "incorrect command" <<endl;
        }
    }
    if(handler!=nullptr && frame!=""){
        handler->sendMessage(frame);
    }
    if(shouldDisconnect){
        shouldTerminate = true;
    }
}


std::string StompProtocol::frameToOutput( std::string frame){
    vector< std::string> frameToLines = split(frame, "\n");
    std::string frameType = frameToLines[0];
    if(frameType == "CONNECTED"){
        std::cout << "Login successful\n" <<endl;
        return "Login successful";
    }
    else if(frameType == "ERROR"){
        std::cout<<frameToLines[8]<<endl;
        // if(handler){
        //     delete handler;
        //     handler = nullptr;
        // }
        // shouldTerminate = true;
    }
    else if(frameType == "RECEIPT"){
       string receiptId = frameToLines[1].substr(11); 
       int toSub = stoi(receiptId) - 10;
       string dest = subIdToTopic[toSub];
       if(receiptIdToCommand[stoi(receiptId)] == "exit")
            std::cout<<"Exited channel " + dest << endl;
       else if(receiptIdToCommand[stoi(receiptId)] == "join"){
            std::cout<<"Joined channel " + dest << endl;       
        }
        else{
            shouldDisconnect = true;
            return "disconnecting";
        }
    }
    else if (frameType == "MESSAGE"){
        MESSAGE(frameToLines);
    }
    return frame;
}




 void StompProtocol::CONNECT(){ //login 91836 deema 1234
    if(userName != " "){
       std::cout <<"The client is already logged in, log out before trying again" <<endl;
       return;
    }
    int index = splitedStr[1].find(":");
    string host = splitedStr[1].substr(0,index);
    int port  = stoi(splitedStr[1].substr(index+1));   
    userName = splitedStr[2];
    password = splitedStr[3];
    string frame = "CONNECT\naccept-version:1.2\nhost:stomp.cs.bgu.ac.il\nlogin:" + userName + "\npasscode:" +
        password + "\n";
    handler = new ConnectionHandler(host, port);
    if(handler->connect()){
        sendFrame(frame);
    }
    else
        handler = nullptr;
}



void StompProtocol::UNSUBSCRIBE(){
    string destination = splitedStr[1];
    if(topicToSubId.count(destination) <= 0){
        cout << "invaild unsubscribtion" <<endl;
        return;
    }
    int sub =  topicToSubId[destination];
    string receiptID = to_string(sub+10);
    receiptIdToCommand[sub +10]="exit";
    std::string frame = "UNSUBSCRIBE\nid:"+ to_string(sub) +
       "\n"+ "receipt:" +
        receiptID + "\n";
    topicToSubId.erase(destination);
    sendFrame(frame);
}



void StompProtocol:: SUBSCRIBE(){
    std::string destination = splitedStr[1];
    if(topicToSubId.count(destination) > 0){
        cout << "invaild subscribtion" << endl;
        return;
    }
    topicToSubId[destination] = subId;// the sub id pf the dest 
    subIdToTopic[subId] = destination;
    receiptIdToCommand[subId + 10] = "join"; // what he did in receipt
    std::string frame = "SUBSCRIBE\ndestination:/" + destination +'\n' + 
    "id:"+ std::to_string(subId) + '\n'+ 
    "receipt:" + std::to_string(subId + 10) + '\n';
    subId ++;
    sendFrame(frame);
}





void StompProtocol::DISCONNECT(){
    if(splitedStr.size()!=1)
        cout << "Input is inValid" <<endl;
    std::string destination = splitedStr[1];
    std::string frame = "DISCONNECT\nreceipt:" + to_string(0) + "\n";

    shouldDisconnect = true;
    sendFrame(frame);
}


void StompProtocol::SENDSUMMARY(){
    string gameName = splitedStr[1];
    string user = splitedStr[2];
    string fileName = splitedStr[3];
    pair<string,string> key(gameName,user);
    try{
        vector<Event>* eventList = pairToEventList.at(key);
        string toAdd = stats(splitedStr, eventList, gameName);
        writeToFile(toAdd, fileName);
    }
    catch(out_of_range e){
        cout << "There is no reports on this topic from this user" << endl;
    }
}

string StompProtocol::stats(vector<string> &input, vector<Event> *eventList, string gameName)
{
    int index = gameName.find('_');
    string team_a = gameName.substr(0, index);
    string team_b = gameName.substr(index + 1);
    string stats = team_a + " vs " + team_b + '\n';
    stats = stats + "Game stats:\n" + "General stats:\n";
    map<string, string> game_updates;
    map<string, string> team_a_updates;
    map<string, string> team_b_updates;
    for(Event &event: *eventList){
        map<string, string> ev_game_updates = event.get_game_updates();
        map<string, string> ev_a_updates = event.get_team_a_updates();
        map<string, string> ev_b_updates = event.get_team_b_updates();
        for(auto &pair: ev_game_updates)
            game_updates[pair.first] = pair.second;
        for(auto &pair: ev_a_updates)
            team_a_updates[pair.first] = pair.second;
        for(auto &pair: ev_b_updates)
            team_b_updates[pair.first] = pair.second;
    }
    for(auto &pair: game_updates)
        stats += pair.first + ": " + pair.second + "\n";
        
    stats += team_a + " stats:\n";
    for(auto &pair: team_a_updates)
        stats += pair.first + ": " + pair.second + "\n";

    stats += team_b + " stats:\n";
    for(auto &pair: team_b_updates)
        stats += pair.first + ": " + pair.second + "\n";

    stats += "\nGame event reports:\n";
    for(Event &event: *eventList)
        stats += to_string(event.get_time()) + " - " + event.get_name() + ":\n\n" + event.get_discription() + "\n\n\n\n";
    return stats;
}

void StompProtocol::writeToFile(string toWrite, string fileName)
{
    ofstream f(fileName);
    f << toWrite;
    f.close();
}

void StompProtocol::MESSAGE(vector<string> frameToLines)
{
    string user = frameToLines[5].substr(5);
    string gameName = frameToLines[3].substr(13);
    pair<string, string> key(gameName, user);
    Event recieved = createEvent(frameToLines);
    bool listExists = (pairToEventList.count(key) != 0);
    if(listExists){
        vector<Event>* eventList = pairToEventList.at(key);
        eventList->push_back(recieved);
    }
    else{
        vector<Event>* eventList = new vector<Event>();
        eventList->push_back(recieved);
        pairToEventList[key] = eventList;
    }
}

Event StompProtocol::createEvent(vector<string> frameToLines)
{
    string team_a = frameToLines[6].substr(7);
    string team_b = frameToLines[7].substr(7);
    string user = frameToLines[5].substr(5);
    string eventName = frameToLines[8].substr(11);
    int time = stoi(frameToLines[9].substr(5));
    int index = 11;
    map<string, string> gamesMap= map<string, string>();    //game updates

    while(frameToLines[index] != "team a updates:"){
        vector<string> key_value = split(frameToLines[index], ":");
        string key = key_value[0];
        string value = key_value[1];
        gamesMap[key] = value;
        index++;
    }
    index++;

    map<string, string> team_a_map = map<string, string>(); //creat map for team a 
    while(frameToLines[index] != "team b updates:"){
        vector<string> key_value = split(frameToLines[index], ":");
        string key = key_value[0];
        string value = key_value[1];
        team_a_map[key] = value;
        index++;
    }
    index++;

    map<string, string> team_b_map = map<string, string>(); //creat map for team b
    while(frameToLines[index] != "description:"){
        vector<string> key_value = split(frameToLines[index], ":");
        string key = key_value[0];
        string value = key_value[1];
        team_b_map[key] = value;
        index++;
    }
    
    index++;
    string description = frameToLines[index];
    return Event(team_a, team_b, eventName, time,gamesMap,team_a_map, team_b_map, description);
}





void StompProtocol::sendFrame(string frame)
{
    if (handler != nullptr && frame != "")
        handler->sendMessage(frame);
}

void StompProtocol::SENDREPORT(){
    string fileName = splitedStr[1];
    std::string frame;
    names_and_events names_and_events = parseEventsFile("data/"+fileName);
    string opening = firstOfCreateSendFrame(names_and_events);
    for (Event event : names_and_events.events)
    {
        frame = createSendFrame(opening, event);
        sendFrame(frame);
    }
}

   
string StompProtocol::createSendFrame(string opening, Event &event)
{
    string game_updates = event.generalGameUpdates();
    string teamA_updates = event.teamAUpdate();
    string teamB_updates = event.teamBUpdate();
    string frame = opening + "event name:" + event.get_name() + "\n" +
                    "time:" + to_string(event.get_time()) + "\n" +
                    "general game updates:\n" + game_updates +
                   "team a updates:\n" + teamA_updates +
                    "team b updates:\n" + teamB_updates +
                     "description:\n" + event.get_discription() + "\n";
    return frame;
}

string StompProtocol::firstOfCreateSendFrame(names_and_events &names_and_events)
{
    string gameName = names_and_events.team_a_name + "_" + names_and_events.team_b_name;
    string opening = "SEND\n";
    opening += "destination:/" + gameName +
             "\n\n" + "user:" + userName + "\n" +
            "team a:" + names_and_events.team_a_name +
            "\nteam b:" + names_and_events.team_b_name + "\n";
    return opening;
}


vector<std::string> StompProtocol::split(string s, string delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    string token;
    vector<string> res;
    while ((pos_end = s.find (delimiter, pos_start)) != string::npos) {
        token = s.substr (pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back (token);
    }
    res.push_back (s.substr (pos_start));
    return res;
}

void StompProtocol::resetHandler(){
    //delete handler;
    handler = nullptr;
}
