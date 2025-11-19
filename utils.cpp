#include "utils.h"

std::string logError(const std::string& msg) {
    return "\033[1;31m" + msg + "\033[0m";
}

