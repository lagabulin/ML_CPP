#include <iostream>

#include "../dlib/dlib/matrix.h"

using namespace std;
using namespace dlib;

int main(){
    matrix<double> m(3,3);
    m = 54.2, 7.4, 12.1,
        1,    2,   3,
        5.9,  0.05,1;
    cout << m << endl;
    return 0;
}
