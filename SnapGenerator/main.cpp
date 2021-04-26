//
//  main.cpp
//  SnapGenerator
//
//  Created by Amit Jain on 22/04/21.
//  Copyright © 2021 ajonnet. All rights reserved.
//

#include <iostream>
#include <boost/filesystem.hpp>
#include <opencv2/opencv.hpp>
#include <libconfig.h++>

#define APP_VER "0.1"

using namespace std;
using namespace libconfig;
namespace fs = boost::filesystem;

//Config Params
string configFPath = "config.cfg";
string storagePath = "/Users/Amit/Desktop/SnapGenerator/Storage";
string outputPath = "/Users/Amit/Desktop/SnapGenerator/Output";
const string camStorePath = "xiaomi_camera_videos/04cf8c70d5de";

//Generates snap File name, by fetching date time info from its path
string genSnapFName(fs::path vidFilePath);
void initFromConfigFile();
void processCmdLineArgs(int argc, const char * argv[]);


int main(int argc, const char * argv[]) {
    
    processCmdLineArgs(argc,argv);
    initFromConfigFile();

    //List all Video files at the storage path
    fs::path vidsFilePath = fs::path(storagePath) / fs::path(camStorePath);
    fs::directory_iterator end;
    
    //Fetching all hourMin Folders
    vector<fs::path> hourMinDirs;
    for(fs::directory_iterator item(vidsFilePath); item != end; ++item) {
        const fs::path iItem = (*item);
        if(fs::is_directory(iItem)) hourMinDirs.push_back(iItem);
    }
    
    //Fetching video files from each hourMin Folders
    vector<fs::path> vidFiles;
    for(auto hourMinDir : hourMinDirs) {
        for(fs::directory_iterator item(hourMinDir); item!=end; ++item) {
            const fs::path iItem = (*item);
            
            //Skip video files for which Snap is already generated
            fs::path snapFPath = fs::path(outputPath) / genSnapFName(iItem);
            if (fs::exists(snapFPath)) continue;
            
            vidFiles.push_back(iItem);
        }
    }
    
    //For each Video file found
    cv::VideoCapture vidSrc;
    for(auto vidFile : vidFiles) {
        vidSrc.open(vidFile.string(),cv::CAP_FFMPEG);
        
        //Failed to open the vidFile, handler
        if(!vidSrc.isOpened()) {
            cout<<"Failed to open video file : "<<vidFile<<endl;
            continue;
        }
        
        //Generate Snap from the video
        cv::Mat snap;
        vidSrc>>snap;
        assert(snap.empty() == false);
        
        //Optimize the snap for storage
        cv::resize(snap, snap, cv::Size(0, 0),0.5,0.5);
        
        //Store Snap with appropriate filename in the output path
        fs::path snapFPath = fs::path(outputPath) / genSnapFName(vidFile);
        cv::imwrite(snapFPath.string(), snap);
        cout<<"Snap Generated: "<<snapFPath.filename()<<endl;
        
        vidSrc.release();
    }

    cout<<"Exiting.."<<endl;
    
    return 0;
}


string genSnapFName(fs::path vidFilePath) {
    const string snapExt = ".jpg";
    string snapFName =  vidFilePath.parent_path().leaf().string() + "_" + vidFilePath.stem().string() + snapExt;
    return snapFName;
}


void initFromConfigFile() {
    Config cfg;
    //initialize config file if not exist
    if(!fs::exists(configFPath)) {
        Setting &root = cfg.getRoot();
        root.add("VidStoragePath", Setting::TypeString);
        root.add("SnapOutputPath", Setting::TypeString);
        cfg.writeFile(configFPath.c_str());
        
        cout<<"Configuration generated at Path: "<<configFPath<<endl;
        cout<<"Update the config file and restart the application"<<endl;
        std::exit(EXIT_SUCCESS);

    }else {
        
        //Read Config file and update the params
        cfg.readFile(configFPath.c_str());
        Setting &root = cfg.getRoot();
        string val;
        root.lookupValue("VidStoragePath", storagePath);
        root.lookupValue("SnapOutputPath", outputPath);
        
        //Validate config values
        bool validationFailed = false;
        if(!fs::exists(storagePath)) {
            cout<<"Invalid VidStoragePath Path"<<endl;
            validationFailed = true;
        }
        
        if(!fs::exists(outputPath)) {
            cout<<"Invalid SnapOutputPath Path"<<endl;
            validationFailed = true;
        }
        
        if(validationFailed) std::exit(EXIT_FAILURE);
        else {
            cout<<"Config loaded"<<endl;
            cout<<"VidStoragePath: "<<storagePath<<endl;
            cout<<"SnapOutputPath: "<<outputPath<<endl;
        }
    }
    

}


void processCmdLineArgs(int argc, const char * argv[]) {
    const string keys=
    "{help h  ? |         | show help}"
    "{version V |         | show build version}"
    "{configFile c|       | Config File Path}"
    "{openCvLibVer |      | show opencv Lib Version}"
    ;
    
    cv::CommandLineParser parser(argc, argv, keys);
    parser.about("SnapGenerator build v" + string(APP_VER));
    
    if (parser.has("help")){
        parser.printMessage();
        exit(EXIT_SUCCESS);
    }
    
    if(parser.has("version")) {
        cout<<"SnapGenerator build v"<<APP_VER<<endl;
        exit(EXIT_SUCCESS);
    }
    
    if(parser.has("openCvLibVer")) {
        cout<<"OpenCV Ver: "<<CV_VERSION<<endl;
        exit(EXIT_SUCCESS);
    }
    
    if(parser.has("configFile")) {
        configFPath = parser.get<string>("configFile");
        cout<<"Using config file at path: "<<configFPath<<endl;
    }
}

