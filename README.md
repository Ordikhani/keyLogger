# keyLogger

Description
This project is a simple Windows keylogger implemented in C++ using the WinAPI.
It captures keyboard input via a low-level keyboard hook (WH_KEYBOARD_LL) and logs both special keys and printable characters into a text file (key_file.txt).

How to Build & Run
Using MinGW (Windows)
1- Install MinGW and ensure g++ is in your PATH.

2- Compile:

bash
Copy
Edit
g++ main.cpp -o keylogger -static

3- Run as Administrator:

bash
Copy
Edit
keylogger.exe
4- To stop the program, press Ctrl + C in the console.

Using Visual Studio
Create a new C++ Console Project.

Replace the auto-generated code with main.cpp.

Build and run the project as Administrator.

