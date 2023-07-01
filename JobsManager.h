#ifndef JobsManager_H
#define JobsManager_H
#include <sys/types.h>


#include <string>

#include "List.h"

using namespace std;

class JobsManager {
public:
    JobsManager();
    virtual ~JobsManager();
    
    List<pid_t> & getChildrenPids();
    
    pid_t wakeFirstChild();
private:
    List<pid_t> childrenPids;

};

#endif /* JobsManager_H */

