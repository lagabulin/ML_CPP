#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <unordered_map>
#include <stdexcept>

#include "../dlib/dlib/matrix.h"
#include "../dlib/dlib/svm.h"
#include "../plotcpp/plot.h"

namespace fs = std::filesystem;
using namespace dlib;

const std::vector<std::string> colors{"black", "red", "blue", "green", "cyan", "yellow", "brown", "magenta"};
using DataType = double;
using Matrix = matrix<DataType>;
using Coords = std::vector<DataType>;
using PointCoords = std::pair<Coords, Coords>;
using Clusters = std::unordered_map<size_t, PointCoords>; // cluster_idx, cluster

void PlotClusters(const Clusters& clusters, const std::string& name, const std::string& file_name)
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
        // startX endX startY visualization_style_configuration
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

//Matrix := matrix<double>
void MultivariateGaussianDist(const Matrix& normal,
                              const Matrix& test,
                              const std::string& file_name)
{
    // set mean vector
    dlib::matrix<double> mu(1, normal.nc());
    dlib::set_all_elements(mu,0);

    for(long c = 0; c< normal.nc(); ++c)
    {
        auto col_mean = dlib::mean(dlib::colm(normal, c));
        dlib::set_colm(mu, c) = col_mean;
    }

    // set covariance matrix
    dlib::matrix<double> cov(normal.nc(), normal.nc());
    dlib::set_all_elements(cov,0);

    // covariance between each rows(sample)
    // row vector -> (r - mu).T @ (r - mu) = (col - mu.T) @ (col - mu.T).T
    for(long r = 0; r< normal.nr(); ++r)
    {
        auto row = dlib::rowm(normal, r);
        cov += dlib::trans(row - mu) * (row - mu);
    }
    cov *= 1.0 / normal.nr();
    double cov_det = dlib::det(cov);
    dlib::matrix<double> cov_inv = dlib::inv(cov);

    // Fixed part: 1 / [(2 pi)^(n/2) * (det(cov))^(1/2)]
    auto first_part = 
        1.0 / std::pow(2.0 * M_PI, normal.nc() / 2.0) / std::sqrt(cov_det);
    
    // Depends on sample: exp( -0.5 * [(x - mu).T @ cov.inv @ (x - mu)] )
    auto prob = [&](const dlib::matrix<double>& sample){
        dlib::matrix<double> s = sample - mu;
        dlib::matrix<double> exp_val_m = s * (cov_inv * dlib::trans(s));
        double exp_val = -0.5 * exp_val_m(0,0);
        double p = first_part * std::exp(exp_val);
        return p;
    };

    // hashmap
    Clusters clusters;

    double prob_threshold = 0.001;

    auto detect = [&](auto samples){
        for(long r = 0; r< samples.nr(); ++r)
        {
            auto row = dlib::rowm(samples, r);
            double x = row(0,0);
            double y = row(0,1);
            auto p = prob(row);
            if(p >= prob_threshold)
            {
                clusters[0].first.push_back(x);
                clusters[0].second.push_back(y);
            }
            else
            {
                clusters[1].first.push_back(x);
                clusters[1].second.push_back(y);
            }
        }
    };


    detect(normal);
    detect(test);
    PlotClusters(clusters, "Multivariate Gaussaian Distribution", file_name);
}

//Matrix := matrix<double>
void OneClassSvm(const Matrix& normal,
                 const Matrix& test,
                 const std::string& file_name)
{
    typedef matrix<double,0,1> sample_type;
    typedef radial_basis_kernel<sample_type> kernel_type;
    svm_one_class_trainer<kernel_type> trainer;
    trainer.set_nu(0.5);
    trainer.set_kernel(kernel_type(0.5));
    
    // filled with samples of row vector form
    std::vector<sample_type> samples;
    for(long r= 0; r<normal.nr(); ++r)
    {
        auto row = rowm(normal, r);
        samples.push_back(row);
    }

    decision_function<kernel_type> df = trainer.train(samples);
    Clusters clusters;
    double threshold = -2.0;
    auto detect = [&](auto target_samples) {
        for(long r= 0; r< target_samples.nr(); ++r)
        {
            auto row = dlib::rowm(target_samples,r);
            double x = row(0,0);
            double y = row(0,1);
            auto p = df(row);
            if(p > threshold)
            {
                clusters[0].first.push_back(x);
                clusters[0].second.push_back(y);
            } else{
                clusters[1].first.push_back(x);
                clusters[1].second.push_back(y);
            }
        }
    };

    detect(normal);
    detect(test);
    PlotClusters(clusters, "One Class SVM", file_name);
}

//Matrix := matrix<double>;
using Dataset = std::pair<Matrix, Matrix>;

Dataset LoadDataset(const fs::path& file_path)
{
    if(fs::exists(file_path))
    {
        std::ifstream file(file_path);
        matrix<DataType> data;
        file >> data;

        long n_normal = 50;
        Matrix normal = dlib::subm(data, range(0,n_normal - 1), range(0,data.nc()-1));
        Matrix test = dlib::subm(data, range(n_normal, data.nr() -1), range(0, data.nc() - 1));

        return {normal,test};
    }
    else
    {
        throw std::invalid_argument("Provide valied file path");
    }
}

int main(int argc, char** argv)
{
    if(argc > 1)
    {
        auto path = fs::path(argv[1]);
        std::string data_name_multi{"multivar.csv"};
        std::string data_name_uni{"univar.csv"};

        fs::path path_multi = path / data_name_multi;
        fs::path path_uni = path / data_name_uni;
        if(fs::exists(path_multi) && fs::exists(path_multi))
        {
            auto dataset_multi = LoadDataset(path_multi);
            auto dataset_uni = LoadDataset(path_uni);

            OneClassSvm(dataset_multi.first, dataset_multi.second, "dlib-ocsvm.png");
            MultivariateGaussianDist(dataset_multi.first, dataset_multi.second, "dlib-multi-var.png");
        }
    }
    else
    {
        std::cout << "Provide path of file directory";
    }
    return 0;
}