#include <dirent.h> // Directory Entries
#include <unistd.h>
#include <string>
#include <vector>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <string>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  // The DIR data type represents a directory stream
  DIR* directory = opendir(kProcDirectory.c_str()); // c_str() converts a C++ string into a C-style
  // Directory Entries
  struct dirent* file;
  // readdir
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename); // stoi Converting Strings to Numbers
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

float LinuxParser::MemoryUtilization() { 
  float mem_total{0}, mem_free{0};
  string key, val, line;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()){
    while( std::getline(filestream, line) ){
      std::istringstream linestream(line);
      linestream >> key >> val;
      if (key == "MemTotal:") mem_total = stof(val);
      if (key == "MemFree:") mem_total = stof(val);
    }
    return (mem_total - mem_free) / mem_total;
  }
  return 0.0;
}

long LinuxParser::UpTime() { 
  string val, line;
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if(filestream.is_open()){
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> val;
    return stol(val);
  }
  return 0;
}

long LinuxParser::Jiffies() { 
  
  vector<string> cpu_utils = LinuxParser::CpuUtilization();
  long total = 0;
  for(int i = kUser_; i <= kSteal_; i++){
    total += stol(cpu_utils[i]);
  }
  return total;

}

// Read and return the number of active jiffies for a PID
// https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat/16736599#16736599
// #14 utime - CPU time spent in user code, measured in clock ticks
// #15 stime - CPU time spent in kernel code, measured in clock ticks
// #16 cutime - Waited-for children's CPU time spent in user code (in clock ticks)
// #17 cstime - Waited-for children's CPU time spent in kernel code (in clock ticks)
// #22 starttime - Time when the process started, measured in clock ticks
// First we determine the total time spent for the process:
// total_time = utime + stime
// We also have to decide whether we want to include the time from children processes. If we do, then we add those values to total_time:

// total_time = total_time + cutime + cstime
// Next we get the total elapsed time in seconds since the process started:

// seconds = uptime - (starttime / Hertz)
// Finally we calculate the CPU usage percentage:

// cpu_usage = 100 * ((total_time / Hertz) / seconds)
long LinuxParser::ActiveJiffies(int pid) { 
  string val, line;
  vector<string> vec;
  long utime, stime, cutime, cstime, starttime;
  // long total = 0;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kUptimeFilename);
    if(filestream.is_open()){
      std::getline(filestream, line);
      std::istringstream linestream(line);
      while(linestream >> val){
        vec.push_back(val);
      }
    }
  utime = stol(vec[13]);
  stime = stol(vec[14]);
  cutime = stol(vec[15]);
  cstime = stol(vec[16]);
  starttime = stol(vec[21]);
  
  return utime+stime+cutime+cstime+starttime;
}

long LinuxParser::ActiveJiffies() { 
  return LinuxParser::Jiffies() - LinuxParser::IdleJiffies();
}

long LinuxParser::IdleJiffies() { 
  vector<string> cpu_utils = LinuxParser::CpuUtilization();
  long total = 0;
  for(int i = kIdle_; i <= kIOwait_; i++){
    total += stol(cpu_utils[i]);
  }
  return total;
}

// Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
  
  string val, line;
  vector<string> cpu_utils{};
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()){
    std::getline(filestream, line);
    std::istringstream linestream(line);
    while( linestream >> val ){
      if (val != "cpu" ) cpu_utils.push_back(val);
    }
  }
  return cpu_utils;
}

int LinuxParser::TotalProcesses() {
  string key, val, line;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()){
    while(std::getline(filestream, line)){
      std::istringstream linestream(line);
      linestream >> key >> val;
      if (key == "processes" ) return stoi(val);
    }
  }
  return 0;
}

int LinuxParser::RunningProcesses() { 
  string key, val, line;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()){
    while(std::getline(filestream, line)){
      std::istringstream linestream(line);
      linestream >> key >> val;
      if (key == "procs_running" ) return stoi(val);
    }
  }
  return 0;
}

string LinuxParser::Command(int pid) {
  string line;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if(filestream.is_open()){
    std::getline(filestream, line);
    // no need to use istringstream
    // std::istringstream linestream(line);
  }
  return line;

}

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) { 
  string key, val, line;
  float ram;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if(filestream.is_open()){
    while (std::getline(filestream, line)){
      std::istringstream linestream(line);
      linestream >> key >> val;
      if (key == "VmSize"){
        ram = stof(val) / 1024;
        break;
      }
    }
  }
  return std::to_string(ram); 
}

// Read and return the user ID associated with a process
// string LinuxParser::Uid(int pid){ 
string LinuxParser::Uid(int pid) { 
  string key, val, line;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if(filestream.is_open()){
    while (std::getline(filestream, line)){
      std::istringstream linestream(line);
      linestream >> key >> val;
      if (key == "Uid:") return val;
    }  
}
return string(); 
}

// Read and return the user associated with a process
// /etc/passwd
string LinuxParser::User(int pid) { 
  string key, val, x, line;
  std::ifstream filestream(kPasswordPath);
  string uid = LinuxParser::Uid(pid);
  if(filestream.is_open()){
    while (std::getline(filestream, line)){
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> key >> x >> val;  
      if (val == uid) return key;
    }
  }
  return string(); 
}

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) { 
  string val, line;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if(filestream.is_open()){
    std::getline(filestream, line);
    std::istringstream linestream(line);
    int i = 0;
    while(linestream >> val ){
      if (i == 21) { return stol(val) / sysconf(_SC_CLK_TCK); }
      i++;
    }
  }
  return 0;
 
}