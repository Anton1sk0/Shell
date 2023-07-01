

#include "WildcardEvaluator.h"

WildcardEvaluator::WildcardEvaluator() {
}

WildcardEvaluator::~WildcardEvaluator() {
}

List<string> * WildcardEvaluator::evaluate(string expression) {
    List<string> * list = new List<string>();
    // opendr
    DIR* dir = opendir(".");

    // readir

    if (dir != NULL) {
        struct dirent* entry;
        while ((entry = readdir(dir)) != NULL) {
            if (fnmatch(expression.c_str(), entry->d_name, 0) == 0) {
                // for each file if it matches expression addit to list
                list->add(entry->d_name);
            }
        }
        // closedir
        closedir(dir);
    }
    return list;

}
