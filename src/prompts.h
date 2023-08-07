
#ifndef __PROMPTS_CPP_H__
#define __PROMPTS_CPP_H__

#include <string>
#include <iostream>
#include <functional>

namespace cpp_prompt {

namespace colors {
const std::string red = "\033[31m";
const std::string blue = "\033[34m";
const std::string green = "\033[32m";
const std::string yellow = "\033[33m";
const std::string white = "\033[37m";
const std::string grey = "\033[30;1m";
const std::string reset = "\033[0m";

const std::string bold = "\033[1m";
const std::string underline = "\033[4m";

// bold colors
const std::string bred = bold + red;
const std::string bgreen = bold + green;
const std::string bblue = bold + blue;
const std::string byellow = bold + yellow;
const std::string bwhite = bold + white;
const std::string bgrey = bold + grey;

} // namespace colors

namespace symbols {
#ifdef __WIN32__
  const std::string arrowUp = "↑";
  const std::string arrowDown = "↓";
  const std::string arrowLeft = "←";
  const std::string arrowRight = "→";
  const std::string radioOn = "(*)";
  const std::string radioOff = "( )";	
  const std::string tick = "√";	
  const std::string cross = "×";	
  const std::string ellipsis = "...";	
  const std::string pointerSmall = "»";	
  const std::string line = "─";	
  const std::string pointer = ">";	            
#else
  const std::string arrowUp = "↑";
  const std::string arrowDown = "↓";
  const std::string arrowLeft = "←";
  const std::string arrowRight = "→";
  const std::string radioOn = "◉";
  const std::string radioOff = "◯";
  const std::string tick = "✔";	
  const std::string cross = "×";	
  const std::string ellipsis = "…";	
  const std::string pointerSmall = "›";	
  const std::string line = "─";	
  const std::string pointer = "❯";
#endif   
}

namespace _internal {
void printPrompt(const std::string& message, bool complete = false, bool coloredMessage = false) {
    if (complete) std::cout << colors::bgreen << " " << symbols::tick << "  " << colors::reset;
    else std::cout << colors::bgrey << " ? " << colors::reset;
    std::cout << message;
    if (complete) std::cout << colors::grey << symbols::ellipsis << " " << colors::reset;
    else std::cout << colors::grey << symbols::pointerSmall << " " << colors::reset;
    if (coloredMessage) std::cout << colors::blue << colors::underline;
}

void clearLine(int lines = 0) {
    for (int i = 0; i < (lines+1); i++) {
        std::cout << "\033[A" << "\33[2K\r";
    } 
}
static inline std::string ltrim(std::string &s) {
    auto copy = s;
    copy.erase(copy.begin(), std::find_if(copy.begin(), copy.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
    return copy;
}

static inline void moveCursorUp(int lines = 1) {
    for (int i = 0; i < lines; i++) 
        std::cout << "\x1b[A";
}

static inline void saveCursorPos() {
    std::cout << "\x1b[s";
}

static inline void restoreCursorPos() {
    std::cout << "\x1b[u";
}

} // namespace _internal

enum PromptType { 
    Text,
    Number,
    Password,
    Confirm,
    List,
    MultiList,
    Select,
    MultiSelect,
};

template <PromptType type = PromptType::Text>
typename std::enable_if<type == PromptType::Text, std::string>::type
prompt(const std::string& message, const std::string& default_value = "", std::function<std::string(const std::string&)> validator = [](const std::string&){ return ""; }) {
    _internal::printPrompt(message);
    std::string input;
    std::getline(std::cin, input);

    if (_internal::ltrim(input).empty()) input = default_value;
    bool hadError = false;
    std::string error = validator(input);
    while (!error.empty()) {
        _internal::clearLine();
        _internal::printPrompt(message, false);
        _internal::saveCursorPos();
        std::cout << "\n" << colors::bred << "  " << symbols::cross << " " << error << colors::reset << "\n";
        _internal::restoreCursorPos();
        if (!hadError) _internal::moveCursorUp();
        hadError = true;
        std::getline(std::cin, input);
        if (_internal::ltrim(input).empty()) input = default_value;
        error = validator(input);
    }

    _internal::clearLine();
    _internal::printPrompt(message, true);
    std::cout << input << std::endl;
    return input;
}

template <PromptType type = PromptType::Text>
typename std::enable_if<type == PromptType::Text, std::string>::type
prompt(const std::string& message, std::function<std::string(const std::string&)> validator) {
    return prompt<type>(message, "", validator);
}

}; // namespace cpp_prompt

#endif // __PROMPTS_CPP_H__
