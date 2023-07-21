package bgu.spl.net.impl.stomp;
import java.util.HashMap;
import java.util.concurrent.atomic.AtomicInteger;
public class Frame {
    private String stompCommand ;
    private String frameBody ;
    private static final AtomicInteger msgId = new AtomicInteger(0);
    

    public Frame( String stompCommand , String frameBody) {
        this.stompCommand = stompCommand ; 
        this.frameBody = "";
    }

    public static String connectedFrame(){ // CONNECTED frame 
        return "CONNECTED\n" + "version:1.2\n ";
    }

    public static String receiptFrame(String receiptId){ //RECEIPT frame
     return ("RECEIPT\n" + "receipt-id:" + receiptId + "\n");
    }

    public static String errorFrame(String receiptId, String errorType , String msg , String description){ //ERROR frame
        return ("ERROR\n" + "receipt-id : " + receiptId + '\n' + "message : " + errorType +
         '\n' +"\n The message: " + "\n----- \n" + msg + "\n----- \n" + description +"\n" );
    }
    public static String messageFrame(String subscription, String dest , String body){ //MESSAGE frame
        return ("MESSAGE\n" + "subscription:" + subscription + "\nmessage-id:" + msgId.toString() + "\ndestination:/" + dest + "\n\n" + body+ "\n");
    }
    public String getStompCommand(){
        return stompCommand;
    }
    public String getBody(){
        return frameBody;
    }
    public static void addIdMSg(){
        int id;
        do{
            id = msgId.get();
        }
        while (!msgId.compareAndSet(id, id+1));
    }
}
