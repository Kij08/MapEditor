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

#include <fstream>
#include <filesystem>
#include <iostream>
#include <string>
#include <chrono>
#include <any>
#include "../Scene/Scene.h"

namespace FileManager {
    //List of markers for file reading
    constexpr std::string ObjectMarker = "o";
    constexpr std::string ClassMarker = "cl";
    constexpr std::string TransformMarker = "t";
    constexpr std::string ComponentMarker = "co";
    constexpr std::string EndMarker = "e";

    //List of classes. In lieu of writing a reflection system (c++26!!!)/doing code generation shenanigans, Ill do this for now
    constexpr std::string EmptyClass = "Empty";
    constexpr std::string ObjectClass = "Object";

    //List of components for same reason as above
    constexpr std::string MeshComponent = "Mesh";
    constexpr std::string CameraComponent = "Camera";


    //Path to the dir that contains the map file
    inline std::filesystem::path currentMapDirectory;
    inline std::filesystem::path currentMapFile;
    //Path to the dir that the user has selected in the file viewer
    inline std::filesystem::path currentWorkingDirectory;
    inline std::ofstream logFile;

    inline void OpenLogFile() {
        std::filesystem::path logPath = "Log.txt";
        logFile.open(logPath, std::ios::out);
    }

    inline void WriteStringToLog(std::string logStr) {
        logFile << "[" << std::chrono::system_clock::now() << "] " << logStr << std::endl;
    }

    inline void CloseLogFile() {
        logFile.close();
    }

    std::filesystem::path OpenChooseDirectoryDialog();

    bool OpenMapFile(Scene*& s);

    void SaveCurrentMap(Scene* s);

    void SaveCurrentMapAs(Scene* s);

    bool ValidateFilePath(const std::string& path);

    bool CreateNewMap(const std::string& newMapPath, const std::string& newMapName, Scene*& s);

    //Stores the files that are in the users current working directory
    inline std::vector<std::filesystem::path> DirectoryFiles;

    void SetNewCurrentDirectory(const std::filesystem::path& newDir);
};



#endif //FILEMANAGER_H
