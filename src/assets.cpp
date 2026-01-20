#include "assets.hpp"
#include "common.hpp"


Assets::Assets() {
    char b[] = "/home/bandeira/Documents/GIT/PhysicsEngine/";
    directory = (char*)malloc(sizeof(b));
    memcpy(directory, b, sizeof(b));
    update();
}
Assets::~Assets() {
    free(directory);
}

void Assets::update() {
    if( files.size() ) files.clear();
    if( directory == nullptr ) return;

    char *command = (char*) malloc(strlen(directory) + 19);
    sprintf(command, "ls %s -p > .assets", directory);
    pclose(popen(command, "r"));

    unsigned int fileLenght;
    char* buffer = utils.read_file(".assets", &fileLenght);
    unsigned int i = 0;
    File fParent;
    fParent.completeName = (char*)"../";
    fParent.type = FILE_TYPES::FOLDER_FILE;
    files.push_back(fParent);
    while(1) {

        unsigned int k = i;
        while( buffer[i] != '\n' && buffer[i] != 0 ) i++;

        if( buffer[i] == 0 ) break;
        if( i == k ){
            i++;
            continue;
        }
        char *c = (char*) malloc(i - k + 1);
        memcpy(c, (buffer + k), (i - k));

        c[i - k] = 0;

        unsigned int exteIndex = utils.get_extension_index(c, i - k + 1);
        File file;
        if( c[i - k - 1] == '/' ) file.type = FILE_TYPES::FOLDER_FILE;
        file.completeName = c;
        if( exteIndex == 0 ) {
            file.extension = nullptr;
            file.simpleName = c;
        }
        else {
            file.extension = (char*)malloc(i - k + 1 - exteIndex);
            memcpy(file.extension, c + exteIndex, i - k + 1 - exteIndex);
            file.extension[i - k - exteIndex] = 0;

            file.simpleName = (char*)malloc(exteIndex + 1);
            memcpy(file.simpleName, c, exteIndex);
            file.simpleName[exteIndex] = 0;
        }

        if( file.extension != nullptr && utils.match_pairs(file.extension, 1, {'o', 'b', 'j'}) ) file.type = MESH_FILE;
        files.push_back(file);
        i++;
    }
    free(buffer);
}
