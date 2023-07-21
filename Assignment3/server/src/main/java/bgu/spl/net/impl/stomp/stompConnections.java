package bgu.spl.net.impl.stomp;

import java.util.ArrayList;
//import java.sql.Connection;
import java.util.HashMap;
import java.util.HashSet;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentSkipListSet;
import bgu.spl.net.srv.ConnectionHandler;
import bgu.spl.net.srv.Connections;

public class stompConnections<T> implements Connections<T> {
    public HashMap<Integer, ConnectionHandler<T>> idToHandler;// connId -> handler
    public HashMap<String, String> userToPassword;//registered users
    public HashSet<String> loggedInUsers; //check if the user already logged in 
    public ConcurrentHashMap<String,ArrayList<Integer>> topicToSub; // for each topic who are the subscribed (<chan, connID>)
    public ConcurrentHashMap<Integer,ConcurrentHashMap <String , String>> clientToSub; // for each client the subscribed channels <topic,subId>
 
    // add hash map for the topics and their connections with the client id 
    public stompConnections(){
        idToHandler = new HashMap<>();
        userToPassword = new HashMap<>();
        loggedInUsers = new HashSet<>();
        topicToSub = new ConcurrentHashMap<>();
        clientToSub =  new ConcurrentHashMap<>();
    }

    @Override
    public boolean send(int connectionId, T msg) { //should use the connectiosList to get the connection handlers
        if(idToHandler.containsKey(connectionId)){
          idToHandler.get(connectionId).send(msg);
          return true;
        }
        return false;
    }

    @Override
    public void send(String channel, T msg) { //should use the topics field to get all the users subbed to the topic
      ArrayList<Integer> listOfSubs = topicToSub.get(channel);
      if(!listOfSubs.isEmpty()){
        for(Integer connId : listOfSubs){
          send(connId, msg);
        }
      }
    }

    @Override
    public void disconnect(int connectionId) {
        clientToSub.remove(connectionId);
        idToHandler.remove(connectionId);
        for(String topic: topicToSub.keySet())
          if(topicToSub.get(topic).contains(connectionId))
              topicToSub.get(topic).remove(connectionId);
    }
}
