#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

// TODO: Return this process's ID
int Process::Pid() { return this->pid_;}

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() { 
    this->cpuUtilization_ = (float)LinuxParser::ActiveJiffies(this->pid_) / (float)LinuxParser::Jiffies();
  return this->cpuUtilization_;
}

// TODO: Return the command that generated this process
string Process::Command() { 
    return LinuxParser::Command(this->pid_) ;
}

// TODO: Return this process's memory utilization
string Process::Ram() { 
    return LinuxParser::Ram(this->pid_);
}

// TODO: Return the user (name) that generated this process
string Process::User() { 
    return LinuxParser::User(this->pid_);
}

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { 
    return LinuxParser::UpTime() - LinuxParser::UpTime(this->pid_);
}

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const { 
    return a.cpuUtilization_ < this->cpuUtilization_;
}