// g++ -O3 -std=c++17 -I.. ../dlib/dlib/all/source.cpp -lpthread -lX11 classification.cpp -o classification
// ./classification ../Clustering/data
#include <iostream>
#include <filesystem>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <stdexcept>

#include "../dlib/dlib/matrix.h"
#include "../dlib/dlib/svm_threaded.h"
#include "../plotcpp/plot.h"

namespace fs = std::filesystem;
using namespace dlib;

//uniform initialization
const std::vector<std::string> data_names{"dataset0.csv","dataset1.csv","dataset2.csv",
                                          "dataset3.csv","dataset4.csv"};

const std::vector<std::string> colors{"red","green","blue","cyan","black"};

using DataType = double;
using Coords = std::vector<DataType>;
using PointCoords = std::pair<Coords, Coords>;
using Classes = std::map<size_t, PointCoords>;

using SampleType = matrix<DataType, 2, 1>;
using Samples = std::vector<SampleType>;
using Labels = std::vector<DataType>;

void PlotClusters(const Classes& classes,
                  const std::string& name,
                  const std::string& file_name)
{
    plotcpp::Plot plt;
    plt.SetTerminal("png");
    plt.SetOutput(file_name);
    plt.SetTitle(name);
    plt.SetXLabel("x");
    plt.SetYLabel("y");
    plt.SetAutoscale();
    plt.GnuplotCommand("set grid");

    auto draw_state = plt.StartDraw2D<Coords::const_iterator>();
    for(auto& cls: classes)
    {
        std::stringstream params;
        params << "lc rgb '" << colors[cls.first] << "' pt 7";
        plt.AddDrawing(draw_state,
                       plotcpp::Points(cls.second.first.begin(), cls.second.first.end(),
                                       cls.second.second.begin(),
                                       std::to_string(cls.first) + " cls", params.str()));
    }
    plt.EndDraw2D(draw_state);
    plt.Flush();
}

void KRRClassification(const Samples& samples, const Labels& labels,
                       const Samples& test_samples, const Labels& test_labels,
                       const std::string& name)
{
    using OVOtrainer = one_vs_one_trainer<any_trainer<SampleType>>;
    using KernelType = radial_basis_kernel<SampleType>;

    krr_trainer<KernelType> krr_trainer;
    krr_trainer.set_kernel(KernelType(0.1));

    OVOtrainer trainer;
    trainer.set_trainer(krr_trainer);

    one_vs_one_decision_function<OVOtrainer> df = trainer.train(samples, labels);

    Classes classes;
    DataType accuracy = 0.0;
    for(size_t i = 0; i < test_samples.size(); ++i)
    {
        auto vec = test_samples[i];
        auto class_idx = static_cast<size_t>(df(vec));
        if(static_cast<size_t>(test_labels[i]) == class_idx) ++accuracy; // If classification was correct
        auto x = vec(0,0);
        auto y = vec(1,0);
        classes[class_idx].first.push_back(x);
        classes[class_idx].second.push_back(y);
    }

    accuracy /= test_samples.size();

    PlotClusters(classes, "Kernel Ridge Regression " + std::to_string(accuracy), name+"-krr-dlib.png");

}

void SVMClassification(const Samples& samples, const Labels& labels,
                       const Samples& test_samples, const Labels& test_labels,
                       const std::string& name)
{
    using OVOtrainer = one_vs_one_trainer<any_trainer<SampleType>>;
    using KernelType = radial_basis_kernel<SampleType>;

    svm_nu_trainer<KernelType> svm_trainer;
    svm_trainer.set_kernel(KernelType(0.1));

    OVOtrainer trainer;
    trainer.set_trainer(svm_trainer);

    one_vs_one_decision_function<OVOtrainer> df = trainer.train(samples, labels);

    Classes classes;
    DataType accuracy = 0;
    for(size_t i = 0; i < test_samples.size(); ++i)
    {
        auto vec = test_samples[i];
        auto class_idx = static_cast<size_t>(df(vec)); // classification on test set
        if(static_cast<size_t>(test_labels[i]) == class_idx) ++accuracy;
        auto x = vec(0,0);
        auto y = vec(1,0);

        classes[class_idx].first.push_back(x);
        classes[class_idx].second.push_back(y);
    }

    accuracy /= test_samples.size();

    PlotClusters(classes, "SVM " + std::to_string(accuracy), name +  "-svm-dlib.png");

}

int main(int argc, char** argv)
{
    if(argc > 1)
    {
        auto dir_path = fs::path(argv[1]);
        for(auto& file_name : data_names)
        {
            auto full_path = dir_path / file_name;
            if(fs::exists(full_path))
            {
                std::ifstream file(full_path);
                matrix<DataType> data;
                file >> data;
                
                // initializer r, c, numRow, numCol: first column is index column of samples.
                auto inputs = dlib::subm(data, 0,1, data.nr(), 2);
                auto outputs = dlib::subm(data, 0, 3, data. nr(), 1); // the third column is label column of samples

                auto num_samples = inputs.nr();
                auto num_features = inputs.nc();

                std::size_t num_classes = std::set<double>(outputs.begin(), outputs.end()).size();

                std::cout << file_name << "\n"
                          << "Num samples: " << num_samples
                          << " num features: " << num_features
                          << " num classes: " << num_classes << std::endl;
                
                // data seperation (test set | training set)
                // 0 ~ 299 test | 300 ~ training
                long test_num = 300;
                Samples test_samples;
                Labels test_labels;
                {
                    for(long row = 0; row < test_num; ++row)
                    {
                        test_samples.emplace_back(dlib::reshape_to_column_vector(
                            dlib::subm_clipped(inputs, row, 0, 1, data.nc())
                        ));

                        test_labels.emplace_back(outputs(row,0));
                    }
                }

                std::vector<SampleType> samples;
                Labels labels;
                {
                    for(long row = test_num; row < inputs.nr(); ++row)
                    {
                        samples.emplace_back(dlib::reshape_to_column_vector(
                            dlib::subm_clipped(inputs, row, 0, 1, data.nc())
                        ));

                        labels.emplace_back(outputs(row,0));
                    }
                }

                SVMClassification(samples, labels, test_samples, test_labels, file_name);
                KRRClassification(samples, labels, test_samples, test_labels, file_name);
            }
            else
            {
                throw std::invalid_argument("Provide valid path of data files");
            }
        }
    }
    else
    {
        throw std::invalid_argument("Provide path of data directory");
    }
    return 0;
}