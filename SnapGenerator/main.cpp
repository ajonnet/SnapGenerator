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

using namespace std;
namespace fs = boost::filesystem;

//Config Params
const string storagePath = "/Users/Amit/Desktop/SnapGenerator/Storage";
const string outputPath = "/Users/Amit/Desktop/SnapGenerator/Output";
const string camStorePath = "xiaomi_camera_videos/04cf8c70d5de";

//Generates snap File name, by fetching date time info from its path
string genSnapFName(fs::path vidFilePath);


int main(int argc, const char * argv[]) {

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
        
        //Store Snap with appropriate filename in the output path
        fs::path snapFPath = fs::path(outputPath) / genSnapFName(vidFile);
        cv::imwrite(snapFPath.string(), snap);
        cout<<"Snap Generated: "<<snapFPath.filename()<<endl;
    }


    
    return 0;
}


string genSnapFName(fs::path vidFilePath) {
    const string snapExt = ".jpg";
    string snapFName =  vidFilePath.parent_path().leaf().string() + "_" + vidFilePath.stem().string() + snapExt;
    return snapFName;
}

