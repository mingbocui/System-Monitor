#include <dirent.h> // Directory Entries
#include <unistd.h>
#include <string>
#include <vector>

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
  float mem_total, mem_free;
  string key, val, line;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()){
    while( std::getline(filestream, line) ){
      std::istringstream linestream(line);
      line >> key >> val;
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
  long sum = 0;
  for(int i = kUser_; i <= kSteal_; i++){
    sum += stol(cpu_utils[i]);
  }
  return sum;

}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
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
  string val;
  vector<string> vec;
  int utime, stime, cutime, cstime, starttime;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kUptimeFilename);
    if(filestream.is_open()){
      std::getline(filestream, line);
      std::istringstream linestream(line);
      while(linestream >> val){
        vec.push_back(val);
      }
    }
  utime = stoi(vec[13]);
  stime = stoi(vec[14]);
  cutime = stoi(vec[15]);
  cstime = stoi(vec[16]);
  starttime = stoi(vec[21]);
}

long LinuxParser::ActiveJiffies() { 
  return LinuxParser::Jiffies() - LinuxParser::IdleJiffies();
}

long LinuxParser::IdleJiffies() { 
  vector<string> cpu_utils = LinuxParser::CpuUtilization();
  long sum = 0;
  for(int i = kIdle_; i <= kIOwait_; i++){
    sum += stol(cpu_utils[i]);
  }
  return sum;
}

// TODO: Read and return CPU utilization
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
  return cpu_util;
}

int LinuxParser::TotalProcesses() {
  string key, val, line;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()){
    while(std::getline(filestream, line){
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
    while(std::getline(filestream, line){
      std::istringstream linestream(line);
      linestream >> key >> val;
      if (key == "procs_running" ) return stoi(val);
    }
  }
  return 0;
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid[[maybe_unused]]) { return 0; }