#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <regex>
#include <sstream>

#include "../dlib/dlib/matrix.h"
#include "../dlib/dlib/dnn.h"
using namespace std;
using namespace dlib;
namespace fs = std::filesystem;

int main(int argc, char** argv){
    if(argc >1)
    {
        auto path = fs::path(argv[1]);
        if(fs::exists(path))
        {

            std::stringstream str_stream;

            std::ifstream data_stream(argv[1]);
            std::string data_string((std::istreambuf_iterator<char>(data_stream)),
                                    std::istreambuf_iterator<char>());
            data_string = std::regex_replace(data_string, std::regex("Iris-setosa"),"1");
            data_string = std::regex_replace(data_string, std::regex("Iris-versicolor"),"2");
            data_string = std::regex_replace(data_string, std::regex("Iris-virginica"),"3");

            str_stream << data_string;

            matrix<double> data;
            str_stream >> data;


            std::vector<matrix<double>> samples;
            size_t numRows = data.nr();
            size_t numCols = data.nc();
            samples.reserve(numRows);
            for(size_t r = 0; r < numRows; ++r)
            {
                samples.emplace_back(rowm(data,r));
            }

            // matrix<matrix<double>> is required. matrix<double> doesn't work; 
            // m(mean(data)) --> (X)
            matrix<double> m(mean(mat(samples)));
            matrix<double> sd(reciprocal(stddev(mat(samples))));

            for(size_t r = 0; r < numRows; ++r)
            {
                samples[r] = pointwise_multiply(samples[r] - m, sd);
            }
            cout << mat(samples) << '\n';

            cout << m << '\n';
            cout << sd << '\n';
        }
    }
    else
    {
        cout << "Provide the address of csv file\n";
    }
    return 0;
}