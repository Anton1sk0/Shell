
#ifndef SHELL_H
#define SHELL_H

#include <string>
#include <cstdlib>
#include <unistd.h>

#include "HistoryManager.h"
#include "AliasManager.h"
#include "List.h"
#include "WildcardEvaluator.h"
#include "JobsManager.h"

using namespace std;

class Shell {
public:
    Shell();
    virtual ~Shell();
    
    void executeTerminalCommand(string command);
    void executeCommand(string command);
    int readTerminalCommand();
    void prompt();
    char * getCommand();
    string spaces(string command);

    
    List<pid_t> & getChildrenPids();
    List<pid_t> & getJobChildrenPids();
private:
    char *buffer;
    size_t bufsize;
    
    HistoryManager historyManager;
    AliasManager aliasManager;
    JobsManager jobManager;
    WildcardEvaluator wildeva;
    List<string> * splitCommands(string terminalCommand);
    List<string> * splitArguments(string command);
    pid_t operatingSystemExecuteSimple(string command, int fd_in = -1, int fd_out = -1, bool shouldWait = true, int ** fd_array = NULL, int n = 0);
    void operatingSystemExecuteAdvanced(string command);
    List<pid_t> childrenPids;
};

#endif /* SHELL_H */

