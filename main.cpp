
#include <string>
#include "watcher.h"
#include <dirent.h>
#include <stdio.h>
#include <string.h>

int main( int argc, char **argv )
{
    Watcher watcher;
    std::string test = "test";
    watcher.init(test);
    

    return 0;
}