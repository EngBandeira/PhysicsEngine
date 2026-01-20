#include "persistent_data.hpp"
#include "common.hpp"
#include "utils.hpp"
#include <X11/X.h>
#include <cstdlib>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>

PersistentDataReturn *PersistentData::get(){
    char *home = getenv("HOME");
    unsigned int clenght = strlen(home) + 35;
    char *str = (char*)malloc(clenght);
    sprintf(str, "%s/.local/share/PhysicsEngine/conf",home);
    return get(str);
}


PersistentDataReturn *PersistentData::get(char *file_path) {
    unsigned int clenght = strlen(file_path);

    char k = file_path[clenght-5];
    file_path[clenght -5] = 0;
    if(!access(file_path, F_OK) ) {
        char *s = (char*)malloc(strlen(file_path) + 7);
        sprintf(s, "mkdir %s",file_path);
        pclose(popen(s, "r"));
    }

    file_path[clenght -5] = k;

    if(! access(file_path, F_OK) ) {
        char *s = (char*)malloc(clenght + 7);
        sprintf(s, "touch %s",file_path);
        pclose(popen(s, "r"));
    }

    unsigned int file_length;
    char *file = utils.read_file(file_path, &file_length);
    unsigned int i = 0;
    while(i < file_length){
        unsigned int k = i;
        while(file[i] != ' ') i++;

        char *name = (char*)malloc(i - k + 2);
        memcpy(name, file + k, i - k + 1);
        name[i - k + 1] = 0;
        k = i;
        while(file[i] != '\n' && file[i] != 0) i++;
        char *value = (char*)malloc(i - k + 2);
        memcpy(value, file + k, i - k + 1);
        value[i - k + 1] = 0;
        k = i;
    }
}
