
#include "SearchServer.h"

std::vector<std::vector<RelativeIndex>> SearchServer::search(const std::vector<std::string>& queries) {

    std::string word; // слово запроса
    std::vector<std::vector<RelativeIndex>> responses;  // ответы
    ConverterJSON c;
    int responsesLimit = c.GetResponsesLimit(0);  // no read config

    for (auto&  querie: queries) {
        std::vector<std::vector<Entry>> foundWords;  // Entry по словам запроса

        std::stringstream querieStream = (std::stringstream)querie;
        std::set<std::string> words;
        while (!querieStream.eof()) {  // разбор по словам
            querieStream >> word;
            words.emplace(word);       // набор уникальных слов запроса   tofun
        }

        for (auto& word : words)     // по уникальным словам запроса
            foundWords.push_back(invertedIndex.GetWordCount(word));

        // сортировка по возрастанию
        if (foundWords.size())
            sort(foundWords.begin(), foundWords.end(), [](auto fw1, auto fw2) {
                return fw1.size() < fw2.size();
            });

        // пересечение множеств по docId
        auto fw = foundWords.at(0);  // // min, здесь собираем пересечение множеств
        bool found = false;
        for (std::vector<Entry>::iterator iter = fw.begin(); iter != fw.end(); iter++) {    // doc с первым словом
            for (int i = 1; i < foundWords.size(); i++) {                                   // words 2 - n
                // по документам со 2 - n словом
                found = false;
                for (auto &it : foundWords.at(i)) {
                    if (it.docId == iter->docId) {
                        found = true;
                        iter->count += it.count;  // собираем abc релевантность
                        break;
                    }
                }
                if (!found) {
                    iter->count = 0;  // нет такого docId
                    break;
                }
            }
        }

        std::vector<Entry> absResponse;  //  выбираем док-ты со всеми словами
        for (auto& we : fw)
            if (we.count) {
                absResponse.push_back(we);
            }

        std::vector<RelativeIndex> response;
        float maxrel = 1;
        if (absResponse.size()) {
            sort(absResponse.begin(), absResponse.end(), [](auto en1, auto en2) {
                return en1.count > en2.count;  //сортировка  по убыванию
            });
            maxrel = (float)absResponse.begin()->count;

            int responsesCount = 0;      // cчетчик для responsesLimit
            for (auto docEntry : absResponse) {
                if (responsesCount++ >= responsesLimit) break;
                response.push_back({ docEntry.docId, docEntry.count / maxrel});
            }
        }

        std::cout << "Relative : ";  // для информации
        for (auto& an : response) std::cout << an.docId << ":" << an.rank << "\t";
        std::cout << "\n";

        responses.push_back(response);
    }
    return responses;
}
