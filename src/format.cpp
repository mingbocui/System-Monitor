#include <string>

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds[[maybe_unused]]) { 
  long h = seconds / 3600;
  long m = (seconds % 3600) / 60;
  long s = ((seconds % 3600) / 60) % 60;
  
  return std::to_string(h) + ':' + std::to_string(m) + ':' + std::to_string(s);
}