#ifndef ALIASMANAGER_H
#define ALIASMANAGER_H

#include <string>

#include "List.h"

using namespace std;

class AliasManager {
public:
    AliasManager();
    virtual ~AliasManager();

    void add(string alias, string command);
    void remove(string alias);
    string getCommand(string alias);

    void print();
private:
    List<string> list;
};

#endif /* ALIASMANAGER_H */
