
#ifndef __PROMPTS_CPP_H__
#define __PROMPTS_CPP_H__

#include <string>
#include <iostream>
#include <functional>
#include <termios.h>
#include <unistd.h>

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
    else std::cout << colors::bgrey << "? " << colors::reset;
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

static inline void moveCursorDown(int lines = 1) {
    for (int i = 0; i < lines; i++) 
        std::cout << "\033[1B";
}

static inline void moveCursorLeft(int lines = 1) {
    for (int i = 0; i < lines; i++) 
        std::cout << "\x1b[D";
}

static inline void moveCursorRight(int lines = 1) {
    for (int i = 0; i < lines; i++) 
        std::cout << "\x1b[C";
}

static inline void saveCursorPos() {
    std::cout << "\x1b[s";
}

static inline void restoreCursorPos() {
    std::cout << "\x1b[u";
}

inline void hideCursor()
{
    std::cout << "\033[?25l";
}

inline void showCursor()
{
    std::cout << "\033[?25h";
}

struct termios orig_termios;
inline void rawModeOff()
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
    showCursor();
}
inline void rawModeOn()
{
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(rawModeOff);
    atexit(showCursor);
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
    hideCursor();
}

} // namespace _internal

std::string
prompt(const std::string& message, const std::string& default_value = "", std::function<std::string(const std::string&)> validator = [](const std::string&){ return ""; }) {
    _internal::printPrompt(message);
    std::string input;
    _internal::saveCursorPos();
    std::getline(std::cin, input);

    if (_internal::ltrim(input).empty()) input = default_value;
    bool hadError = false;
    std::string error = validator(input);
    while (!error.empty()) {
        _internal::moveCursorDown();
        _internal::clearLine();
        _internal::clearLine();
        _internal::printPrompt(message);
        std::cout << std::endl;
        std::cout << " " << colors::bred << symbols::cross << " " << error << std::endl;
        _internal::restoreCursorPos();
        hadError = true;
        std::getline(std::cin, input);
        if (_internal::ltrim(input).empty()) input = default_value;
        error = validator(input);
    }

    _internal::clearLine();
    _internal::printPrompt(message, true);
    std::cout << input << colors::reset << std::endl;
    return input;
}

std::string
prompt(const std::string& message, std::function<std::string(const std::string&)> validator) {
    return prompt(message, "", validator);
}

std::string
prompt(const std::string& message, const std::string mask = "*", bool required = true) {
    char c;
    bool hadError = false;

    // we type "*" as the user tpes characters to make the password hidden
    _internal::printPrompt(message);
    std::string input;
    _internal::saveCursorPos();

    // disable writing to the console
    _internal::rawModeOn();
    while ((c = std::getchar())) {
        if (c == 127) {
            if (!input.empty()) {
                input.pop_back();
                _internal::moveCursorLeft();
                std::cout << " ";
                _internal::moveCursorLeft();
            }
        } else if (c == '\n') {
            if (required && _internal::ltrim(input).empty()) {
                if (!hadError) {
                    hadError = true;
                    _internal::rawModeOff();
                    _internal::saveCursorPos();
                    std::cout << std::endl;
                    std::cout << " " << symbols::pointerSmall << colors::bred << " This field is required" << std::endl;
                    _internal::restoreCursorPos();
                    _internal::rawModeOn();
                }
                continue;
            }
            break;
        } else {
            input += c;
            std::cout << mask;
        }
    }

    // enable writing to the console
    _internal::rawModeOff();
    std::cout << std::endl;
    if (hadError) {
        _internal::moveCursorDown();
        _internal::clearLine(2);
    } else {
        _internal::clearLine();
    }
    _internal::printPrompt(message, true, true);
    for (int i = 0; i < input.length(); i++)
        std::cout << mask;
    std::cout << colors::reset << std::endl;
    return input;
}

std::string
prompt(const std::string& message, bool required) {
    return prompt(message, "*", required);
}

template <typename retTy = int>
retTy promptNumber(const std::string& message, int default_value = 0, std::function<std::string(int)> validator = [](int){ return ""; }) {
    _internal::printPrompt(message);
    std::string input;
    char c;
    bool hadError = false;
    _internal::rawModeOn();
    auto checkInput = [&]() {
        auto validatorError = input.empty() ? "" : validator(std::stoi(input));
        if (!validatorError.empty()) {
            if (!hadError) {
                hadError = true;
                _internal::rawModeOff();
                _internal::saveCursorPos();
                std::cout << std::endl;
                std::cout << " " << symbols::pointerSmall << colors::bred << " " << validatorError << std::endl;
                _internal::restoreCursorPos();
                _internal::moveCursorUp(2);
                _internal::rawModeOn();
            }
        } else {
            hadError = false;
            _internal::saveCursorPos();
            _internal::moveCursorDown();
            _internal::clearLine(2);
            _internal::restoreCursorPos();
        }
    };

    while ((c = std::getchar())) {
        if (c == 127) {
            if (!input.empty()) {
                input = input.erase(input.length() - 1);
                _internal::moveCursorLeft();
                std::cout << " ";
                _internal::moveCursorLeft();
                checkInput();
            }
        } else if (c == '\n') {
            if (!hadError)
                break;
        } else if (c >= '0' && c <= '9') {
            if (input.size() == 10) continue;
            input.append(1, c);
            std::cout << c;
            checkInput();
        }

        // up and down arrows
        else if (c == '\033') {
            std::getchar();
            if (input.empty()) input = "0";
            switch (std::getchar()) {
                case 'A': // up arrow
                    input = std::to_string(std::stoi(input) + 1);
                    std::cout << "\033[1A";
                    _internal::moveCursorDown(2);
                    _internal::clearLine();
                    _internal::printPrompt(message);
                    std::cout << input;
                    checkInput();
                    break;
                case 'B': // down arrow
                    if (std::stoi(input) != 0) {
                        input = std::to_string(std::stoi(input) - 1);
                        std::cout << "\033[1A";
                        _internal::moveCursorDown(2);
                        _internal::clearLine();
                        _internal::printPrompt(message);
                        std::cout << input;
                        checkInput();
                    }
                    break;
            }
        }
    }

    _internal::rawModeOff();
    std::cout << std::endl;
    if (hadError) {
        _internal::moveCursorDown();
        _internal::clearLine(2);
    } else {
        _internal::clearLine();
    }
    _internal::printPrompt(message, true);
    std::cout << input << colors::reset << std::endl;
    std::cout << colors::reset << std::endl;
    return std::stoi(input);
}

template <typename retTy = int>
retTy promptNumber(const std::string& message, std::function<std::string(int)> validator) {
    return promptNumber<retTy>(message, 0, validator);
}


}; // namespace cpp_prompt

#endif // __PROMPTS_CPP_H__
