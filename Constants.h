#ifndef CONSTANTS_H
#define CONSTANTS_H

const int MEMORY_SIZE    = 256;
const int REGISTER_COUNT = 16;
const int SCREEN_ADDRESS = 0xFF;
const int MAX_STEPS      = 10000;
enum class ScreenFormat { ASCII, HEX, DECIMAL };
enum class RunMode { RUN_ALL, STEP_BY_STEP };
enum class ExecStatus { OK, HALTED, SCREEN_OUTPUT,INVALID_INSTRUCTION };

#endif