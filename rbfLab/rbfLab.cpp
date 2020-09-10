#include "Ringbuffer.h"
#include <chrono>

std::condition_variable readCv;
std::condition_variable writeCv;
std::mutex writeLock;
std::mutex readLock;
std::mutex generateLock;






//skriver chars fra consoll til ringbuffer
void writeToBuffer(Ringbuffer<char>* rb, bool* running) {
    std::unique_lock<std::mutex> threadLock(writeLock);
    while (*running) {
        char ch;
        std::cin.get(ch);//henter neste char fra cin istreamen
        rb->write(ch); //skriver til ringbufferen
        readCv.notify_all(); //låser opp alle mutexer koblet til condition variabelen, de vil så skjekke om de kan gå videre
        while (rb->checkForWriteLock()) //dersom neste index er ringbufferen sin out, thread låses til condiction variabelen notifyer
            writeCv.wait(threadLock);
    }
}


//leser fra ringbuffer til konsoll
void readFromBuffer(Ringbuffer<char>* rb, bool* running) {
    std::string str = "";
    std::unique_lock<std::mutex> threadLock(readLock);
    while (*running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        while (rb->checkForReadLock())//dersom out har tatt igjen inn, låser thread til condiction variabelen notifyer
            readCv.wait(threadLock);
        char ch = rb->read();
        std::cout << ch;
        if (ch == ' ' || ch == '\n') //reset str dersom ordet er ferdig/linjeskift
            str = "";
        else
            str += ch;
        if (str == "exit" || str == "Exit")
            *running = false;
        writeCv.notify_one();
    }
}

void generateText(Ringbuffer<char>* rb, bool* running) {
    std::srand(time(NULL)); //gir en seed til random number enginen basert på system time
    char* text = new char[6];
    std::unique_lock<std::mutex> threadLock(generateLock);
    while (*running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(30)); //låser threaden i 30 millisekunder
        for (int i = 0; i < 5; i++) {
            char ch = char(rand() % 58 + 65); //genererer tall fra 65 - 122, caster til char
            text[i] = ch;
        }
        text[5] = '\n'; //legger til et linjeskift bak de 5 genererte charsa
        
        for (int i = 0; i < 6; i++) {//skriver charsene inn i ringbufferen
            rb->write(text[i]);
            readCv.notify_all(); //lar read condiction variabelen gå videre for å se om den kan få lese
            while (rb->checkForWriteLock()) //så lenge neste index er ringbufferen sin out, thread låses til condiction variabelen notifyer
                writeCv.wait(threadLock);
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


