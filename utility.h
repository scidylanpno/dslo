/*
 *
 * For paper 'Finding Subgraphs with Maximum Total Density and Limited Overlap in Weighted Hypergraphs'
 *
 * Copyright reserved.
 *
 */

#ifndef __UTILITY__
#define __UTILITY__

#include "macro.h"

#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <chrono>

#ifndef LOGFILE
#define LOGFILE "dslo.log"
#endif

#define LOG(context) Console::Log(string(__func__) + "(){\n" + string(context) + "}\n")
#define FEATURE(key, value) (string(key) + ": " + string(value) + ";\n")

#define tik auto time_start = std::chrono::high_resolution_clock::now();
#define tok auto time_end = std::chrono::high_resolution_clock::now();
#define elapsed (string("Elapsed time: ") + Double2String(std::chrono::duration<double>(time_end - time_start).count()) + " sec.")

/**********File**********/

class File{
protected:
string path_;
std::fstream file_;
void OpenFile(const std::ios_base::openmode);
void CloseFile();
File(const string, const std::ios_base::openmode);
~File();

};

/**********Input**********/

class Input: public File{

public:
Input(const Input&);
Input(const char*);
bool NextLine(std::stringstream&);

};

/**********Output**********/

class Output: public File{
public:
Output(const Output&);
Output(const char*);
Output(const int, const char*);
void PrintLine(const string);

};

/**********Console**********/

class Console{

private:
static Output log_file_;
static string Time();
static void Abort();

public:
static void Error(const string);
static void Log(const string);
static void Show(const string);

};

/**********...**********/

string Int2String(const int);

int String2Int(const string);

string Double2String(const double);

double String2Double(const string);

string RetrieveFileName(const string);

// #include "utility.cc"

#endif //__UTILITY__
