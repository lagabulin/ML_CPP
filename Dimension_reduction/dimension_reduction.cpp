#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include <filesystem>
#include <unordered_map>

#include "../dlib/dlib/matrix.h"
#include "../dlib/dlib/matrix/matrix_utilities.h"
#include "../dlib/dlib/statistics.h"
#include "../plotcpp/plot.h"

namespace fs = std::filesystem;
using namespace dlib;
using DataType = double;
using Matrix = matrix<DataType>;
using Coords = std::vector<DataType>;
using PointCoords = std::pair<Coords,Coords>;
using Clusters = std::unordered_map<size_t, PointCoords>;

const std::vector<std::string> colors{"black", "red", "blue", "green", "cyan", "yellow", "brown", "magenta"};

void PlotClusters(const Clusters& clusters,
                  const std::string& name,
                  const std::string& file_name)
{
    plotcpp::Plot plt;
    plt.SetTerminal("png");
    plt.SetOutput(file_name);
    plt.SetTitle(name);
    plt.SetXLabel("x");
    plt.SetYLabel("y");
    plt.GnuplotCommand("set size square");
    plt.GnuplotCommand("set grid");

    auto draw_state = plt.StartDraw2D<Coords::const_iterator>();
    for(auto& cluster : clusters)
    {
        std::stringstream params;
        params << "lc rgb '" << colors[cluster.first] << "' pt 7";
        plt.AddDrawing(draw_state,
                       plotcpp::Points(
                            cluster.second.first.begin(), cluster.second.first.end(),
                            cluster.second.second.begin(),
                            std::to_string(cluster.first) + " cls", params.str()
                       ));
    }
    plt.EndDraw2D(draw_state);
    plt.Flush();
}

void PCAreduction(const std::vector<Matrix>& data,
                  const std::vector<unsigned long>& labels,
                  int feature_dim)
{
    dlib::vector_normalizer_pca<Matrix> pca;
    pca.train(data, feature_dim/data[0].nr());
    std::vector<Matrix> new_data;
    new_data.reserve(data.size());
    for(size_t i =0; i < data.size(); ++i)
    {
        new_data.emplace_back(pca(data[i]));
    }

    Clusters clusters;
    for(size_t r = 0; r< new_data.size(); ++r)
    {
        Matrix vec = new_data[r];
        double x = vec(0, 0);
        double y = vec(1, 0);
        auto l = labels[r]; // cluster_idx == label
        clusters[l].first.push_back(x);
        clusters[l].second.push_back(y);
    }
    PlotClusters(clusters, "PCA", "pca.png");
}

// LDA(compute_lda_tranform) requires Matrix not vector<matrix> form;
void LDAreduction(const Matrix& data,
                  const std::vector<unsigned long>& labels,
                  int feature_dim)
{
    dlib::matrix<DataType, 0, 1> mean;
    Matrix transform = data;
    
    // generate transform matrix and fill mean
    dlib::compute_lda_transform(transform, mean, labels, feature_dim);

    Clusters clusters;
    for(size_t r = 0; r < data.nr(); ++r)
    {
        Matrix vec = transform * dlib::trans(dlib::rowm(data,r)) - mean;
        double x = vec(0,0);
        double y = vec(1,0);
        auto l = labels[r];
        clusters[l].first.push_back(x);
        clusters[l].second.push_back(y);
    }
    PlotClusters(clusters, "LDA", "lda.png");
    
}

int main(int argc, char** argv)
{
    if(argc > 1)
    {
        auto dir = fs::path(argv[1]);
        std::string roll_data{"swissroll.dat"};
        std::string roll_labels{"swissroll_labels.dat"};
        auto data_path = dir /  roll_data;
        auto label_path = dir /  roll_labels;
        if(fs::exists(data_path) && fs::exists(label_path))
        {
            dlib::matrix<DataType> data;
            std::vector<Matrix> vdata;
            {
                std::ifstream file(data_path);
                file >> data;
                vdata.reserve(static_cast<size_t>(data.nr()));
                for(long row = 0; row < data.nr(); ++row)
                {
                    vdata.emplace_back(dlib::reshape_to_column_vector(
                        dlib::subm_clipped(data,row,0,1,data.nc())
                    ));
                }
            }
            dlib::matrix<DataType> labels;
            std::vector<unsigned long> vlabels;
            {
                std::ifstream file(label_path);
                file >> labels;
                vlabels.reserve(static_cast<size_t>(labels.nr()));
                for(long row = 0; row < labels.nr(); ++row)
                {
                    vlabels.emplace_back(static_cast<unsigned long>(labels(row,0)));
                }
            }
            // dimension of the feature space
            int feature_dim = 2;
            PCAreduction(vdata, vlabels, feature_dim);
            LDAreduction(data, vlabels, feature_dim);
            // Sammonreduction(vdata,vlabels, feature_dim);
        }
        else
        {
            throw std::invalid_argument("Provide valid path of files");
        }
    }
    else
    {
        throw std::invalid_argument("Provide path of data directory");
    }

    return 0;
}