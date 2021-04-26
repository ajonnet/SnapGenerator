Raspi compile command
g++ `pkg-config --libs --cflags opencv4 libconfig++` -lboost_filesystem SnapGenerator/main.cpp -o snapGen


//ISSUES / TODO
//default config should be generated at the path where executable is located
//camStorePath to find device automatically
//CMake file to be created for Raspi build
