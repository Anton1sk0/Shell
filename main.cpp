#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Shell.h"

int main(int argc, char** argv) {
    Shell shell;

    while (1) {
        shell.prompt();

        int c = shell.readTerminalCommand();

        
        char * terminalCommand = shell.getCommand();

        if (c == -1 || strcmp(terminalCommand, "exit") == 0) {
            break;
        }

        if (c == 0 || strlen(terminalCommand) == 0) {
            continue;
        }
        
        string cmd = shell.spaces(terminalCommand);
//        string cmd = terminalCommand;
        
        shell.executeTerminalCommand(cmd);
        
    }

    return 0;
}

