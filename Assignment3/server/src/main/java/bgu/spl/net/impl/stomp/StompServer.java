package bgu.spl.net.impl.stomp;
import bgu.spl.net.srv.Server;

public class StompServer {
      public static void main(String[] args) {
        String srvType = args[1];
        int port = Integer.parseInt(args[0]);
        if(srvType.equals("tpc"))
            Server.threadPerClient(
                true,
                port, //port
                StompMSGProtocol::new , 
                StompMessageEncoderDecoder :: new
                ).serve();
        else if(srvType.equals("reactor"))
            Server.reactor(
       4,
                port, //port
                () -> new StompMSGProtocol<>(), //protocol factory
                StompMessageEncoderDecoder::new//message encoder decoder factory
                ).serve();
        }
}
