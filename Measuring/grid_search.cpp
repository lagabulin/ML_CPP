// g++ -O3 -std=c++17 -I.. ../dlib/dlib/all/source.cpp -lpthread -lX11 grid_search.cpp -o grid_search
// ./grid_search
#include <iostream>
#include <random>
#include <vector>

#include "../dlib/dlib/matrix.h"
#include "../dlib/dlib/global_optimization.h"
#include "../dlib/dlib/svm.h"
#include "../plotcpp/plot.h"

using SampleType = dlib::matrix<double, 1,1>; // 2d data (x,y)
using Samples = std::vector<SampleType>;
using Labels = std::vector<SampleType>;

Samples LinSpace(double s, double e, size_t n)
{
    Samples x_values(n);
    double step = (e-s)/n;
    double v = s;
    for(size_t i = 0; i < n; ++i)
    {
        x_values[i](0,0) = v;
        v += step;
    }
    x_values[n-1] = e;

    return x_values;
}

std::pair<Samples, Labels> GenerateData(size_t num_samples, size_t seed)
{
    // create vectors for samples and labels
    Samples x_values(num_samples);
    Labels y_values(num_samples);

    std::mt19937 re(seed);
    std::normal_distribution<double> dist; // for random noise

    for(size_t i =0; i<num_samples ; ++i)
    {
        auto x_val = dist(re);
        x_values[i](0,0) = x_val;

        auto y_val = std::cos(M_PI * x_val) + (dist(re) * 0.3);
        y_values[i](0,0) = y_val;
    }
    return {x_values, y_values};
}

int main(){

    using namespace dlib;
    Samples samples;
    Labels labels;
    std::tie(samples, labels) = GenerateData(1000, 435635);

    // minmax normalization
    // lambda function is needed to sort matrices
    auto mm = std::minmax_element(
        samples.begin(),
        samples.end(),
        [](const auto& a, const auto& b) { return a(0,0) <  b(0,0);}
    );

    std::pair<double,double> x_minmax{*mm.first, *mm.second};
    
    vector_normalizer<SampleType> x_normalizer;
    x_normalizer.train(samples);
    std::for_each(samples.begin(), samples.end(), x_normalizer);


    vector_normalizer<SampleType> y_normalizer;
    y_normalizer.train(labels);
    std::for_each(labels.begin(), labels.end(), y_normalizer);

    std::vector<double> raw_labels(labels.size());
    for(size_t i = 0; i<labels.size(); ++i)
    {
        raw_labels[i] = labels[i](0,0);
    }

    randomize_samples(samples, raw_labels);

    auto CrossValidationScore = [&](const double gamma, const double c, const double degree_in){
        auto degree = std::floor(degree_in);
        using KernelType = dlib::polynomial_kernel<SampleType>;
        dlib::svr_trainer<KernelType> trainer;
        trainer.set_kernel(KernelType(gamma, c, degree));

        std::cout << "gamma: " << std::setw(11) << gamma << " c: " << std::setw(11) << c << " degree: " << std::setw(11) << degree;
        std::cout.flush();

        dlib::matrix<double> result = dlib::cross_validate_regression_trainer(trainer, samples, raw_labels, 10);
        std::cout << std::setw(11) << " MSE: " << result(0,0) << std::setw(11) << " MAE: " << result(0,2) << std::endl;

        return result(0,0);
    };

    auto result  = find_min_global(
        CrossValidationScore,
        {0.01, 1e-8, 5},
        {0.1, 1, 15},
        max_function_calls(50)
    );

    double gamma = result.x(0);
    double c = result.x(1);
    double degree = result.x(2);

    std::cout << " best cross validation score: " << result.y << std::endl;
    std::cout << " best gamma: " << gamma << "   best c: " << c << "    best degree: " << degree << std::endl;

    using KernelType = dlib::polynomial_kernel<SampleType>;
    dlib::svr_trainer<KernelType> trainer;
    trainer.set_kernel(KernelType(gamma, c, degree));
    auto decision_func = trainer.train(samples, raw_labels);

    // new samples with normalization
    auto new_samples = LinSpace(x_minmax.first, x_minmax.second, 50);
    std::for_each(new_samples.begin(), new_samples.end(), x_normalizer);

    // apply decision_func on new_samples and store them in predictions
    std::vector<double> predictions(new_samples.size());
    std::transform(new_samples.begin(), new_samples.end(), predictions.begin(), decision_func);

    plotcpp::Plot plt;
    plt.SetTerminal("png");
    plt.SetOutput("grid_search.png");
    plt.SetTitle("SVM polynomial kernel regression");
    plt.SetXLabel("x");
    plt.SetYLabel("y");
    plt.SetAutoscale();
    plt.GnuplotCommand("set grid");

    std::vector<double> x_coords(samples.size());
    std::transform(samples.begin(), samples.end(), x_coords.begin(), [](auto& m) {return m(0,0);});

    std::vector<double> x_pred_coords(new_samples.size());
    std::transform(new_samples.begin(), new_samples.end(), x_pred_coords.begin(), [](auto& m) {return m(0,0);});
    
    plt.Draw2D(plotcpp::Points(x_coords.begin(), x_coords.end(), raw_labels.begin(), "orig", "lc rgb 'black' pt 7"),
                plotcpp::Lines(x_pred_coords.begin(), x_pred_coords.end(), predictions.begin(), "pred", "lc rgb 'red' lw 2"));
    plt.Flush();

    return 0;
}
