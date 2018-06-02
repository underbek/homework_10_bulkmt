#ifndef helper_h
#define helper_h 

#include <string>
#include <ctime>
#include <memory>
#include <sstream>

std::string name(const std::weak_ptr<std::time_t>& time, const int& id = 1);

#endif