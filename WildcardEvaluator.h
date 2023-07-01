
#ifndef WILDCARDEVALUATOR_H
#define WILDCARDEVALUATOR_H
#include <fnmatch.h>
#include <sys/types.h>
 #include <dirent.h>


#include <string>

#include "List.h"

using namespace std;

class WildcardEvaluator {
public:
    WildcardEvaluator();
    virtual ~WildcardEvaluator();
    
    List<string> * evaluate(string expression);
private:

};

#endif /* WILDCARDEVALUATOR_H */

