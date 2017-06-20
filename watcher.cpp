// #include <iostream>
// #include <iomanip>
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
        // printf("full path : %s\n", ftwbuf->base);

        // int newFD = inotify_init();
        // int newWD = inotify_add_watch(newFD, fpath, IN_CREATE | IN_MODIFY | IN_DELETE | IN_MOVED_TO | IN_MOVED_FROM);
        // printf("walker thinks fd is %d\n", newFD);
        // printf("walker thinks wd is %d\n", newWD);
        // descriptorList.emplace_back(newFD,newWD);
        // watchDescriptorList.push_back(inotify_add_watch(fd, directory, IN_CREATE | IN_MODIFY | IN_DELETE | IN_MOVED_TO | IN_MOVED_FROM));
    }
    else {
        printf("walker found file : %s\n", fpath);
    }
    return 0;
}


Watcher::~Watcher(){

}
    // #include <dirent.h>
    // DIR *dir;
    // struct dirent *ent;
    // if ((dir = opendir (directoryGiven)) != NULL) {
    //     while ((ent = readdir (dir)) != NULL) {
    //         if ( strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0){
    //             printf ("%s\n", ent->d_name);
    //         }
    //     }
    //     closedir (dir);
    // }
    // else {
    //     perror ("");
    //     return EXIT_FAILURE;
    // }