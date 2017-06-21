# gEncryptSync
Scans for file changes, encrypts the file, then uploads to a google drive account

Watching every directory has been found to be impractical. Instead I will now scan subdirectories periodically to search for changes.

Currently working on :
make tree of last upload times

Todo:
-make tree of last upload times
    -make my tree object
    -make my dir object
    -make my file object
    -save the tree object
-set/change directory watched
-compare last modified
    -if modified since last upload, re-"upload"
-make it loop
-// at this point the only debug output I want is "change detected"
-upload to google
-encrypt
-store keys


Things that work:
    -watcher successfully scans directories recursively and returns both files and folders found with a relative path to watched directory.
    -check last modified date