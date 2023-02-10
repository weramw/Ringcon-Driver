// RingConDriver.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <chrono>

#include <Windows.h>

#include "Driver.h"


bool running = TRUE;

BOOL WINAPI consoleHandler(DWORD signal) {

    if (signal == CTRL_C_EVENT) {
        std::cout << "Ctrl-C handled" << std::endl; // do cleanup
        running = FALSE;
    }
    return TRUE;
}

int main()
{
    if (!SetConsoleCtrlHandler(consoleHandler, TRUE)) {
        std::cout << "\nERROR: Could not set control handler\n";
        return 1;
    }
    
    double connect_rate = 1; // in Hz
    DWORD connectSleepDuration = DWORD(1 / connect_rate * 1000);


    double rate = 60; // in Hz
    //double rate = 5; // in Hz
    DWORD sleepDuration = DWORD(1 / rate * 1000);

    Driver driver;

    while (running && !driver.is_connected()) {
        driver.connect();
        Sleep(connectSleepDuration);
    }

    if (driver.is_connected())
    {
        std::cout << "Connected to left and right JoyCon!" << std::endl;
    }

    while (running) { 
        std::chrono::steady_clock::time_point time_start = std::chrono::high_resolution_clock::now();
        driver.update();
        std::chrono::steady_clock::time_point time_stop = std::chrono::high_resolution_clock::now();

        std::chrono::microseconds update_duration = std::chrono::duration_cast<std::chrono::microseconds>(time_stop - time_start);
        double update_duration_ms = update_duration.count() / 1000.0;

        DWORD sleep_ms = DWORD(sleepDuration > update_duration_ms ? sleepDuration - update_duration_ms : 0);

        Sleep(sleep_ms);

    }

    driver.disconnect();

    return 0;

    
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
