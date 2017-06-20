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
#include "watcher.h"

int walkerAddToWatchList (const char *fpath, const struct stat *sb, int typeflag);

#define MAX_EVENTS 1024 /*Max. number of events to process at one go*/
#define LEN_NAME 16 /*Assuming that the length of the filename won't exceed 16 bytes*/
#define EVENT_SIZE  ( sizeof (struct inotify_event) ) /*size of one event*/
#define BUF_LEN     ( MAX_EVENTS * ( EVENT_SIZE + LEN_NAME )) /*buffer to store the data of events*/

struct FileInfo {
     FileInfo(int fd_in, int wd_in) : fileDescriptor(fd_in), watchDescriptor(wd_in) {}

    int fileDescriptor;
    int watchDescriptor;
};

// this is the length that will be returned from read()
int readLength;
// this is an iterator to iterate through the buffer of events
int i;
// this is the initial watch Descriptor. it describes what is being watched. if it fails it is not added to the list
int wd;
// this is a list of watch descriptors. each descriptor in this list is a directory being watched.
std::list<int> watchDescriptorList;
// this will be the "master" directory the user points to.
const char* directory;
// TODO describe
int fd;
// TODO describe
char buffer[BUF_LEN];

std::list<FileInfo> descriptorList; // watch descriptor / file descriptor

void Watcher::init(std::string directoryGiven){
    int initialFD;
    int initialWD;
    int fileWalkerReturn;
    readLength = 0;
    i = 0;
    directory = directoryGiven.c_str();

    /* Initialize Inotify*/
    initialFD = inotify_init();
    if ( initialFD < 0 ) {
        perror( "Couldn't initialize inotify");
    }
    /* add watch to starting directory */
    initialWD = inotify_add_watch(initialFD, directory, IN_CREATE | IN_MODIFY | IN_DELETE | IN_MOVED_TO | IN_MOVED_FROM);
    if (wd == -1) {
        printf("Couldn't add watch to %s\n", directory);
        printf("You will need to initialize again.");
    }
    else {
        printf("Watching:: %s\n", directory);
        //watchDescriptorList.push_front(wd);
        descriptorList.emplace_front(initialFD, initialWD);
    }
    fileWalkerReturn = ftw(directory, &walkerAddToWatchList, 4096 );
    if (fileWalkerReturn < 0){
        printf("an error has occured trying to find sub directories.\n");
    }
}

int walkerAddToWatchList (const char *fpath, const struct stat *sb, int typeflag) {
    if (typeflag == FTW_D){
        printf("walker found directory : %s\n", fpath);
        int newFD = inotify_init();
        int newWD = inotify_add_watch(newFD, fpath, IN_CREATE | IN_MODIFY | IN_DELETE | IN_MOVED_TO | IN_MOVED_FROM);
        printf("walker thinks fd is %d\n", newFD);
        printf("walker thinks wd is %d\n", newWD);
        descriptorList.emplace_back(newFD,newWD);
        // watchDescriptorList.push_back(inotify_add_watch(fd, directory, IN_CREATE | IN_MODIFY | IN_DELETE | IN_MOVED_TO | IN_MOVED_FROM));
    }
    return 0;
}
void Watcher::watchMaster () {

    /* do it forever*/
    printf("beginning the while(1) loop\n");
    while (1) {
        for (auto& info : descriptorList){
            i = 0;
            readLength = read( info.fileDescriptor, buffer, BUF_LEN );
            if ( readLength < 0 ) {
                perror( "read" );
            }

            while ( i < readLength ) {
                struct inotify_event *event = ( struct inotify_event * ) &buffer[ i ];
                if ( event->len ) {
                    if ( event->mask & IN_CREATE) {
                        if (event->mask & IN_ISDIR) {
                            printf( "The directory %s was Created.\n", event->name );
                            
                            addToWatchList (event->name);
                        }
                        else {
                            printf( "The file %s was Created with WD %d\n", event->name, event->wd );
                        }
                    }

                    if ( event->mask & IN_MODIFY) {
                        if (event->mask & IN_ISDIR) {
                            printf( "The directory %s was modified.\n", event->name );
                            addToWatchList (event->name);
                        }
                        else {
                            printf( "The file %s was modified with WD %d\n", event->name, event->wd );
                        }
                    }

                    if ( event->mask & IN_DELETE) {
                        if (event->mask & IN_ISDIR) {
                            printf( "The directory %s was deleted.\n", event->name );
                            destroyAWatch (info.fileDescriptor, info.watchDescriptor);
                        }
                        else {
                            printf( "The file %s was deleted with WD %d\n", event->name, event->wd );
                        }
                    }

                    if ( event->mask & IN_MOVED_TO) {
                        if (event->mask & IN_ISDIR) {
                            printf( "The directory %s was moved in.\n", event->name );
                            addToWatchList (event->name);
                        }
                        else {
                            printf( "The file %s was moved in WD %d\n", event->name, event->wd );
                        }
                    }

                    if ( event->mask & IN_MOVED_FROM) {
                        if (event->mask & IN_ISDIR) {
                            printf( "The directory %s was moved out.\n", event->name );
                            destroyAWatch (info.fileDescriptor, info.watchDescriptor);
                        }
                        else {
                            printf( "The file %s was moved out WD %d\n", event->name, event->wd );
                        }
                    }
                    i += EVENT_SIZE + event->len;
                }
            }
        }
    }

    /* Clean up*/
    // inotify_rm_watch( fd, wd );
    // close( fd );
}
int Watcher::addToWatchList (char *fpath) {
    int newFD = inotify_init();
    int newWD = inotify_add_watch(newFD, fpath, IN_CREATE | IN_MODIFY | IN_DELETE | IN_MOVED_TO | IN_MOVED_FROM);
    descriptorList.emplace_back(newFD,newWD);
    // watchDescriptorList.push_back(inotify_add_watch(fd, fpath, IN_CREATE | IN_MODIFY | IN_DELETE | IN_MOVED_TO | IN_MOVED_FROM));
    return 0;
}
void Watcher::destroyAWatch (int fileDescriptor, int watchDescriptor){
    inotify_rm_watch( fileDescriptor, watchDescriptor );
    watchDescriptorList.remove(watchDescriptor);
}


Watcher::~Watcher(){
    for(auto& fileDesc : watchDescriptorList) {
        inotify_rm_watch(fd, fileDesc);
    }
    close( fd );
}
