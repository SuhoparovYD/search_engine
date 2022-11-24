
#include "InvertedIndex.h"

void InvertedIndex::UpdateDocument(size_t docId) {
    // для одного файла
    using namespace std;

    map<string, size_t> frecFileDict;               // word:count - частотный словарь для одного файла
    string text = docs.at(docId);

    string delimiter = " ,!.|;/:?~'_(#)%{&}^<->=[+]*·—–\n\t\"";         // можно из файла config.json
    char* next = nullptr;
    char* token = strtok_s(const_cast<char*>(text.c_str()), delimiter.c_str(), &next);  // слово
    while (token != nullptr) {
        ++frecFileDict[token];   // частота вхождений слова
        token = strtok_s(nullptr, delimiter.c_str(), &next);    // следующее слово
    }

    screenMutex.lock();
    cout << docId << " ";   // вместо прогреессбара
    screenMutex.unlock();

    freqMutex.lock();
    for (auto item : frecFileDict) {                               // загрузка в общий словарь
        if (freqDictionary.count(item.first)) {                         // if word exist
            vector<Entry> wordEntry = freqDictionary.at(item.first);    // this word in freqDictionary
            wordEntry.push_back(Entry{ docId, item.second });
            freqDictionary.at(item.first) = wordEntry;                  // модифицируем словарь
        }
        else
            freqDictionary[item.first] = vector<Entry>{ Entry{docId, item.second} }; // добавдяем слово в словарь
    }
    freqMutex.unlock();

}

// поток индексирования документов
void InvertedIndex::makeThreads() {
    while (1) {
        fileMutex.lock();
        size_t docId = head++;
        fileMutex.unlock();
        if (docId >= docs.size()) break; // > числa файлов
        UpdateDocument(docId);
    }
}

void InvertedIndex::UpdateDocumentBase(std::vector<std::string> inputDocs) {
    docs = inputDocs;
    std::thread* threads = new std::thread[std::thread::hardware_concurrency()];

    screenMutex.lock();
    std::cout << "hardware_concurrency: " << std::thread::hardware_concurrency() << "\n";
    screenMutex.unlock();
    // создание потоков в оптимаотном количестве для данного процессора(ов))
    for (unsigned i = 0; i < std::thread::hardware_concurrency(); i++)
        if (!threads[i].joinable())
            threads[i] = std::thread(&InvertedIndex::makeThreads, this);
    // завершение потоков
    for (unsigned i = 0; i < std::thread::hardware_concurrency(); i++)
        threads[i].join();

    std::cout << "\n";
}

