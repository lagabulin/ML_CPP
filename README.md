# Personal sandbox
- Initial Settings
- Dlib 19.09 C++
- Ubuntu 22.04 LTS
# Compile Command
- Run one of below in working directory
- Compile with all sources
    - `
g++ -std=c++17 -O3 -I.. ../dlib/dlib/all/source.cpp -lpthread -lX11 main.cpp
`
- Copile with specific sources
    - `
g++ -std=c++17 -O3 -lpthread -lX11 main.cpp
`