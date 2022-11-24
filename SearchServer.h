#pragma once
#ifndef SEARCH_ENGINE_SEARCHSERVER_H
#define SEARCH_ENGINE_SEARCHSERVER_H


#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <set>
#include <thread>
#include <stdexcept>

#include "InvertedIndex.h"
#include "ConverterJSON.h"

class SearchServer {
    InvertedIndex  invertedIndex;

public:
    /**
    * @param idx в конструктор класса передаётся ссылка на класс InvertedIndex,
    * чтобы SearchServer мог узнать частоту слов встречаемых в запросе
    */
    SearchServer(InvertedIndex& idx) : invertedIndex(idx) {
    }
    //SearchServer(InvertedIndex& idx)  {
    //    invertedIndex = idx;
    //
    //}

    SearchServer() = default;

    /**
    * Метод обработки поисковых запросов
    * @param queries_input поисковые запросы взятые из файла requests.json
    * @return возвращает отсортированный список релевантных ответов для
    заданных запросов
    */
    std::vector<std::vector<RelativeIndex>> search(const std::vector<std::string>& queries);

    /** Oбёртка над invertedIndex.UpdateDocumentBase(inputDocs); */
    void UpdateDocumentBase(std::vector<std::string> inputDocs)  {
        invertedIndex.UpdateDocumentBase(inputDocs);
    }


};

#endif //SEARCH_ENGINE_SEARCHSERVER_H
