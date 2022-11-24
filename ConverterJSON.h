#pragma once
#ifndef SEARCH_ENGINE_CONVERTERJSON_H
#define SEARCH_ENGINE_CONVERTERJSON_H


#include <iostream>
#include <sstream>
#include "nlohmann\json.hpp"
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <set>
#include <thread>
#include <mutex>
#include <cmath>
#include <stdexcept>

#define CurrentVersion "2.1"

struct RelativeIndex {
    size_t docId;
    float rank;   // Относительная релевантность
    bool operator ==(const RelativeIndex& other) const {
        return (docId == other.docId && rank == other.rank);
    }
};

/**
* Класс для работы с JSON-файлами
*/
class ConverterJSON {
    int responsesLimit=5;
    std::string version = "0.1";
    std::vector<std::string> files;
public:
    std::string SearchServerName = "SearchServer";

    ConverterJSON() = default;

    /**
    * Метод получения содержимого файлов
    * @return Возвращает список с содержимым файлов перечисленных
    * в config.json
    */
    std::vector<std::string> GetTextDocuments();

    /**
    * Метод считывает поле max_responses для определения предельного
    * количества ответов на один запрос
    * @return
    */
    int GetResponsesLimit();

    int GetResponsesLimit(bool read) {  // get value responsesLimit from memory or read from file
        if(!read)
            return responsesLimit;
        return GetResponsesLimit();
    };

    /**
    * Метод получения запросов из файла requests.json
    * @return возвращает список запросов из файла requests.json
    */
    std::vector<std::string> GetRequests();
    /**
    * Положить в файл answers.json результаты поисковых запросов
    */
    void putAnswers(std::vector<std::vector<RelativeIndex>> answers);

    nlohmann::json readConfig(); // читаем json из файла конфигурации
    void GetConfig(); // получение информации из файла конфигурации (files, responsesLimit .....)

    /** Конструктор с инициализацией всех параметров */
    ConverterJSON (bool full) {
        GetConfig();
    }
};

#endif //SEARCH_ENGINE_CONVERTERJSON_H
