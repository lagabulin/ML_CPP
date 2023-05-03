// g++ -O3 -std=c++17 csv_dlib.cpp -o csv_dlib
// ./csv_dlib ./data/iris.data

#include <iostream>
#include <sstream>
// #include <fstream>
#include <regex>
#include <filesystem>

#include "../dlib/dlib/matrix.h"

namespace fs = std::filesystem;
using namespace std;
using namespace dlib;

int main(int argc, char** argv)
{
    if(argc > 1)
    {
        auto file_path = fs::path(argv[1]);
        if(fs::exists(file_path))
        {
            std::stringstream str_stream;
            
            std::ifstream data_stream(argv[1]);
            std::string data_string((std::istreambuf_iterator<char>(data_stream)),
                                    std::istreambuf_iterator<char>());
            data_string = std::regex_replace(data_string, std::regex("Iris-setosa"), "1");
            data_string = std::regex_replace(data_string, std::regex("Iris-versicolor"), "2");
            data_string = std::regex_replace(data_string, std::regex("Iris-virginica"), "3");
            
            str_stream << data_string;    

            matrix<double> data;
            str_stream >> data;

            cout << data << endl;

        }
    }
    else
    {
        std::cerr << "Please provide a path to a dataset\n"; 
    }
    return 0;
}