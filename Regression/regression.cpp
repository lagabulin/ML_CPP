/* g++ -O3 -std=c++17 regression.cpp -o regression */
#include <iostream>
#include <vector>
#include "../dlib/dlib/svm.h"

using namespace std;
using namespace dlib;
int main()
{
    /* 
        trainer requires column vector, so below(row vector) is not allowed
        typedef matrix<double, 1,3> vars;
    */
    typedef matrix<double, 3,1> vars;
    typedef linear_kernel<vars> KernelType;

    std::vector<vars> x;
    std::vector<double> y;

    for(double i=1.0; i < 11.0; i+=1)
    {
        double data[] = {1.0,i,i*i};
        vars m = mat(data,3,1);
        x.push_back(m);
        y.push_back(4*data[0] + 4*i + i*i);
    }

    krr_trainer<KernelType> trainer;
    trainer.set_kernel(KernelType());
    decision_function<KernelType> df = trainer.train(x,y);

    /* df should be y = x^2 + 4x + 4 */

    double testset[] = {1,12,144};
    matrix<double> test = mat(testset,3,1);
    
    /* prediction should be 196 */
    auto prediction = df(test);
    cout << prediction << '\n';
    return 0;
}