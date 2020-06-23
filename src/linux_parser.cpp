#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>

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
  string line, version;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  float allMemory{0.0};
  float freeMemory{0.0};
  float buffer{0.0};
  string line;
  string key;
  float value{0.0};
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "MemTotal:") {
          allMemory = value;
          continue;
        } else if (key == "MemFree:") {
          freeMemory = value;
          continue;
        } else if (key == "Buffers:") {
          buffer = value;
          continue;
        }
      }
    }
  }
  return 1.0 - (freeMemory/(allMemory - buffer)); 
  }

// TODO: Read and return the system uptime
long LinuxParser::UpTime() { 
  long uptime = 0;

  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);

  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
    }
    return uptime;
  }

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 
  long totalJiffiers = 0;
  vector<string> utilStr = LinuxParser::CpuUtilization();
    for (unsigned int i = 0; i < utilStr.size(); i++) {
    if(i != LinuxParser::CPUStates::kGuest_ && i != LinuxParser::CPUStates::kGuestNice_) {
      totalJiffiers += stol(utilStr[i]);
    }
  }
  
  return totalJiffiers; 
  }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) { 
  long totalJiffiers = 0;
  string token;
  string line;
  int index = 1;
  
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
     
      while (linestream >> token) {
        if (index == 14 || index == 15 || index == 16 || index == 17) {
          totalJiffiers += stol(token);
        }
        index++;
      }
    }
  }
  return totalJiffiers;
  }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  long totalJiffies = LinuxParser::Jiffies();
  long idleJiffies = LinuxParser::IdleJiffies();
  long activeJiffies = totalJiffies - idleJiffies;
  
  return activeJiffies;  
  }

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  vector<string> utilStr = LinuxParser::CpuUtilization();
  long idleJiffies = stol(utilStr[LinuxParser::CPUStates::kIdle_]) + stol(utilStr[LinuxParser::CPUStates::kIOwait_]);
  
  return idleJiffies;
  }

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
  vector<string> cpuStrs;
    string value, line;

    std::ifstream filestream(kProcDirectory + kStatFilename);

    if (filestream.is_open()) {
        std::getline(filestream, line);
        std::istringstream linestream(line);

        while (linestream >> value) {
            if (value != "cpu") {
                cpuStrs.push_back(value);
            }
        }
    }

    return cpuStrs;
 }

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  int total = 0;
  string key;
  string line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  
  if (stream.is_open()) {
    // Call getline multiple times to search until label = processes
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> total;
      if (key != "processes") {
        return total;
      }
    }
  }
  return total;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  int prcsRunning = 0; 
  string key;
  string line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> prcsRunning;
      if (key == "procs_running") {
         return prcsRunning;
      }
      
    }
  }
  return prcsRunning;
 }

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) { 
  string line;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
  }

  return line;
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) { 
  string ram;
  string key;
  string line;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key;
      if (key == "VmSize:") {
        linestream >> ram;
      }
    }
  }
  
  return to_string(stol(ram)/1024) + " MB";
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  string id;
  string key;
  string line;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key;
      if (key == "Uid:") {
        linestream >> id;
      }
    }
  }
  return id;
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) { 
  string user = "u";
  string pidId = LinuxParser::Uid(pid);
  string line;
  string firstVal, x, secVal;

  std::ifstream filestream(kPasswordPath);

  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> firstVal >> x >> secVal) {
        if (secVal == pidId) {
          user = firstVal;
          break;
        }
      }
    }
  }

  return user;
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) { 
  long startTime;
  string token;
  string line;
  
  
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      int index=1;
      
      while (linestream >> token) {
        if(index == 22) {
          startTime = stol(token);
          break;
        }
        index++;
      }
       
    }
  }
  
  return startTime / sysconf(_SC_CLK_TCK);
}