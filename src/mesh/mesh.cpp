#include <X11/X.h>
#include <glm/ext/quaternion_transform.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <string.h>

#include "mesh.hpp"
#include "common.hpp"


unsigned int fileLenght;
char *buffer;

void Mesh::sanatize() {
    o = (char**)malloc(0);
    fileLenght = 0;
    buffer = utils.read_file(meshPath, &fileLenght);
    unsigned int path_lenght = strlen(meshPath);
    unsigned int point_i = utils.get_extension_index((char*)meshPath, path_lenght);
    char *ext = (char*)malloc(path_lenght - point_i);
    memcpy(ext, meshPath + point_i + 1, path_lenght - point_i);
    if(utils.match_pairs(ext, 0, (char[3]){'o','b','j'}))
        obj();
    if(utils.match_pairs(ext, 0, (char[2]){'p','e'}))
        pe();

    free(buffer);
}
// abce0
// 01234
void Mesh::pe() {
    obj();
    vertices_index = (unsigned int*)malloc(sizeof(int) * vertices_count / 3);
    for(unsigned int i = 0; i < vertices_count / 3; i++){
        vertices_index[i] = i;
    }
}

void Mesh::obj(){
    char c;
    unsigned int i = 0;
    while(fileLenght - 1 > i) {

        if(buffer[i] == '#' ) {
            while(buffer[i] != '\n') i++;
            i++;
            continue;
        }
        if(buffer[i] == 'f'){
            unsigned int space = 0;
            while(buffer[i] != '\n' && buffer != 0){
                if(buffer[i] == ' ')
                    space++;
                i++;
            }
            index_count += 3 * (space - 2);
            i++;
            continue;
        }
        if(buffer[i] == 'v' && buffer[i+1] == ' ')
            vertices_count += 3;
        if(buffer[i] == 'v' && buffer[i+1] == 'n')
            normal_vertices_count += 3;
        if(buffer[i] == 'v' && buffer[i+1] == 't')
            texture_vertices_count += 2;
        while(buffer[i] != '\n' && buffer != 0) i++;

        if(buffer[i] == 0 ) break;

        i++;
        // continue;
    }



    i = 0;
    vertices = (float*)malloc(sizeof(float) * vertices_count);
    texture_vertices = (float*)malloc(sizeof(float) * texture_vertices_count);
    normal_vertices = (float*)malloc(sizeof(float) * normal_vertices_count);

    vertices_index = (unsigned int*)malloc(sizeof(int) * index_count);
    texture_vertices_index = (unsigned int*)malloc(sizeof(int) * index_count);
    normal_vertices_index = (unsigned int*)malloc(sizeof(int) * index_count / 3);
    unsigned int vertices_i = 0,
            normal_index_i = 0,
            texture_vertices_i = 0,
            normal_vertices_i = 0,
            index_i = 0;

    while(1) {
        c = buffer[i];
        if( c == 0 ) break;

        if( c == '\n' ) {
            i++;
            continue;
        }

        if( c == '#' ) {
            while(buffer[i] != '\n') i++;
            i++;
            continue;
        }

        if( utils.match_pairs(buffer, i, {'m', 't', 'l', 'l', 'i', 'b', ' '})) {
            i += 7;

            unsigned int k = i;
            while(buffer[i] != '\n' && buffer[i] != ' ') i++;

            mtl = (char*) malloc(i - k + 1);
            memcpy(mtl, (buffer  +k), (i - k));
            mtl[i - k] = 0;
            continue;
        }

        if( c == 'o' && buffer[i+1] == ' ') {
            i += 2;

            unsigned int k = i;
            while(buffer[i] != '\n' && buffer[i] != ' ') i++;

            oCount++;
            o = (char**) realloc(o, sizeof(char*) * oCount);
            o[oCount - 1] = (char*) malloc(i - k + 1);
            memcpy(o[oCount - 1], (buffer + k), (i - k));
            o[oCount - 1][i - k] = 0;
            continue;
        }

        if( c == 'v' ) {
            if( buffer[i+1] == ' ' ) {
                i += 2;

                for( int j = 0; j < 3; j++ ) {
                    unsigned int k = i;
                    while(buffer[i] != '\n' && buffer[i] != ' ') i++;

                    buffer[i] = 0;
                    vertices[vertices_i++] = atof(buffer+ k);
                    buffer[i] = ' ';
                    i++;
                }

                continue;
            }

            if( buffer[i+1] == 'n' && buffer[i+2] == ' ') {
                i += 3;

                for( int j = 0; j < 3; j++ ) {
                    unsigned int k = i;
                    while(buffer[i] != '\n' && buffer[i] != ' ') i++;

                    buffer[i] = 0;
                    normal_vertices[normal_vertices_i++] = atof(buffer + k);

                    buffer[i] = ' ';
                    i++;
                }

                continue;
            }

            if( buffer[i+1] == 't' && buffer[i+2] == ' ' ) {
                i += 3;

                for( int j = 0; j < 2; j++ ) {
                    unsigned int k = i;
                    while(buffer[i] != '\n' && buffer[i] != ' ') i++;

                    buffer[i] = 0;
                    texture_vertices[texture_vertices_i++] = atof(buffer + k);

                    buffer[i] = ' ';
                    i++;
                }

                continue;
            }
        }

        if( c == 'f' && buffer[i+1] == ' ' ) {
            i += 2;

            unsigned int k,
                         indexValues = 0,
                         first[3],
                         prev[3],
                         value[3];

            while(1) {

                for( int j = 0; j < 3; j++ ) {
                    k = i;
                    while(buffer[i] != '/' && buffer[i] != ' ' && buffer[i] != '\n' && buffer[i] != 0) i++;

                    char b = buffer[i];
                    buffer[i] = 0;
                    value[j] = atoi(buffer + k)-1;
                    buffer[i] = b;
                    i++;
                }

                switch(indexValues) {
                    case 0: {
                        first[0] = value[0];
                        first[1] = value[1];
                        first[2] = value[2];
                    } break;
                    case 1: {
                        prev[0] = value[0];
                        prev[1] = value[1];
                        prev[2] = value[2];
                    } break;
                    default: {
                        vertices_index[index_i] = first[0];
                        vertices_index[index_i + 1] = prev[0];
                        vertices_index[index_i + 2] = value[0];

                        texture_vertices_index[index_i] = first[1];
                        texture_vertices_index[index_i + 1] = prev[1];
                        texture_vertices_index[index_i + 2] = value[1];

                        normal_vertices_index[normal_index_i++] = first[2];

                        index_i += 3;

                        prev[0] = value[0];
                        prev[1] = value[1];
                        prev[2] = value[2];
                    } break;
                }

                indexValues++;
                if( buffer[i-1] == '\n' || buffer[i-1] == 0 ) {
                    i--;
                    break;
                }
            }

            continue;
        }

        if( c == 's' && buffer[i + 1] == ' ' ) {
            i += 2;
            continue;
        }

        i++;
    }
}
