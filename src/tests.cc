
#include "prompts.h"
using namespace cpp_prompt;

int main() {
    prompt("What is your name? ", [](std::string name) {
        if (name != "hello") {
            return "That's not your name, " + name + "!";
        }
    });
}
