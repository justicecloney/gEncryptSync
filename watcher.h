#include <string>

#ifndef WATCHER_H
#define WATCHER_H


class Watcher {
public:
    void watchMaster();
    void init(std::string directoryGiven);
    ~Watcher();
private:
    int addToWatchList (char *fpath);
    void destroyAWatch (int fileDescriptor, int watchDescriptor);
};
#endif