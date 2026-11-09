
1. to run the console version:
    on Windows:
        g++ -std=c++17 (Get-ChildItem *.cpp -Exclude gui.cpp).Name -o vole
        .\vole.exe

    on Linux:
        g++ -std=c++17 $(ls *.cpp | grep -v '^gui.cpp$') -o vole
        ./vole



2. to run the gui
    using a terminal:
        cmake -S . -B build -G Ninja
        cmake --build build
        build/vole_gui
    using qtmaker:
        open qtmaker and choose the CMakeLists.txt file of the project


             

3. How to use the gui:

    Controls
    LOAD           choose a program file, then its start address (hex, even)
    EDIT           overwrite a register or a memory cell
    STEP           run one instruction (the Enter key does the same)
    RUN ALL        run until the program halts
    SCREEN FORMAT  ASCII / HEX / DECIMAL for the LCD
    Exit           exit the program
    HELP           help on how to use the program

    Display
    header    PC, IR, HALT flag and the LCD screen (it scrolls when the output is long)
    console   messages and errors (scrolls)
    registers R0-RF, memory 16 x 16 matrix
    highlights: amber = cells at the PC (next instruction), inverted green = changed by the
                last action, dim = zero.



