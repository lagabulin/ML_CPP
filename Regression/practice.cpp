#include <iostream>

#include "../dlib/dlib/matrix.h"

using namespace std;
using namespace dlib;

int main(){
    double data[] = {1,2,3,4,
                     5,6,7,8,
                     9,10,11,12,
                     13,14,15,16};
    matrix<double> m = mat(data,4,4);
    m(1,2) = 0;
    cout << m << '\n';
    
    matrix<double> I = identity_matrix<double>(3);
    cout << I << '\n';

    I = 2 * I;
    I = I * 5;

    matrix<double> ones = ones_matrix<double>(3,4);
    cout << ones << endl;

    cout << I * ones << '\n';

    auto M = randm(3,4);
    cout << M << '\n';
    cout << M + ones << '\n';
    
    auto mT = trans(M);
    cout << mT << endl;
    cout << M << endl;

    cout << "M matrix: " << endl << m << endl;
    auto rowsub = rowm(m, 0);
    cout << "rowm(m,0): " << endl << rowsub << endl;

    matrix<double> colsub = colm(m,2);
    cout << "col(m,2): " << endl << colsub << endl;

    matrix<double> matsub = subm(m,range(1,2),range(1,2));
    cout << "subm(m,range(1,2),range(1,2)): "<< endl << matsub << endl;

    matsub = subm(m,range(0,1),range(0,0));
    cout << "subm(m,range(0,1),0)" << endl << matsub << endl;

    set_subm(m,range(1,1),range(2,2)) = 7;
    cout << "After set_subm(m,range(1,1),range(2,2)) = 7" << endl << m << endl;

    set_subm(m,range(0,3),range(0,3)) = 1;
    cout << "Set every entry to one" << endl << m << endl;

    set_subm(m,range(0,3),range(0,3)) += 1;
    cout << "Increment every entry one" << endl << m << endl;

    set_colm(m,range(1,1)) += 4;
    cout << "add 4 to 2th col" << endl << m << endl;

    set_rowm(m,range(1,1)) += 2;
    cout << "add 2 to 2th row" << endl << m << endl;

    /* Below doesn't work */
    // set_subm(m,range(1,2), range(1,2)) *= 5;
    // cout << "mul 2th row by 2" << endl << m << endl;
    return 0;
}
