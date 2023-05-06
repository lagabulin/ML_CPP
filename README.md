# Personal Sandbox
- Libraries
    - Dlib 19.09 C++
    - Fast-CPP-CSV-parser(Headers-only lib)
    - Plotcpp(Headers-only lib)
    - RapidJson
- Operating System & Compiler
    - Ubuntu 22.04 LTS
    - g++ (Ubuntu 11.3.0-1ubuntu1~22.04) 11.3.0
# Compile Command
- Run one of below in working directory
- Compile with all sources
    - `
g++ -std=c++17 -O3 -I.. ../dlib/dlib/all/source.cpp -lpthread -lX11 {code file name}.cpp -o {executable name}
`
- Compile with X-window(GUI)
    - `
g++ -std=c++17 -O3 -lpthread -lX11 {code file name}.cpp -o {executable name}
`
- Ordinary Compilation
    - `
g++ -std=c++17 -O3 -lpthread {code file name}.cpp -o {executable name}
`