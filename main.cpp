
#include <string>
#include "watcher.h"


int main( int argc, char **argv )
{
    Watcher watcher;
    std::string test = "test";
    watcher.init(test);
    watcher.watchMaster();


    return 0;
}