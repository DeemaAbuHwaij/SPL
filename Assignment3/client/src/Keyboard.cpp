#include "../include/Keyboard.h"

using namespace std;

Keyboard::Keyboard(StompProtocol* protocol):protocol(protocol){}

void Keyboard::run() {
    while(!(protocol->shouldTerminate) ) {
        const short bufsize = 1024;
        char buf[bufsize];
        cin.getline(buf, bufsize);
        string line(buf);
        if(line.length() > 1)
            protocol->commandToFrame(line);
        sleep(1);    
    }
}

Keyboard::Keyboard(const Keyboard &keyboardToServer): protocol(keyboardToServer.protocol) {}

Keyboard& Keyboard::operator=(const Keyboard &keyboardToServer){
    protocol = keyboardToServer.protocol;
    return *this;
}

Keyboard::~Keyboard() {
    delete protocol;
}