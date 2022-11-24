
#include "ConverterJSON.h"

std::vector<std::string> ConverterJSON::GetTextDocuments() {
    // Загрузка текстовых файлов
    std::vector<std::string> texts;
    for (auto path : files) {
        try {
            std::ifstream textFile(path);
            if (!textFile)
                throw std::runtime_error(("Unable to open file " + path + "\n").c_str());
            std::string text;
            textFile.seekg(0, std::ios_base::end);                         // размер файла
            text.resize(textFile.tellg());                              // текстовый буфер
            textFile.seekg(0, std::ios_base::beg);
            textFile.read((char*)text.data(), text.size());       // читаем данные в буфер
            textFile.close();
            texts.push_back(text);
        }
        catch (const std::runtime_error& ex) {
            std::cerr << "\n" << SearchServerName << " - " << ex.what();
        }
    }
    return texts;
}

nlohmann::json ConverterJSON::readConfig() {
    // читаем файл настроек => nlohmann::json config;
    nlohmann::json config;
    try {
        std::ifstream file("config.json");
        if (!file) {
            throw std::runtime_error("Unable to open config.json\n");
        }
        file >> config;
        file.close();
        if (config["config"].is_null()) {
            throw std::runtime_error("Config is_null\n");
        }
    }

    catch (const std::runtime_error& ex) {
        std::cerr << "\n" << SearchServerName << " - " << ex.what();
        exit(-1);
    }
    return (config);
}


int ConverterJSON::GetResponsesLimit() {
    auto config = readConfig();       // читаем файл настроек
    if (config["config"]["maxResponses"].is_null())
        std::cerr <<"Config maxResponses is_null, use default\n";
    else
        responsesLimit = config["config"]["maxResponses"];   // устанавливаем значение поля класса

    return responsesLimit;
}

void ConverterJSON::GetConfig() {
    // читаем все настройки
    GetResponsesLimit();
    auto config = readConfig();       // читаем файл настроек

    if (config["config"]["version"].is_null())
        std::cerr << "Config version is_null, current version - " << CurrentVersion << "\n";
    else {
        version = config["config"]["version"];
        if(version != CurrentVersion)
            std::cerr << "Config version - " <<  version << ", current version - " << CurrentVersion << "\n";
    }

    if (config["config"]["name"].is_null())
        std::cerr << "Config name is_null, use SearchServer default\n";
    else
        SearchServerName = config["config"]["name"];

    files.empty();
    for (auto& file : config["files"])
        files.push_back(file);
}

std::vector<std::string> ConverterJSON::GetRequests() {
    // читаем файл requests
    std::ifstream file("requests.json");
    if (!file)
        throw std::runtime_error("Unable to open requests.json\n");
    nlohmann::json requests;
    file >> requests;
    file.close();

    if (requests["requests"].is_null())   // throw
        throw std::runtime_error("Requests is_null\n");

    //собираем заросы
    std::vector<std::string> queries;
    for (auto& request : requests["requests"])
        queries.push_back(request);

    return queries;
}

void ConverterJSON::putAnswers(std::vector<std::vector<RelativeIndex>> responses) {

    // Создание файла answers.json
    // вспоьогательные словари
    nlohmann::json fileAnswers;
    nlohmann::json answers;
    nlohmann::json relevance;
    char num[8]{};

    for (int i = 0; i < responses.size(); i++) {  // по запросам
        nlohmann::json answer;  // словать ответа по текущему запросу
        answer["result"] = responses.at(i).size() ? "true" : "false";

        if (responses.at(i).size() == 1) {
            answer["docid"] = responses.at(i).at(0).docId;
            answer["rank"] = (float)responses.at(i).at(0).rank;
        }

        if (responses.at(i).size() > 1) {
            std::vector<nlohmann::json> vectorRels;  // для накопления relevance
            // по документам
            for (int j = 0; j < responses.at(i).size();j++) {
                relevance["docid"] = responses.at(i).at(j).docId ;  //
                sprintf_s(num, "%1.3f", responses.at(i).at(j).rank);   // убираем огрехи float json
                relevance["rank"] = (double)atof(num);

                vectorRels.push_back(relevance);
            }
            nlohmann::json relevances(vectorRels);
            answer["relevance"] = relevances;
        }
        sprintf_s(num, "%03d", i + 1);
        answers["request" + std::string(num)] = answer;
    }

    // to file answers.json
    fileAnswers["answers"] = answers;
    std::ofstream file("answers.json");
    file << fileAnswers;
    file.close();

}

