#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>
#include <utility>

#include "linux_parser.h"

#include "process.h"

using std::string;
using std::to_string;
using std::vector;

// constructor
// Process::Process(int pid) : pid_{pid} {}
Process::Process(int p) : pid_(p) {}

// Return this process's ID
int Process::Pid() { return Process::pid_; }

float Process::CpuUtilization(){ 
  std::ifstream filestream(LinuxParser::kProcDirectory + std::to_string(pid_) + LinuxParser::kStatFilename);
  string line;
  std::getline(filestream, line); // file contains only one line
  std::istringstream buffer(line);
  std::istream_iterator<string> beginning(buffer), end;
  std::vector<string> line_content(beginning, end);
  float utime = LinuxParser::UpTime(pid_);
  float stime = stof(line_content[14]);
  float cutime = stof(line_content[15]);
  float cstime = stof(line_content[16]);
  float starttime = stof(line_content[21]);
  float uptime = LinuxParser::UpTime();
  float freq = sysconf(_SC_CLK_TCK);
  float total_time = utime + stime + cutime + cstime;
  float seconds = uptime - (starttime / freq);
  cpu_util_ = 100.0 * ((total_time / freq) / seconds);
  return cpu_util_;

}

// Return the command that generated this process
string Process::Command() { return LinuxParser::Command(pid_); }

// Return this process's memory utilization
string Process::Ram() { return LinuxParser::Ram(pid_); }

// Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(pid_); }

// Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(pid_); }

//  Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const{ 
  // this < a means the Ram used by this object is less than 
  return cpu_util_ < a.cpu_util_;//
  // return stol(LinuxParser::Ram(pid_)) > stol(LinuxParser::Ram(a.pid_));
}