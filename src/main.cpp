/**
 * @file main.cpp
 * @brief Entry point for the Vestify stock analysis application.
 *
 * This file contains the main function that initializes and runs the interactive
 * menu system for the stock analysis application. The application provides
 * functionality for loading stock data, performing analysis, and displaying
 * results through a command-line interface.
 */

#include "ui/InteractiveMenu.hpp"

/**
 * @brief Main entry point for the Vestify application.
 *
 * Initializes the interactive menu system and starts the application loop.
 * The menu handles user input and coordinates between different components
 * of the stock analysis system.
 *
 * @param argc Number of command-line arguments.
 * @param argv Array of command-line argument strings.
 * @return Exit code (0 for success, non-zero for errors).
 */
int main(int argc, char **argv)
{
    InteractiveMenu menu;
    return menu.run(argc, argv);
}