#include "../include/StompProtocol.h"

using namespace std;

class Keyboard{
    private:
    StompProtocol* protocol;

    public:
    void run();
    Keyboard(StompProtocol* protocol);
    Keyboard(const Keyboard& keyboardToServer);
    Keyboard& operator=(const Keyboard &keyboardToServer);
    ~Keyboard();
};