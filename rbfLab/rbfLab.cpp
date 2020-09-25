#include "Ringbuffer.h"
#include <chrono>



//skriver chars fra consoll til ringbuffer
void writeToBuffer(rB::Ringbuffer<char>* ptrRb, bool* ptrRunning) {
    while (*ptrRunning) {
        char ch;
        std::cin.get(ch);//henter neste char fra cin istreamen
        ptrRb->write(ch);
    }
}


//leser fra ringbuffer til konsoll
void readFromBuffer(rB::Ringbuffer<char>* ptrRb, bool* ptrRunning) {
    std::string str = "";
    while (*ptrRunning) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        char ch = ptrRb->read();
        std::cout << ch;
        if (ch == ' ' || ch == '\n') //reset str dersom ordet er ferdig/linjeskift
            str = "";
        else
            str += ch;
        if (str == "exit" || str == "Exit") 
            *ptrRunning = false;
    }
}

void generateText(rB::Ringbuffer<char>* ptrRb, bool* ptrRunning) {
    std::srand(time(NULL));
    char* text = new char[6];
    while (*ptrRunning) {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        for (int i = 0; i < 5; i++) {
            //char ch = char(rand() % 58 + 65); //genererer ett tall i mengden <0,57>, legger til 65 så mengden blir <65,122>, (ascii for A-z)
            char ch = char(rand() % 10 + 48); //genererer ett tall i menden <0,9> legger til 48 så mengden blir <48,57> (ascii for 0-9)
            text[i] = ch;
        }
        text[5] = '\n';
        ptrRb->write(text,0,5);
    }
    delete[] text;
}


int main(){
    int capacity = 10;
    rB::Ringbuffer<char>* ptrRb = new rB::Ringbuffer<char>(capacity);
    bool running = true;
    std::thread th1(writeToBuffer, ptrRb,&running);
    std::thread th2(readFromBuffer, ptrRb,&running);
    std::thread th3(generateText, ptrRb, &running);
    if (th1.joinable()) th1.join();
    if (th2.joinable()) th2.join();
    if (th3.joinable()) th3.join();
    delete ptrRb;
    return 0;
}


