sudo apt-get install libstdc++-14-dev

cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local\
    -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++