
#include "prompts.h"
using namespace cpp_prompt;

int main() {
    promptNumber("What's ur age? ", [](int age) {
        if (age < 18) {
            return "You must be 18 or older to use this program.";
        }
        return "true;";
    });
}
