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
#include <any>
#include "../Scene/Scene.h"

namespace FileManager {
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
    //Path to the dir that the user has selected in the file viewer
    inline std::filesystem::path currentWorkingDirectory;

    inline std::filesystem::path OpenChooseFileDialog() {
        std::filesystem::path openedFile;
    #if Linux
        //Linux file open dialog handling
        FILE* f = popen("zenity --file-selection --file-filter *.tmap", "r");
        char buffer[512];
        while (fgets(buffer, 512, f)) {
            puts(buffer);
        }
        pclose(f);

        std::string path(buffer);
        //Trim new line from zenity return
        std::erase_if(path, [](char c) { return std::iscntrl(c); });


    #elif Windows

    #endif

        openedFile = std::filesystem::path(path);
        currentMapDirectory = openedFile.parent_path();
        return openedFile;
    }

    inline std::filesystem::path OpenChooseDirectoryDialog() {
        std::filesystem::path openedDirectory;
    #if Linux
        //Linux file open dialog handling
        FILE* f = popen("zenity --file-selection --directory", "r");
        char buffer[512];
        while (fgets(buffer, 512, f)) {
            puts(buffer);
        }
        pclose(f);

        std::string path(buffer);
        //Trim new line from zenity return
        std::erase_if(path, [](char c) { return std::iscntrl(c); });
    #elif Windows

    #endif

        openedDirectory = std::filesystem::path(path);
        return openedDirectory;
    }

    //Wrapper for getline() to get line without control characters
    inline std::istream& GetNextLineInFile(std::ifstream& file, std::string& line) {
        std::istream& isStillReading = getline(file, line);

        //Remove any control characters (i.e. newlines) in the file line
        std::erase_if(line, [](char c) { return std::iscntrl(c); });

        return isStillReading;
    }

    //Transforms a comma seperated transform string into a Transform type
    inline Transform StringToTransform(std::string s) {
        int transformValues[9];
        for (int i = 0; i < 9; i++) {
            size_t bitEndPos = s.find_first_of(',');
            std::string transformBit = s.substr(0, bitEndPos);
            s = s.substr(bitEndPos + 1);

            transformValues[i] = std::stoi(transformBit);
        }
        return {.position = {transformValues[0], transformValues[1], transformValues[2]},
        .rotation = {transformValues[3], transformValues[4], transformValues[5]},
        .scale = {transformValues[6], transformValues[7], transformValues[8]}};
    }

    inline void AddDefinedObjectToScene(Scene* s, const ObjectDefinition& objectDef) {
        if (objectDef.className == EmptyClass) {
            //s->CreateObject<Empty>(objectDef.objectName); TODO: Empty implementation
        }
        else if (objectDef.className == ObjectClass) {
            s->CreateObject<Object>(objectDef.objectName);
        }
    }

    inline bool OpenMapFile(Scene*& s) {
        //filesystem should be cross-platform so once we have the filepath we can work with it on both OSs
        std::filesystem::path mapToLoad = OpenChooseFileDialog();
        std::ifstream mapFile(mapToLoad);
        if (mapFile.fail()) {
            std::cout << "Error opening map file: " << mapToLoad.string() << std::endl;
            return false;
        }
        std::string line;

        //Create new scene from map file

        //Store all map file object definitions before constructing the scene
        std::vector<ObjectDefinition> objectDefinitions;
        while (GetNextLineInFile(mapFile, line)) {
            std::cout << line << std::endl;

            std::string marker = line.substr(0, line.find_first_of(' '));

            //Go down the file lines and check if the file fits the format. If it does create a scene, if it doesn't then return an error.
            if (marker == ObjectMarker) {
                ObjectDefinition objectDef;
                //If the object marker is found then parser for the object name
                objectDef.objectName = line.substr(line.find_first_of(' ') + 1);

                //Get next line of file
                GetNextLineInFile(mapFile, line);
                marker = line.substr(0, line.find_first_of(' '));
                if (marker == ClassMarker) {
                    objectDef.className = line.substr(line.find_first_of(' ') + 1);

                    //Get next line of file
                    GetNextLineInFile(mapFile, line);
                    marker = line.substr(0, line.find_first_of(' '));
                    if (marker == TransformMarker) {
                        std::string transformString = line.substr(line.find_first_of(' ') + 1);
                        objectDef.transform = StringToTransform(transformString);

                        //Get next line of file
                        GetNextLineInFile(mapFile, line);
                        marker = line.substr(0, line.find_first_of(' '));
                        if (marker == ComponentMarker) {
                            //while this objects definition is not ended, keep getting lines and reading in components
                            bool notObjectEnd = true;
                            while (notObjectEnd) {
                                GetNextLineInFile(mapFile, line);
                                if (mapFile.eof() && line == EndMarker) {
                                    //object and map file have both ended
                                    //TODO: Scene setup logic here
                                    return true;

                                }
                                if (line == EndMarker) {
                                    //Object has ended so store this obj definition and move onto next one
                                    objectDefinitions.push_back(objectDef);
                                    notObjectEnd = false;
                                }
                                else if (mapFile.eof()) {
                                    //Return false if map file ended without proper end marker
                                    std::cout << "File malformed. No end detected." << std::endl;
                                    return false;
                                }

                            }
                        }
                    }
                }
                else {
                    //If no class file malformed
                    std::cout << "File malformed. No object class specified." << std::endl;
                    return false;
                }
            }
            else {
                std::cout << "File malformed. No object specified." << std::endl;
                mapFile.close();
                return false;
            }

        }

        s = new Scene();

        mapFile.close();
        return true;
    }

    inline void SaveCurrentMap(Scene* s) {

    }

    inline void SaveCurrentMapAs(Scene*& s) {

    }

    inline bool ValidateFilePath(const std::string& path) {
        return std::filesystem::exists(path);
    }

    inline void CreateNewMap(const std::string& newMapPath, const std::string& newMapName) {
        std::filesystem::path mapPath(newMapPath);

        //Create a folder named as "map name" in the specified directory
        mapPath.append(newMapName);
        std::filesystem::create_directory(mapPath);
        currentMapDirectory = mapPath;

        //In the created directory make a map file named "map name" and a "Map Content" directory
        std::ofstream mapFile( mapPath.string() + newMapName + ".tmap");
    }

    //Stores the files that are in the users current working directory
    inline std::vector<std::string> DirectoryFiles;

    //Updates DirectoryFiles to the new current directory. Don't call every frame
    inline void UpdateFilesInCurrentDirectory() {
        DirectoryFiles.clear();
        for (const auto& file : std::filesystem::directory_iterator(currentWorkingDirectory)) {
            DirectoryFiles.push_back(file.path().filename().string());
        }
    }
};



#endif //FILEMANAGER_H
