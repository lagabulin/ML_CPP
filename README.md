# Personal Sandbox
- Settings
    - Dlib 19.09 C++
    - Fast-CPP-CSV-parser(Headers-only lib)
    - plotcpp(Headers-only lib)
    - Ubuntu 22.04 LTS
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