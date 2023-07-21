#include "../include/Keyboard.h"
#include <thread>
#include <map>
#include <list>

using namespace std;

void run(StompProtocol* protocol) {
    while(!(protocol->shouldTerminate) ) {
        const short bufsize = 1024;
        char buf[bufsize];
        cin.getline(buf, bufsize);
        string line(buf);
        if(line.length() > 1)
            protocol->commandToFrame(line);
       // sleep(1);
    }
}


int main(int argc, char *argv[])
{
	map<pair<string, string>, vector<Event>*> EventList = map<pair<string, string>, vector<Event>*>();
	while (1)
	{
		StompProtocol *protocol = new StompProtocol(EventList);
		//Keyboard keyboardToServer = Keyboard(protocol);
		//thread keyboardThread(&run, ref(protocol));
		//ConnectionHandler *handler = protocol->getHandler();
		//login
		while(protocol->getHandler() == nullptr){
			const short bufsize = 1024;
        	char buf[bufsize];
        	cin.getline(buf, bufsize);
        	string line(buf);
			protocol->commandToFrame(line);
			if(protocol->getHandler() == nullptr){
				protocol->userName = " ";
				protocol->password = " ";
				continue;
			}
			string response;
			protocol->getHandler()->getMessage(response);
			string output = protocol->frameToOutput(response);
			if(output != "Login successful"){
				protocol->userName = " ";
				protocol->password = " ";
				protocol->getHandler()->close();
				protocol->resetHandler();
			}
		}

		thread keyboardThread(&run, ref(protocol));
		ConnectionHandler *handler = protocol->getHandler();
		while (!protocol->shouldTerminate)
		{
			string response, output = "";
			handler->getMessage(response);
			output = protocol->frameToOutput(response);
			if(output == "disconnecting")
				keyboardThread.join();
		}
		cout << "Disconnected, Exiting\n" << std::endl;
	}

	for(pair<pair<string,string>,vector<Event>*> key:EventList)
		delete &key.second;
	return 0;
}
