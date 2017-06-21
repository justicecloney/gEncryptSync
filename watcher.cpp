#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/inotify.h>
#include <limits.h>
#include <unistd.h>
#include <string>
#include <list>
#include <ftw.h>
#include <dirent.h>
#include "watcher.h"
#include <time.h>

int walkerAddToWatchList (const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf);


// this will be the "master" directory the user points to.
const char* directory;
int sleepTime;

void Watcher::init(std::string directoryGiven){
    directory = directoryGiven.c_str();
    sleepTime = 1;
    int fileWalkerReturn;
    // TODO maybe give the user a way to use some of these. Don't cross mount points, dont follow sym links etc.
    int flags = 0;
    fileWalkerReturn = nftw(directory,             // Directory to be walked
                            &walkerAddToWatchList, // Function called for each entry in tree
                            4096,                  // Max directories held open simultaneously. 
                            flags                  // ORing zero or more of the flags. see http://man7.org/linux/man-pages/man3/ftw.3.html
    );
    if (fileWalkerReturn < 0){
        printf("an error has occured trying to find sub directories.\n");
    }
}

int walkerAddToWatchList (const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {

    if (typeflag == FTW_D){
        printf("walker found directory : %s\n", fpath);
    }
    else {
        printf("walker found file : %s\n", fpath);
    }
    printf("it's last modification was : %s\n", ctime(&sb->st_mtim.tv_sec)); // ctime has a carriage return at the end of it.
    printf("that's the integer %s\n", std::to_string(sb->st_mtim.tv_sec).c_str());
    // printf("it's last status change was : %s\n", sb->st_ctim);
    return 0;
}


Watcher::~Watcher(){

}
