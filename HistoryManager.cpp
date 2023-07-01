
#include <iostream>
#include <string>

#include "HistoryManager.h"

using namespace std;

HistoryManager::HistoryManager() {
}

HistoryManager::~HistoryManager() {
}

void HistoryManager::add(string command) {
    int count = history.getCount();
    
    history.add(count, command);
    
    if (count > 20) {
        history.removeFirst();
    }
}

string HistoryManager::getCommand(int number) {
    return history.searchByPosition(number);
}

void HistoryManager::print() {
    history.print();
}