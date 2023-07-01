

#include "AliasManager.h"
using namespace std;

AliasManager::AliasManager() {
}

AliasManager::~AliasManager() {
}

void AliasManager::add(string alias, string command) {
    if (list.search(alias) != "") {
        list.remove(alias);
        list.add(alias, command);
    } else {
        list.add(alias, command);
    }
}

string AliasManager::getCommand(string alias) {
    return list.search(alias);
}

void AliasManager::remove(string alias) {
    list.remove(alias);
}

void AliasManager::print() {
    list.print();
}