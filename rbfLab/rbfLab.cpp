#include "Ringbuffer.h"
#include <chrono>



//skriver chars fra consoll til ringbuffer
void writeToBuffer(Ringbuffer<char>* rb, bool* running) {
    while (*running) {
        char ch;
        std::cin.get(ch);//henter neste char fra cin istreamen
        rb->write(ch);
    }
}


//leser fra ringbuffer til konsoll
void readFromBuffer(Ringbuffer<char>* rb, bool* running) {
    std::string str = "";
    while (*running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        char ch = rb->read();
        std::cout << ch;
        if (ch == ' ' || ch == '\n') //reset str dersom ordet er ferdig/linjeskift
            str = "";
        else
            str += ch;
        if (str == "exit" || str == "Exit")
            *running = false;
    }
}

void generateText(Ringbuffer<char>* rb, bool* running) {
    std::srand(time(NULL)); //gir en seed til random number enginen basert på system time
    char* text = new char[6];
    while (*running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        for (int i = 0; i < 5; i++) {
            //char ch = char(rand() % 58 + 65); //genererer ett tall i mengden <0,57>, legger til 65 så mengden blir <65,122>, caster til char
            char ch = char(rand() % 10 + 48);
            text[i] = ch;
        }
        text[5] = '\n'; //legger til et linjeskift bak de 5 genererte charsa
        
        for (int i = 0; i < 6; i++) {//skriver charsene inn i ringbufferen
            rb->write(text[i]);
        }
        
        
    }
    delete[] text;
}


int main(){
    int capacity = 10;
    Ringbuffer<char>* rb = new Ringbuffer<char>(capacity);
    bool running = true;
    std::thread th1(writeToBuffer, rb,&running); 
    std::thread th2(readFromBuffer, rb,&running);
    std::thread th3(generateText, rb, &running);
    if (th1.joinable()) th1.join();
    if (th2.joinable()) th2.join();
    if (th3.joinable()) th3.join();
    delete rb;
    return 0;
}


