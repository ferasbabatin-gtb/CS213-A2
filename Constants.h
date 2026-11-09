#ifndef CONSTANTS_H
#define CONSTANTS_H

const int MEMORY_SIZE    = 256;
const int REGISTER_COUNT = 16;
const int SCREEN_ADDRESS = 0xFF;
const int MAX_STEPS      = 10000;

enum class RunMode { RUN_ALL, STEP_BY_STEP };
enum class ExecStatus { OK, HALTED, SCREEN_OUTPUT,INVALID_INSTRUCTION };
enum class ScreenFormat {
    ASCII,
    HEX,
    DECIMAL
};

enum class LoadStatus {
    LOAD_OK,
    FILE_NOT_FOUND,
    ODD_ADDRESS,
    ADDRESS_OUT_OF_RANGE,
    PROGRAM_TOO_LARGE,
    INVALID_INSTRUCTION_IN_FILE
};

enum class StepResult {
    STEP_OK,
    STEP_HALTED,
    STEP_INVALID_INSTRUCTION,
    STEP_PC_OUT_OF_RANGE,
    STEP_NOT_LOADED,
    STEP_LIMIT_REACHED
};
#endif
