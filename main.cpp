#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <stdexcept>

#include <conio.h>


#include "ConverterJSON.h"
#include "SearchServer.h"

std::mutex fileMutex;
std::mutex freqMutex;
std::mutex screenMutex;

int main() {
    using namespace std;
    system("chcp 65001");

    ConverterJSON cj(true);  // + read config

    SearchServer ss;
    ss.UpdateDocumentBase(cj.GetTextDocuments());  // обновляем индекс в классе SearchServer

    screenMutex.lock();
    cout << "\n";
    system("mkdir last");  // имитация истории
    cout << "\n";
    screenMutex.unlock();

    int count = 0;  // счетчик запросов - имитация
    while(1) {
        try {
            auto queries = cj.GetRequests();
            auto responses = ss.search(queries);
            cj.putAnswers(responses);
        }
        catch (const std::runtime_error& ex) {
            std::cerr << "\n" << cj.SearchServerName << " - " << ex.what();
        }

        // имитация поступления нового запроса
        // прежние answers и requests перемещаются в /last
        int command;
        cout << "\nCommand (1, 2 - new request, 3 - exit) : ";
        cin >> command;
        if (command == 3) exit(0);
        if (command== 1 or command==2) {
            string target = "move/y answers.json last/answers_" + to_string(count) + ".json";
            system(target.c_str());
            target = "move/y requests.json last/requests_" + to_string(count++) + ".json";
            system(target.c_str());
            if(command==1)
                system("copy 1\\requests.json requests.json");
            else
                system("copy 2\\requests.json requests.json");
        }
    }

    return 0;
}
