/*
 *
 * For paper 'Finding Subgraphs with Maximum Total Density and Limited Overlap in Weighted Hypergraphs'
 *
 * Copyright reserved.
 *
 */

#include "utility.h"

#include <time.h>
#include <stdlib.h>

#include <iostream>
#include <algorithm>


Output Console::log_file_(LOGFILE);

/**********Console**********/

string Console::Time(){
    time_t raw_time;
    struct tm* time_info;
    time(&raw_time);
    time_info = localtime(&raw_time);
    return asctime(time_info);
}

void Console::Abort(){
    Show("Abort at " + Time());
    LOG(FEATURE("Time", Time()));
    exit(1);
}

void Console::Error(const string err){
    Show("[Error]");
    Show(err);
    Show("");
    LOG(FEATURE("Message", err));
    Abort();
}

void Console::Log(const string context){
    log_file_.PrintLine(context);
}

void Console::Show(const string context){
    std::cerr << context << std::endl;
}

/**********File**********/

void File::OpenFile(const std::ios_base::openmode mode){
    try{
        if(file_.is_open()){
            CloseFile();
        }
        file_.open(path_, mode);
        if(!file_.is_open()){
            throw 0;
        }
        LOG(FEATURE("Path", path_));
    }
    catch(...){
        Console::Error("Can't open <" + path_ + ">!");
    }
}

void File::CloseFile(){
    LOG(FEATURE("Path", path_));
    if(file_.is_open()){
        file_.close();
    }
}

File::File(const string path, const std::ios_base::openmode mode):path_(path){
    OpenFile(mode);
}

File::~File(){
    CloseFile();
}

/**********Input**********/

Input::Input(const Input& other): File(other.path_, std::ios::in){}

Input::Input(const char* path): File(path, std::ios::in){}

bool Input::NextLine(std::stringstream& str){
    string buff;
    str.clear();
    str.str("");
    try{
        if(!getline(file_, buff)){
            return false;
        }
    }catch(...){
        Console::Error("Reading file error.");
    }
    str.str(buff);
    return true;
}

/**********Output**********/

Output::Output(const Output& other): File(other.path_, std::ios::out){}

Output::Output(const char* path): File(path, std::ios::out){}

Output::Output(const int n, const char* path): File(path, std::ios::out | std::ios::app){}

void Output::PrintLine(const string context){
    try{
        if(!file_.is_open()){
            throw 0;
        }
        file_ << context << std::endl;
        file_.flush();
    }catch(...){
        Console::Error("Output error.");
    }
}

/**********...**********/

string Int2String(const int x){
    return std::to_string(x);
}

int String2Int(const string str){
    return atoi(str.c_str());
}

string Double2String(const double x){
    return std::to_string(x);
}

double String2Double(const string str){
    return atof(str.c_str());
}

string RetrieveFileName(const string str){
    int pos = 0;
    for(int i = 0; i < str.length(); ++i){
        if(str[i] == '/'){
            pos = i + 1;
        }
    }
    string ans = "";
    for(; pos < str.length(); ++pos){
        if(str[pos] == '.'){
            ans += '_';
        }else{
            ans += str[pos];
        }
    }
    return ans;
}


