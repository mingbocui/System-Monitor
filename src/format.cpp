#include <string>

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) { 
  string hh, mm, ss;
  long h = seconds / 3600;
  long s = seconds % 3600;
  long m = s / 60;
  
  if (h < 10) {hh = '0' + std::to_string(h);}
  if (m < 10) {mm = '0' + std::to_string(m);}
  if (s < 10) {ss = '0' + std::to_string(s);}
  
  return hh + ':' + mm + ':' + ss;
}