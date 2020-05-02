#include "processor.h"
#include "linux_parser.h"
// TODO: Return the aggregate CPU utilization
float Processor::Utilization() { 
  
  std::vector<string> cpuUtil = LinuxParser::CpuUtilization();
  // formula copied from website
  float user = stof(cpuUtil[0]);
  float nice = stof(cpuUtil[1]);
  float system = stof(cpuUtil[2]);
  float idle = stof(cpuUtil[3]);
  float iowait = stof(cpuUtil[4]);
  float irq = stof(cpuUtil[5]);
  float softirq = stof(cpuUtil[6]);
  float steal = stof(cpuUtil[7]);

  float PrevIdle = kIdle_ + kIOwait_;
  float Idle = idle + iowait;

  float PrevNonIdle = kUser_ + kNice_+ kSystem_ + kIRQ_ + kSoftIRQ_ + kSteal_;
  float NonIdle = user + nice + system + irq + softirq + steal;

  float PrevTotal = PrevIdle + PrevNonIdle;
  float Total = Idle + NonIdle;

  float totald = Total - PrevTotal;

  float idled = Idle - PrevIdle;

  float CPU_Percentage = (totald - idled) / totald;

  return CPU_Percentage;
}