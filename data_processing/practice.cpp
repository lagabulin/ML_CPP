// g++ -O3 -std=c++17 practice.cpp -o practice
// ./practice ./data/iris.data
#include <iostream>
#include <vector>
#include <tuple>
#include <string>
#include <filesystem>

#include "../fast-cpp-csv-parser/csv.h"

using namespace std;

namespace fs = std::filesystem;

template <std::size_t... Idx, typename T, typename R>
bool read_row_helper(std::index_sequence<Idx...>, T& row, R& r)
{
    // r = csv_reader, type = io::CSVReader<[the number of columns]>
    return r.read_row(std::get<Idx>(row)...);
}

template <std::size_t... Idx, typename T>
void fill_values(std::index_sequence<Idx...>, T& row, vector<double>& data)
{
    data.insert(data.end(), {std::get<Idx>(row)...});
    // data.push_back({std::get<Idx>(row)...});
}

int main(int argc, char** argv)
{
    if(argc > 1)
    {
        auto path = fs::path(argv[1]);
        const uint32_t columns_num = 5;
        io::CSVReader<columns_num> csv_reader(path);
        
        vector<string> categorical_column;
        vector<double> values;

        using RowType = std::tuple<double, double, double, double, std::string>;
        RowType row;

        uint32_t row_num = 0;
        try
        {
            bool done = false; // done == true when cursor reaches EOF(because doen = !read_row)
            while(!done)
            {
                done = !read_row_helper(std::make_index_sequence<std::tuple_size<RowType>::value>{},row,csv_reader);
                if(!done)
                {
                    categorical_column.push_back(std::get<4>(row)); // Extract 5th element from tuple(row)
                    fill_values(std::make_index_sequence<columns_num - 1>{}, row, values);
                }
            }
            for(auto& v: values)
            {
                cout << v;
            }
        }
        catch(const io::error::no_digit& err)
        {
            std::cerr << err.what() << std::endl;
        }
    }
    else
    {
        cout << "Please provide a path to a dataset file\n";
    }



    return 0;
}