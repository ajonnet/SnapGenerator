Raspi compile command
g++ `pkg-config --libs --cflags opencv4 libconfig++` -lboost_filesystem SnapGenerator/main.cpp -o snapGen