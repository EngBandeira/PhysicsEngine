#include "assets.hpp"
#include "common.hpp"


void Assets::init() {
    char b[] = "/home/bandeira/Documents/GIT/PhysicsEngine/";
    directory = (char*)malloc(sizeof(b));
    memcpy(directory, b, sizeof(b));
    files = (File*)malloc(sizeof(File));
    File fParent;
    fParent.completeName = (char*)"../";
    fParent.type = FILE_TYPES::FOLDER_FILE;
    files[0] = (fParent);
    update();
}
void Assets::free_data() {
    for(int i = 1; i < files_number; i++){
        if(files[i].simpleName)
            free(files[i].simpleName);
        if(files[i].extension)
            free(files[i].extension);
        if(files[i].completeName)
            free(files[i].completeName);
    }
    free(files);
    free(directory);
}

void Assets::update() {
    for(int i = 1; i < files_number; i++){
        if(files[i].simpleName)
            free(files[i].simpleName);
        if(files[i].extension)
            free(files[i].extension);
        if(files[i].completeName)
            free(files[i].completeName);
    }
    if( directory == nullptr ) return;

    char *command = (char*) malloc(strlen(directory) + 19);
    sprintf(command, "ls %s -p > .assets", directory);
    pclose(popen(command, "r"));

    unsigned int fileLenght, file_i = 1, i = 0;
    char* buffer = utils.read_file(".assets", &fileLenght);
    files_number = 1;
    for(int i = 0; i < fileLenght; i++) {
        if(buffer[i] == '\n')
            files_number++;
    }

    files = (File*)realloc(files,sizeof(File) * files_number);
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

            file.simpleName = (char*)malloc(i - k + 1);
            memcpy(file.simpleName, c, i - k + 1);
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
        files[file_i++] = file;
        i++;
    }
    free(buffer);
    free(command);
}

File::~File(){

}
