1. to run the console version:
    on Windows:
        (one-time: install MSYS2 and add C:\msys64\ucrt64\bin to PATH, see step 2)
        $src = Get-ChildItem *.cpp -Exclude gui.cpp | ForEach-Object Name
        g++ -std=c++17 $src -o vole
        .\vole.exe

    on Linux:
        g++ -std=c++17 $(ls *.cpp | grep -v '^gui.cpp$') -o vole
        ./vole



2. to run the gui
    using a terminal:

        one-time setup, in the "MSYS2 UCRT64" terminal (not PowerShell):
            pacman -Syu
            (if asked to close and reopen the terminal, do so, then run pacman -Syu again)
            pacman -S --needed mingw-w64-ucrt-x86_64-gcc \
                              mingw-w64-ucrt-x86_64-cmake \
                              mingw-w64-ucrt-x86_64-ninja \
                              mingw-w64-ucrt-x86_64-qt6-base \
                              mingw-w64-ucrt-x86_64-qt6-tools

        one-time setup, in PowerShell:
            [Environment]::SetEnvironmentVariable("Path", $env:Path + ";C:\msys64\ucrt64\bin", "User")
            (close and reopen PowerShell after this)

        build and run, in PowerShell:
            cmake -S . -B build -G Ninja -DCMAKE_PREFIX_PATH="C:/msys64/ucrt64"
            cmake --build build
            .\build\vole_gui.exe

        after changing source files, this is enough:
            cmake --build build

    using Qt Creator:
        open Qt Creator and choose the CMakeLists.txt file of the project



3. How to use the gui:

    Controls
    LOAD           choose a program file, then its start address (hex, even)
    EDIT           overwrite a register or a memory cell
    STEP           run one instruction (the Enter key does the same)
    RUN ALL        run until the program halts
    SCREEN FORMAT  ASCII / HEX / DECIMAL for the LCD
    HELP           help on how to use the program
    EXIT           exit the program

    Display
    header    PC, IR, HALT flag and the LCD screen (it scrolls when the output is long)
    console   messages and errors (scrolls)
    registers R0-RF, memory 16 x 16 matrix
    highlights: amber = cells at the PC (next instruction), inverted green = changed by the
                last action, dim = zero.



