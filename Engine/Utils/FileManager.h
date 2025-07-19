//
// Created by Kij on 25/07/19.
//

#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#ifdef _WIN32
#define Windows 1
#include <windows.h>

#elif __linux__
#define Linux 1
#endif


#include <filesystem>

namespace FileManager {

    inline void OpenFileDialog() {
#if Linux
        FILE* f = popen("zenity --file-selection", "r");
        char buffer[512];
        while (fgets(buffer, 512, f)) {
            puts(buffer);
        }
        pclose(f);
#elif Windows

#endif

    }
};



#endif //FILEMANAGER_H
