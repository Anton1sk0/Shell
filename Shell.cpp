#include <cstdlib>
#include <cstdio>
#include <string>
#include <sstream>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <signal.h>
#include "WildcardEvaluator.h"

#include "Shell.h"

static Shell * currentShell = nullptr;

void catchinterrupt(int signo) {
    List<pid_t> & pids = currentShell->getChildrenPids();

    int n = pids.getCount();

    if (n > 0) {
        if (signo == SIGINT) {
            int * keys = pids.keysToArray();

//            printf("Received SIGINT (CTRL-C) signal. Exit now. pid: %d \n", getpid());

            for (int i = 0; i < n; i++) {
                if (keys[i] != 0) {
                    printf("INTERRUPTING : %d \n", keys[i]);
                    
                    kill(keys[i], SIGINT);
                }
            }            

            delete [] keys;
            
            printf("End of signal handler (SIGINT), pid: %d  \n", getpid());
        } else if (signo == SIGTSTP) {
            int * keys = pids.keysToArray();

//            printf("Received SIGINT (CTRL-C) signal. Exit now. pid: %d \n", getpid());

            for (int i = 0; i < n; i++) {
                if (keys[i] != 0) {
                    printf("STOPPING : %d \n", keys[i]);
                    kill(keys[i], SIGSTOP);
                    
                    currentShell->getJobChildrenPids().add(keys[i]);
                }
            }

            delete [] keys;
            
            printf("End of signal handler (SIGTSTP), pid: %d  \n", getpid());
        }
    }
    
    
}

Shell::Shell() : buffer(nullptr), bufsize(32) {
    currentShell = this;

    static struct sigaction act;
    act.sa_handler = catchinterrupt;
    act.sa_flags = SA_RESTART;
    sigfillset(&(act.sa_mask));
    sigaction(SIGINT, &act, NULL);
    
    
    
    static struct sigaction act2;
    act2.sa_handler = catchinterrupt;
    sigfillset(&(act.sa_mask));
    sigaction(SIGTSTP, &act2, NULL);
}

Shell::~Shell() {
    if (buffer != nullptr) {
        free(buffer);
    }

    currentShell = nullptr;
}

void Shell::prompt() {
    printf("in-mysh-now:> ");
}

int Shell::readTerminalCommand() {
    int c = getline(&buffer, &bufsize, stdin);

    if (c > 0) {
        char * ptr = buffer;

        while (*ptr != '\n' && *ptr != '\0') {
            ptr++;
        }

        if (*ptr == '\n') {
            *ptr = '\0';
        }
    }

    string command = buffer;

    if (command != "myHistory") {
        historyManager.add(command);
    }

    return c;
}

char * Shell::getCommand() {
    return buffer;
}

void Shell::executeTerminalCommand(string command) {
    List<string> * commands = splitCommands(command);

    for (Node<string> * current = commands->getFirst(); current != nullptr; current = current->next) {
        string command = current->key;

        cout << "Executing composite command: " << command << endl;

        executeCommand(command);
    }

    delete commands;
}

void Shell::executeCommand(string command) {
    if (command == "myHistory") {
        historyManager.print();
    } else if (command == "jobs") {
        cout << "Current jobs: " << endl;
        jobManager.getChildrenPids().print();
    } else if (command == "fg") {
        pid_t target = jobManager.wakeFirstChild();
        
        if (target != 0) {
            waitpid(target, nullptr, 0);
        }
    } else if (command == "aliases") {
        aliasManager.print();
    } else if (command.substr(0, 9) == "myHistory") {
        string rest = command.substr(10);

        int index = atoi(rest.c_str());

        if (index >= 0 && index < 20) {
            command = historyManager.getCommand(index);

            if (command != "") {
                cout << "Executing command from history: " << command << endl;
                executeTerminalCommand(command);
            }
        }
    } else if (command.substr(0, 11) == "createalias") {
        // createalias gg "git status"

        string rest = command.substr(12);
        stringstream ss(rest);
        string alias;
        ss >> alias;

        stringstream ss2(rest);
        string token;

        std::getline(ss, token, '"');
        std::getline(ss, token, '"');

        aliasManager.add(alias, token);
    } else if (command.substr(0, 12) == "destroyalias") {
        // destroyalias gg
        string alias = command.substr(13);

        aliasManager.remove(alias);
    } else {
        string alias_command = aliasManager.getCommand(command);

        if (alias_command != "") {
            cout << "Executing command from aliases: " << alias_command << endl;
            executeCommand(alias_command);
        } else {
            bool redirection_or_pipe = false;

            for (size_t i = 0; i < command.length(); i++) {
                if (command[i] == '|' || command[i] == '>' || command[i] == '<') {
                    redirection_or_pipe = true;
                }
            }

            string simple = redirection_or_pipe ? "false" : "true";

            cout << "Forking command " << command << " via system calls. Simple: " << simple << endl;
            cout << "_________________________________________\n";


            if (!redirection_or_pipe) {
                operatingSystemExecuteSimple(command);
            } else {
                operatingSystemExecuteAdvanced(command);
            }
            cout << "_________________________________________\n";
        }
    }
}

List<string> * Shell::splitCommands(string terminalCommand) {
    List<string> * list = new List<string>();

    stringstream ss(terminalCommand);
    string token;

    while (std::getline(ss, token, ';')) {
        list->add(token);
    }

    return list;
}

List<string> * Shell::splitArguments(string command) {
    List<string> * list = new List<string>();

    stringstream ss(command);
    string token;

    while (std::getline(ss, token, ' ')) {
        bool mustevaluate = false;

        for (long unsigned int i = 0; i < token.length(); i++) { //check if token include * or ? and must be evaluated
            if (token[i] == '*' || token[i] == '?') {
                mustevaluate = true;
            }
        }

        if (mustevaluate == true) {
            list->append(wildeva.evaluate(token));
        } else {
            list->add(token);
        }
    }

    return list;
}

string Shell::spaces(string command) {
    string newCommand = "";
    int len = command.length();
    bool spaceAdded = false;

    for (int i = 0; i < len; i++) {
        char current = command[i];
        if ((current == '>' || current == '<' || current == '|') && command[i + 1] != '>' && command[i - 1] != '>') {
            if (!spaceAdded && i > 0 && command[i - 1] != ' ') {
                newCommand += ' ';
                spaceAdded = true;
            }
            newCommand += current;
            if (i < len - 1 && command[i + 1] != ' ') {
                newCommand += ' ';
                spaceAdded = true;
            }
        } else if (command[i] == '>' && command[i + 1] == '>' && command[i - 1] != ' ' && command[i + 2] != ' ') { // >> 
            newCommand += ' ';
            spaceAdded = true;
            newCommand += current;
            newCommand += command[i + 1];
            newCommand += ' ';
            i = i + 1;
        } else {
            newCommand += current;
            spaceAdded = false;
        }
    }

    // TODO:trim spaces

    return newCommand;
}

pid_t Shell::operatingSystemExecuteSimple(string command, int fd_in, int fd_out, bool shouldWait, int ** fd_array, int n) {
    List<string> * tokens = splitArguments(command);

    if (tokens->getCount() == 2 && tokens->getFirst()->key == "cd") {
        string directory = tokens->getFirst()->next->key;

        chdir(directory.c_str());

        return 0;
    } else {
        delete tokens;
    }

    pid_t child_pid = fork();

    if (child_pid > 0) { // synchronous
        if (child_pid != 0) {
            childrenPids.add(child_pid);
        }

        if (shouldWait) {
            printf("** WAIT (SIMPLE) **\n") ;
            wait(nullptr);
            printf("** WAIT END (SIMPLE )**\n") ;
        }

        if (child_pid != 0) {
            childrenPids.remove(child_pid);
        }
    } else {
        static struct sigaction act;
        act.sa_handler = SIG_DFL;
        sigaction(SIGINT, &act, NULL);
        sigaction(SIGTSTP, &act, NULL);

        List<string> * tokens = splitArguments(command);

        if (tokens->getCount() == 0) {
            delete tokens;
            exit(0);
        }

        string program = tokens->getFirst()->key;
        int totalArguments = tokens->getCount() - 1;

        if (fd_in != -1) {
            dup2(fd_in, 0);
            if (fd_array == NULL) {
                close(fd_in);
            }
        }

        if (fd_out != -1) {
            dup2(fd_out, 1);
            if (fd_array == NULL) {
                close(fd_out);
            }
        }

        if (fd_array != NULL) {
            for (int i = 0; i < n; i++) {
                close(fd_array[i][0]);
                close(fd_array[i][1]);
            }
        }


        if (totalArguments == 0) {
            execlp(program.c_str(), program.c_str(), nullptr);
            fprintf(stderr, "\t command not found - execlp failed: %s \n", program.c_str());
            exit(0);
        } else {
            char ** args = new char*[totalArguments + 2];
            for (int i = 0; i < totalArguments + 2; i++) {
                args[i] = nullptr;
            }

            Node<string> * current = tokens->getFirst();

            args[0] = (char*) current->key.c_str();


            for (int i = 1; i <= totalArguments; i++) {
                current = current->next;

                if (current->key == "<" || current->key == ">" || current->key == ">>" || current->key == "|") {
                    args[i] = NULL;
                    totalArguments = i;
                    break;
                }

                args[i] = (char*) current->key.c_str();
            }

            for (int i = 0; i < totalArguments + 1; i++) {
                fprintf(stderr, "\t execvp args[%d]: %s (total:%d) \n", i, args[i], totalArguments);
            }

            execvp(program.c_str(), args);

            fprintf(stderr, "\t command not found - execlp failed: %s \n", program.c_str());

            delete [] args;
            exit(0);
        }

        delete tokens;
    }

    return child_pid;
}

void Shell::operatingSystemExecuteAdvanced(string command) {
    List<string> * tokens = splitArguments(command);

    if (tokens->getCount() == 2 && tokens->getFirst()->key == "cd") {
        string directory = tokens->getFirst()->next->key;

        chdir(directory.c_str());

        return;
    }

    int n = tokens->getCount();

    string * array = tokens->keysToArray();

    for (int i = 0; i < n; i++) {
        cout << "composite command array[" << i << "] = " << array[i] << endl;
    }

    bool existsInputRedirection = false;
    bool existsOutputRedirection = false;
    bool existsOutputAppendRedirection = false;
    string inputRedirectionFile = "";
    string outputRedirectionFile = "";
    string outputRedirectionAppendFile = "";

    int totalPipes = 0;

    for (int i = 0; i < n - 1; i++) {
        string token = array[i];

        if (token == "<") {
            existsInputRedirection = true;
            inputRedirectionFile = array[i + 1];
            i++;
        }

        if (token == ">") {
            existsOutputRedirection = true;
            outputRedirectionFile = array[i + 1];
            i++;
        }

        if (token == ">>") {
            existsOutputAppendRedirection = true;
            outputRedirectionAppendFile = array[i + 1];
            i++;
        }


        if (token == "|") {
            totalPipes++;
        }
    }

    int totalForks = totalPipes + 1;

    cerr << "Input  redirection         : " << existsInputRedirection << " (" << inputRedirectionFile << ")" << endl;
    cout << "Output redirection         : " << existsOutputRedirection << " (" << outputRedirectionFile << ")" << endl;
    cerr << "Output redirection (append): " << existsOutputAppendRedirection << " (" << outputRedirectionAppendFile << ")" << endl;

    string partialCommand = "";

    int ** pipe_fds = NULL;

    if (totalPipes > 0) {
        pipe_fds = (int**) malloc(sizeof (int*)*totalPipes);

        for (int i = 0; i < totalPipes; i++) {
            pipe_fds[i] = (int*) malloc(sizeof (int)*2);
            pipe(pipe_fds[i]);
        }
    }

    bool shouldWait = totalPipes == 0;

    for (int i = 0, forksExecuted = 0; i < n; i++) {
        string token = array[i];

        //        if (i == n - 1) {
        //                if (partialCommand == "") {
        //                    partialCommand = token;
        //                } else {
        //                    partialCommand = partialCommand + " " + token;
        //                }
        //            }

        if (token.length() == 0) {
            continue;
        }

        if (token == "|" || (partialCommand != "" && i == n - 1) || (token == "<" || token == ">" || token == ">>")) {
            int fd_in = -1;
            int fd_out = -1;

            //
            // Assign pipe fds
            //
            if (totalPipes > 0) {
                if (forksExecuted == 0) {
                    fd_out = pipe_fds[0][1];
                } else if (forksExecuted == totalForks - 1) {
                    fd_in = pipe_fds[totalPipes - 1][0];
                } else {
                    fd_in = pipe_fds[forksExecuted - 1][0];
                    fd_out = pipe_fds[forksExecuted][1];
                }
            }

            //
            // Assign redirections fds
            //
            if (forksExecuted == 0 && existsInputRedirection) {
                fd_in = open(inputRedirectionFile.c_str(), O_RDONLY);
                fprintf(stderr, "File opened for stdin: %d \n", fd_in);

                if (fd_in == -1) {
                    fprintf(stderr, "Input redirection failed for file:  %s \n", inputRedirectionFile.c_str());
                    break;
                } else {
                    fprintf(stderr, "Input redirection successful for file:  %s \n", inputRedirectionFile.c_str());
                }
            }

            if (forksExecuted == totalForks - 1 && existsOutputRedirection) {
                fd_out = open(outputRedirectionFile.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0666);
                cout << "File opened for stdout: " << fd_out << endl;

                if (fd_out == -1) {
                    fprintf(stderr, "Input redirection failed for file:  %s \n", outputRedirectionFile.c_str());
                    break;
                } else {
                    fprintf(stderr, "Input redirection successful for file:  %s \n", outputRedirectionFile.c_str());
                }
            }

            if (forksExecuted == totalForks - 1 && existsOutputAppendRedirection) {
                fd_out = open(outputRedirectionAppendFile.c_str(), O_APPEND | O_WRONLY, 0666);
                fprintf(stderr, "File opened  for stdout (append mode): %d \n", fd_out);

                if (fd_out == -1) {
                    fprintf(stderr, "Input redirection (append mode) failed for file:  %s \n", outputRedirectionAppendFile.c_str());
                    break;
                } else {
                    fprintf(stderr, "Input redirection (append mode) successful for file:  %s \n", outputRedirectionAppendFile.c_str());
                }
            }

            cerr << "Executing partial command (simple): " << partialCommand << endl;

            pid_t childpid = operatingSystemExecuteSimple(partialCommand, fd_in, fd_out, shouldWait, pipe_fds, totalPipes);

            childrenPids.add(childpid);

            partialCommand = "";
            forksExecuted++;
            
            if (token == "<" || token == ">" || token == ">>") {
                i++;
            }
            continue;
        } else {
            if (partialCommand == "") {
                partialCommand = token;
            } else {
                partialCommand = partialCommand + " " + token;
            }
        }
    }

    for (int i = 0; i < totalPipes; i++) {
        close(pipe_fds[i][0]);
        close(pipe_fds[i][1]);
    }

    if (true) {
        int x = 0;
        
        printf("wait start (Advanced) \n");
        
        while ((x=wait(NULL)) > 0) {
            printf(" *** %d **** \n", x);
        }
        
        printf("wait end (Advanced) \n");

        childrenPids.clear();
    }

    for (int i = 0; i < totalPipes; i++) {
        free(pipe_fds[i]);
    }

    free(pipe_fds);


    delete [] array;
}

List<pid_t> & Shell::getChildrenPids() {
    return this->childrenPids;
}

List<pid_t> & Shell::getJobChildrenPids() {
    return this->jobManager.getChildrenPids();
}