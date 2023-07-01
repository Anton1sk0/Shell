
#ifndef HISTORYMANAGER_H
#define HISTORYMANAGER_H

#include <iostream>
#include <string>

#include "List.h"

using namespace std;

class HistoryManager {
public:
    HistoryManager();
    virtual ~HistoryManager();

    void add(string command);
    string getCommand(int index);

    void print();

private:
    List<int> history;
};

#endif /* HISTORYMANAGER_H */
