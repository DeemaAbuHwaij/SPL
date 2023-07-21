package bgu.spl.net.impl.stomp;
import java.util.ArrayList;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentSkipListSet;
import bgu.spl.net.api.StompMessagingProtocol;
import bgu.spl.net.srv.ConnectionHandler;
import bgu.spl.net.srv.Connections;

public class StompMSGProtocol<T> implements StompMessagingProtocol<T> {
    private boolean shouldTerminate;
    private boolean ifConnected = false;
    private String username;
    int connectionId;
    Connections<T> connections; // list of the new connection handler for each active client

    @Override
    public void start(int connectionId, Connections<T> connections) {
        this.connectionId = connectionId;
        this.connections = connections;
        shouldTerminate = false;
    }

    @Override
    public void process(T message) {
        // processe a given message and sends response via the connection object send functions
        String frameFromClient = (String)message;
        String []frameToLines = frameFromClient.split("\n"); //translate the frame
        String frameType = frameToLines[0];
        String response = " ";
        if(!ifConnected){
            if(frameType.compareTo("CONNECT") == 0){
            String frameToSend = processConnect(frameToLines);
            connections.send(connectionId, (T)frameToSend); 

            }
            else{
                connections.send(connectionId,(T) "ERROR\nmessage:You must send CONNECT to log in\n\n");
                shouldTerminate = true;
            }
        ifConnected = true;  
        }
        else{
            if(frameType.equals("SUBSCRIBE")){
                response = processSub(frameToLines);
            }
           
            else if(frameType.equals("UNSUBSCRIBE")){
                response = processUnsub(frameToLines);
            }


            else if(frameType.equals("DISCONNECT")){
                 processDisconnect(frameToLines);
                
            }

            else if(frameType.equals("SEND")){
                response = processSend(frameToLines);
            }
            String[] checkResponse = response.split("\n");
            if(checkResponse[0].equals("ERROR")){
                connections.send(connectionId,(T)response);
               // connections.disconnect(connectionId);
            }
            else if(checkResponse[0].equals("RECEIPT")){
                connections.send(connectionId,(T)response);
            }
            else {} // if he receive message            
        }
    }

    @Override
    public boolean shouldTerminate() {
        return shouldTerminate;
    }




    //connect
    public String processConnect(String []frameToLines){
        username = frameToLines[3].substring(frameToLines[3].indexOf(":") + 1);
        String passcode = frameToLines[4].substring(frameToLines[4].indexOf(":") + 1);
        if(((stompConnections<T>)connections).userToPassword.containsKey(username)){ // the user is exist
            if(!((stompConnections<T>)connections).loggedInUsers.contains(username)){ // the user is not loged in
                if(!((stompConnections<T>)connections).userToPassword.get(username).equals(passcode)) // wrong password
                    return Frame.errorFrame("", "", "", "Wrong password");
                else{
                    ((stompConnections<T>)connections).loggedInUsers.add(username);
                    ConcurrentHashMap <String,String> toAdd = new ConcurrentHashMap<>();
                    ((stompConnections<T>)connections).clientToSub.put(connectionId, toAdd);
                    return Frame.connectedFrame();
                    }
            }
            else{
                return Frame.errorFrame("", "", "", "User already logged in");
            }
        }
        else{// new user
            ((stompConnections<T>)connections).userToPassword.put(username, passcode);
            ((stompConnections<T>)connections).loggedInUsers.add(username);
            ConcurrentHashMap <String,String> toAdd = new ConcurrentHashMap<>();
            ((stompConnections<T>)connections).clientToSub.put(connectionId, toAdd);
            return Frame.connectedFrame();
        }
    }

 
      
public String processSub(String []frameToLines){
    String response = Frame.errorFrame("null", "null", "null ", ""); 
    String channell = frameToLines[1].substring(frameToLines[1].indexOf("/")+1);
    Integer sub = Integer.parseInt(frameToLines[2].substring(frameToLines[2].indexOf(":")+1));
    String receipt = frameToLines[3].substring(frameToLines[3].indexOf(":")+1); 
    if(!((stompConnections<T>)connections).topicToSub.containsKey(channell))
        ((stompConnections<T>)connections).topicToSub.put(channell, new ArrayList<>());
    ((stompConnections<T>)connections).topicToSub.get(channell).add(connectionId);

    if(!((stompConnections<T>)connections).clientToSub.containsKey(connectionId))
        ((stompConnections<T>)connections).clientToSub.put(connectionId, new ConcurrentHashMap<>());
    ((stompConnections<T>)connections).clientToSub.get(connectionId).put(channell, Integer.toString(sub));
    response = Frame.receiptFrame(receipt);
    return response;
}
//*********************UNSUBSICRIBE**********************************
    public String processUnsub(String []frameToLines){
        String response = "";
        String sub2 = frameToLines[1].substring(frameToLines[1].indexOf(":")+1);
        //Integer sub2ToGo = Integer.parseInt(frameToLines[1].substring(frameToLines[1].indexOf(":")+1));
        String receipt = frameToLines[2].substring(frameToLines[2].indexOf(":")+1); 
        ConcurrentHashMap <String , String> chToSub = ((stompConnections<T>)connections).clientToSub.get((Integer)connectionId);
        String topic = "";
        for(String channel: ((stompConnections<T>)connections).clientToSub.get(connectionId).keySet())
            if(((stompConnections<T>)connections).clientToSub.get(connectionId).get(channel).equals(sub2)){
                ((stompConnections<T>)connections).clientToSub.get(connectionId).remove(channel);
                topic = channel;
                break;
            }
        ((stompConnections<T>)connections).topicToSub.get(topic).remove(new Integer(connectionId));
        response = Frame.receiptFrame(receipt);
        return response;
    }


   
    void processDisconnect(String []frameToLines){
        String response ="RECEIPT\nreceipt-id:-1\n";         
        ((stompConnections<T>)connections).loggedInUsers.remove(username);
        connections.send(connectionId, (T) response);
        connections.disconnect(connectionId);
        shouldTerminate = true; 
    }


    public String processSend(String []frameToLines){
        String response = Frame.errorFrame("null", "null body", "null", "");
        if(frameToLines.length > 1){
            if(frameToLines[1].substring(0,frameToLines[1].indexOf(":")).equals("destination")){
                String dest = (frameToLines[1].substring(frameToLines[1].indexOf("/") + 1));
                if(dest.length() > 0){
                    String subId1 = ((stompConnections<T>)connections).clientToSub.get(connectionId).get(dest);
                    String body = "";
                    for(int i = 3 ;i < frameToLines.length ; i++){
                        body = body + frameToLines[i]+"\n";
                    }
                    response = Frame.messageFrame(subId1, dest, body);
                    connections.send(frameToLines[1].substring(13),(T)response); 
                    Frame.addIdMSg();
                
                
                }
            }
        }
        return response;
    }
}

