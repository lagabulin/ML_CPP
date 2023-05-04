#ifndef JSON_HANDLING_H
#define JSON_HANDLING_H

#include <vector>
#include <string>

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

#endif