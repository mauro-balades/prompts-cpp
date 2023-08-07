
#ifndef __PROMPTS_CPP_H__
#define __PROMPTS_CPP_H__

#include <string>

namespace cpp_prompt {

namespace colors {
const std::string red = "\033[31m";
const std::string blue = "\033[34m";
const std::string green = "\033[32m";
const std::string yellow = "\033[33m";
const std::string white = "\033[37m";
const std::string grey = "\033[30;1m";
const std::string reset = "\033[0m";
} // namespace colors

}; // namespace cpp_prompt

#endif // __PROMPTS_CPP_H__
