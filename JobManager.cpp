
#include <signal.h>

#include "JobsManager.h"


JobsManager::JobsManager() {

}

JobsManager::~JobsManager() {

}

List<pid_t> & JobsManager::getChildrenPids() {
    return childrenPids;
}

pid_t JobsManager::wakeFirstChild() {
    pid_t victim = 0;
    
    for (Node<pid_t> * current = childrenPids.getFirst(); current != nullptr; current = current->next) {
        pid_t target = current->key;
        
        if (target != 0) {
            kill(target, SIGCONT);
            victim = target;
            break;
        } else {
            printf("ERROR, PID 0 in list detected \n");
        }
    }
    
    if (victim != 0) {
        childrenPids.remove(victim);
    }
    
    return victim;
}