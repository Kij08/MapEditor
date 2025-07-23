//
// Created by Kij on 25/07/19.
//

#include "FileManager.h"

//Path to the dir that contains the map file
inline std::filesystem::path currentMapDirectory;
//Path to the dir that the user has selected in the file viewer
inline std::filesystem::path currentWorkingDirectory;
inline std::ofstream logFile;

//Helper functions
void WriteObjectDefinitionToFile(std::ifstream& mapFile, ObjectDefinition& objectDefinition);
bool CreateSceneFromFile(std::ifstream& mapFile, Scene*& s);
std::istream& GetNextLineInFile(std::ifstream& file, std::string& line);
std::filesystem::path OpenChooseFileDialog();

//Component definitions
void GetComponentDefinition(const std::string& line, ObjectDefinition& definition);
MeshComponentDefinition* GetMeshCompDefinition(std::string line);


//Opens the platform specific dialog for choosing a specific map file
std::filesystem::path OpenChooseFileDialog() {
    std::filesystem::path openedFile;
#if Linux
    //Linux file open dialog handling
    FILE* f = popen("zenity --file-selection --file-filter *.tmap", "r");
    char buffer[512];
    while (fgets(buffer, 512, f)) {
        //puts(buffer);
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

//Opens the platform specific dialog for choosing a directory
std::filesystem::path FileManager::OpenChooseDirectoryDialog() {
    std::filesystem::path openedDirectory;
#if Linux
    //Linux file open dialog handling
    FILE* f = popen("zenity --file-selection --directory", "r");
    char buffer[512];
    while (fgets(buffer, 512, f)) {
        //puts(buffer);
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
std::istream& GetNextLineInFile(std::ifstream& file, std::string& line) {
    std::istream& isStillReading = getline(file, line);

    //Remove any control characters (i.e. newlines) in the file line
    std::erase_if(line, [](char c) { return std::iscntrl(c); });

    return isStillReading;
}


MeshComponentDefinition* GetMeshCompDefinition(std::string line) {
    MeshComponentDefinition* mc = new MeshComponentDefinition();

    //Find next space and store that string value in the comp def
    size_t spacePos = line.find_first_of(' ');
    mc->componentTransform = Transform::StringToTransform(line.substr(0, spacePos));

    //Repeatedly cut out the beginning to the first space of the string and transform that next chunk to data
    line = line.substr(spacePos + 1);
    spacePos = line.find_first_of(' ');
    std::string meshPath = line.substr(0, spacePos);
    mc->meshPath = meshPath;

    line = line.substr(spacePos + 1);
    spacePos = line.find_first_of(' ');
    std::string texPath = line.substr(0, spacePos);
    mc->texturePath = texPath;

    return mc;
}

void GetComponentDefinition(const std::string& line, ObjectDefinition& definition) {
    std::string lineSubString = line.substr(line.find_first_of(' ') + 1);
    std::string compClass = lineSubString.substr(0, lineSubString.find_first_of(' '));
    std::string definitionStart = lineSubString.substr(lineSubString.find_first_of(' ') + 1);
    if (compClass == FileManager::MeshComponent) {
        MeshComponentDefinition* mc = GetMeshCompDefinition(definitionStart);
        definition.ComponentDefinitions.push_back(mc);
    }
}

bool CreateSceneFromFile(std::ifstream& mapFile, Scene*& s) {
    std::string line;
    //Create new scene from map file

    //Store all map file object definitions before constructing the scene
    std::vector<ObjectDefinition> objectDefinitions;
    while (GetNextLineInFile(mapFile, line)) {
        std::cout << line << std::endl;

        std::string marker = line.substr(0, line.find_first_of(' '));

        //Go down the file lines and check if the file fits the format. If it does create a scene, if it doesn't then return an error.
        if (marker == FileManager::ObjectMarker) {
            ObjectDefinition objectDef;
            //If the object marker is found then parser for the object name
            objectDef.objectName = line.substr(line.find_first_of(' ') + 1);

            //Get next line of file
            GetNextLineInFile(mapFile, line);
            marker = line.substr(0, line.find_first_of(' '));
            if (marker == FileManager::ClassMarker) {
                objectDef.className = line.substr(line.find_first_of(' ') + 1);

                //Get next line of file
                GetNextLineInFile(mapFile, line);
                marker = line.substr(0, line.find_first_of(' '));
                if (marker == FileManager::TransformMarker) {
                    std::string transformString = line.substr(line.find_first_of(' ') + 1);
                    objectDef.transform = Transform::StringToTransform(transformString);

                    //Get next line of file
                    GetNextLineInFile(mapFile, line);
                    marker = line.substr(0, line.find_first_of(' '));
                    if (marker == FileManager::ComponentMarker) {

                        GetComponentDefinition(line, objectDef);


                        //while this objects definition is not ended, keep getting lines and reading in components
                        bool notObjectEnd = true;
                        while (notObjectEnd) {
                            GetNextLineInFile(mapFile, line);

                            if (mapFile.eof() && line == FileManager::EndMarker) {
                                //object and map file have both ended
                                objectDefinitions.push_back(objectDef);
                                break;

                            }
                            if (line == FileManager::EndMarker) {
                                //Object has ended so store this obj definition and move onto next one
                                objectDefinitions.push_back(objectDef);
                                notObjectEnd = false;
                            }
                            else if (mapFile.eof()) {
                                //Return false if map file ended without proper end marker
                                FileManager::WriteStringToLog("File malformed. No end detected.");
                                return false;
                            }

                            //If none of these then just get the next component
                            GetComponentDefinition(line, objectDef);
                        }
                    }
                }
            }
            else {
                //If no class file malformed
                FileManager::WriteStringToLog("File malformed. No object class specified. " + line);
                return false;
            }
        }
        else if (line == FileManager::EndMarker) {
            //If the first line is an end marker its an empty map file so we don't add anything and make an empty scene
            break;
        }
        else {
            FileManager::WriteStringToLog("File malformed. No object specified.");
            mapFile.close();
            return false;
        }

    }

    //If the loop has exited then the file is properly formed
    //Delete the old scene and make a new one
    delete s;
    s = new Scene(objectDefinitions);

    mapFile.close();
    FileManager::WriteStringToLog("Map file successfully opened.");
    return true;
}

bool FileManager::OpenMapFile(Scene*& s) {
    WriteStringToLog("Opening Map File...");
    //filesystem should be cross-platform so once we have the filepath we can work with it on both OSs
    std::filesystem::path mapToLoad = OpenChooseFileDialog();
    currentMapDirectory = mapToLoad.parent_path();
    currentWorkingDirectory = currentMapDirectory;
    SetNewCurrentDirectory(currentWorkingDirectory.append("Map Content/"));

    std::ifstream mapFile(mapToLoad);
    if (mapFile.fail()) {
        WriteStringToLog("Error opening map file: " + mapToLoad.string());
        return false;
    }

    return CreateSceneFromFile(mapFile, s);

}

void FileManager::SaveCurrentMap(Scene* s) {
    for (auto& obj : s->GetObjectList()) {

    }
}

void FileManager::SaveCurrentMapAs(Scene* s) {

}

bool FileManager::ValidateFilePath(const std::string& path) {
    return std::filesystem::exists(path);
}

bool FileManager::CreateNewMap(const std::string& newMapPath, const std::string& newMapName, Scene*& s) {
    std::filesystem::path mapPath(newMapPath);

    //Create a folder named as "map name" in the specified directory
    mapPath.append(newMapName);
    std::filesystem::create_directory(mapPath);
    currentMapDirectory = mapPath;


    //In the created directory make a map file named "map name" and a "Map Content" directory
    std::ofstream mapFile( mapPath.string() + '/' + newMapName + ".tmap");
    mapFile << EndMarker << std::endl; //Write end marker to denote empty scene
    std::filesystem::create_directory(mapPath.string() + "Map Content");
    std::filesystem::create_directory(mapPath.string() + "Logs");
    SetNewCurrentDirectory(mapPath.string() + ("Map Content/"));
    mapFile.close();

    std::ifstream readMapFile(mapPath.string() + '/' + newMapName + ".tmap");

    return CreateSceneFromFile(readMapFile, s);
}

//Updates DirectoryFiles to the new current directory. Don't call every frame
void UpdateFilesInCurrentDirectory() {
    FileManager::DirectoryFiles.clear();
    for (const auto& file : std::filesystem::directory_iterator{currentWorkingDirectory}) {
        FileManager::DirectoryFiles.push_back(file.path());
    }
}

void FileManager::SetNewCurrentDirectory(std::filesystem::path newDir) {
    if (ValidateFilePath(newDir)) {
        currentWorkingDirectory = newDir;
        WriteStringToLog("New working directory: " + newDir.string());
        //UpdateFilesInCurrentDirectory(); TODO: Fix updating dir files
    }
    else {
        WriteStringToLog("Set working dir to invalid path");
    }
}
