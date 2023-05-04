#ifndef JSON_HANDLING_H
#define JSON_HANDLING_H
#include <vector>
#include <string>
#include <memory> // unique_ptr

#include "../../rapidjson/error/en.h"
#include "../../rapidjson/filereadstream.h"
#include "../../rapidjson/reader.h"

using namespace std;

struct Paper
{
    uint32_t id{0};
    string preliminary_decision;
    vector<struct Review> reviews;
};

using Papers = vector<struct Paper>;

struct Review
{
    string confidence;
    string evaluation;
    uint32_t id{0};
    string language;
    string orientation;
    string remarks;
    string text;
    string timespan;
};

enum class HandlerState
{
    None,
    Global,
    PapersArray,
    Paper,
    ReviewArray,
    Review
};

struct ReviewsHandler
: public rapidjson::BaseReaderHandler<rapidjson::UTF8<>, ReviewsHandler>
{
    ReviewsHandler(Papers* papers) : papers_(papers){} // initialization list

    bool Uint(unsigned u);
    bool String(const char* str, rapidjson::SizeType length, bool /*copy*/);
    bool Key(const char* str, rapidjson::SizeType length, bool /*copy*/);
    bool StartObject();
    bool EndObject(rapidjson::SizeType /*memberCount*/);
    bool StartArray();
    bool EndArray(rapidjson::SizeType /*elementCount*/);

    struct Paper paper_;
    struct Review review_;
    string key_;
    Papers* papers_{nullptr};
    HandlerState state_{HandlerState::None};
};

Paper ReadPaperReview(const string& filename)
{
    // <Class T, Class Deleter>
    auto file = std::unique_ptr<FILE, void (*)(FILE*)>(
        fopen(filename.c_str(), "r"),
        [](FILE* f){if(f) ::fclose(f);}
    );

    if(file)
    {
        char readBuffer[65536];
        rapidjson::FileReadStream is(file.get(), readBuffer, sizeof(readBuffer));
        rapidjson::Reader reader;
        Papers papers;
        ReviewsHandler handler(&papers);
        auto res = read.Parse(is, handler);
        
        if(!res) throw std::runtime_error(rapidjson::GetParseError_En(res.Code()));
        return papers;
    }
    else
    {
        throw std::invalid_argument("File can't be opend " + filename);
    }
}

#endif