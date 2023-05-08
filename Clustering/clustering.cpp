// g++ -O3 -std=c++17 clustering.cpp -o clustering
// ./clustering ./data
#include <iostream>
#include <filesystem>
#include <unordered_map>
#include <vector>
#include <string>
#include <set>

#include "../dlib/dlib/matrix.h"
#include "../dlib/dlib/clustering.h"
#include "../plotcpp/plot.h"

using namespace dlib;
namespace fs = std::filesystem;
using SampleType = dlib::matrix<double,1,1>;
using samples = std::vector<SampleType>;

const std::vector<std::string> data_names = {"dataset0.csv","dataset1.csv","dataset2.csv",
                                            "dataset3.csv","dataset4.csv","dataset5.csv"};
const std::vector<std::string> colors = {"black", "red", "blue", "green", "cyan", "yellow", "brown","magenta"};

using DataType = double;
using Coords = std::vector<DataType>;
using PointCoords = std::pair<Coords,Coords>;
using Clusters = std::unordered_map<size_t, PointCoords>; // first = color index, second = coordinates

void PlotClusters(const Clusters& clusters, const std::string& name, const std::string& file_name)
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
    for(auto& cluster: clusters)
    {
        std::stringstream params;
        params << "lc rgb '" << colors[cluster.first] << "' pt 7";
        // plotcpp::Points(startX, endX, startX, gnu visual style configuration) -> endy is not needed
        plt.AddDrawing(draw_state, 
                        plotcpp::Points(
                            cluster.second.first.begin(), cluster.second.first.end(),
                            cluster.second.second.begin(), std::to_string(cluster.first) + " cls", params.str()));
    }

    plt.EndDraw2D(draw_state);
    plt.Flush();
}

template <typename I>
void DoKMeansClustering(const I& inputs, size_t num_clusters, const std::string& name)
{
    typedef matrix<double, 2, 1> sample_type;
    // typedef polynomial_kernel<sample_type> kernel_type;
    // kcentroid<kernel_type> kc(kernel_type(1,0,2),0.01,8);
    typedef radial_basis_kernel<sample_type> kernel_type;
    kcentroid<kernel_type> kc(kernel_type(0.1),0.01,8);
    kkmeans<kernel_type> kmeans(kc);

    std::vector<sample_type> samples;
    samples.reserve(inputs.nr());
    for(long i = 0; i != inputs.nr(); ++i)
    {
        samples.push_back(dlib::trans(dlib::subm(inputs, i, 0,1,2)));
    }

    std::vector<sample_type> initial_centers;
    pick_initial_centers(num_clusters, initial_centers, samples, kmeans.get_kernel());

    kmeans.set_number_of_centers(num_clusters);
    kmeans.train(samples, initial_centers);

    std::vector<unsigned long> clusters;
    Clusters plot_clusters;

    for(long i = 0; i != inputs.nr(); ++i)
    {
        // kmeans(...) -> return idx of cluster owning sample
        auto cluster_idx = kmeans(samples[i]);

        // plot_clusters is hashmap, so [] operator generates node for cluster_idx automatically.
        plot_clusters[cluster_idx].first.push_back(inputs(i,0));
        plot_clusters[cluster_idx].second.push_back(inputs(i,1));
    }
    PlotClusters(plot_clusters, "K-Means", name + "-kmeans.png");
}

int main(int argc, char** argv)
{
    if(argc >1)
    {
        auto path = fs::path(argv[1]);
        for(auto& dataset : data_names)
        {
            // fs path operator '/'
            auto dataset_name = path / dataset;
            if(fs::exists(dataset_name))
            {
                std::ifstream file(dataset_name);
                matrix<DataType> data;
                file >> data;

                auto inputs = dlib::subm(data, 0, 1, data.nr(), 2);
                auto labels = dlib::subm(data, 0, 3, data.nr(), 1);

                auto num_samples = inputs.nr();
                auto num_features = inputs.nc();

                size_t num_clusters = std::set<double>(labels.begin(), labels.end()).size();

                num_clusters = (num_clusters < 2) ? 3 : num_clusters;

                std::cout << dataset << "\n"
                          << "Num Samples: " << num_samples
                          << " num features: " << num_features
                          << " num clusters: " << num_clusters << std::endl;
                DoKMeansClustering(inputs, num_clusters, dataset);
            }
        }
    }
    else
    {
        std::cout << "Please provide path to the datasets directory\n";
    }
    return 0;
}