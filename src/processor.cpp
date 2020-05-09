#include "processor.h"
#include "linux_parser.h"
#include <string>
#include <unistd.h>

using std::string;
using std::stoi;
// Return the aggregate CPU utilization
float Processor::Utilization() { 
  // long total_jiffies_1 = LinuxParser::Jiffies();
  // long active_jiffies_1 = LinuxParser::ActiveJiffies();
  // usleep(10000);
  // long total_jiffies_2 = LinuxParser::Jiffies();
  // long active_jiffies_2 = LinuxParser::ActiveJiffies();

  // long delta_total = total_jiffies_2 - total_jiffies_1;
  // long delta_active = active_jiffies_2 - active_jiffies_1;

  // return float (delta_active / delta_total);

  string cpu, user, nice, sys, idle, iowait, irq, soft_irq, steal, guest, guest_nice;
  string line;

  std::ifstream filestream(LinuxParser::kProcDirectory + LinuxParser::kStatFilename);

  if (filestream.is_open())
  {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> cpu >> user >> nice >> sys >> idle >> iowait >> irq >> soft_irq >> steal >> guest >> guest_nice;

    int idle_time = stoi(idle) + stoi(iowait);
    int no_idle_time = stoi(user) + stoi(nice) + stoi(sys) + stoi(irq) + stoi(soft_irq) + stoi(steal);

    float total_time = idle_time + no_idle_time;

    return (total_time - idle_time)/total_time;
  }
  return 0;

}

  // std::vector<string> cpuUtil = LinuxParser::CpuUtilization();
  // // formula copied from website
  // float user = stof(cpuUtil[0]);
  // float nice = stof(cpuUtil[1]);
  // float system = stof(cpuUtil[2]);
  // float idle = stof(cpuUtil[3]);
  // float iowait = stof(cpuUtil[4]);
  // float irq = stof(cpuUtil[5]);
  // float softirq = stof(cpuUtil[6]);
  // float steal = stof(cpuUtil[7]);

  // float PrevIdle = kIdle_ + kIOwait_;
  // float Idle = idle + iowait;

  // float PrevNonIdle = kUser_ + kNice_+ kSystem_ + kIRQ_ + kSoftIRQ_ + kSteal_;
  // float NonIdle = user + nice + system + irq + softirq + steal;

  // float PrevTotal = PrevIdle + PrevNonIdle;
  // float Total = Idle + NonIdle;

  // float totald = Total - PrevTotal;

  // float idled = Idle - PrevIdle;

  // float CPU_Percentage = (totald - idled) / totald;

  // return CPU_Percentage;