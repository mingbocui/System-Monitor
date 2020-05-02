#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include "linux_parser.h"

#include "process.h"

using std::string;
using std::to_string;
using std::vector;

// constructor
Process::Process(int pid) : pid_(pid) {}

// Return this process's ID
int Process::Pid() { return pid_; }

// TODO: Return this process's CPU utilization

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


float Process::CpuUtilization() { 
  long total_time = LinuxParser::ActiveJiffies(pid_);
  long start_time = LinuxParser::UpTime(pid_);
  long up_time = LinuxParser::UpTime(); // seconds?
  
  // / sysconf(_SC_CLK_TCK)
//   this->cpu_util_ = total_time / (up_time - start_time);
  
  return total_time / (up_time - start_time);

}

// Return the command that generated this process
string Process::Command() { return LinuxParser::Command(pid_); }

// Return this process's memory utilization
string Process::Ram() { return LinuxParser::Ram(pid_); }

// Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(pid_); }

// Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(pid_); }

// TODO Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const { 
  // this < a means the Ram used by this object is less than 
  return this->CpuUtilization() < a->CpuUtilization();
}