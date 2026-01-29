#include <X11/X.h>
#include <glm/ext/quaternion_transform.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <string.h>

#include "mesh.hpp"
#include "common.hpp"

template <typename T>
T *exportvec(unsigned int *count, std::vector<T> vec) {
    *count = vec.size();
    T *rt = (T *) malloc(sizeof(T) * (*count));
    memcpy(rt, vec.data(), sizeof(T) * (*count));
    return rt;
}
std::vector<float> vertices_, texture_vertices_, normal_vertices_;
std::vector<unsigned int> vertices_index_, texture_vertices_index_, normal_vertices_index_;
unsigned int fileLenght;
char *buffer;

void Mesh::sanatize() {
    vertices_.clear();
    texture_vertices_.clear();
    normal_vertices_.clear();
    vertices_index_.clear();
    texture_vertices_index_.clear();
    normal_vertices_index_.clear();
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
    vertices_index = exportvec<unsigned int>(&vertices_index_count, vertices_index_);
    texture_vertices_index = exportvec<unsigned int>(&texture_vertices_index_count, texture_vertices_index_);
    normal_vertices_index = exportvec<unsigned int>(&normal_vertices_count, normal_vertices_index_);

    vertices = exportvec<float>(&vertices_count, vertices_);
    texture_vertices = exportvec<float>(&texture_vertices_count, texture_vertices_);
    normal_vertices = exportvec<float>(&normal_vertices_count, normal_vertices_);
}
// abce0
// 01234
void Mesh::pe() {
    unsigned int i = 0;
    while(i < fileLenght) {
        if(buffer[i] == 'v' && buffer[i+1] == ' ') {
            i += 2;

            for( int j = 0; j < 3; j++ ) {
                unsigned int k = i;
                while(buffer[i] != '\n' && buffer[i] != ' ' && buffer[i] != 0) i++;

                buffer[i] = 0;
                vertices_.push_back((float) atof(buffer+ k));
                buffer[i] = ' ';
                i++;
            }

            continue;
        }
        i++;
    }
    for(unsigned int i = 0; i < vertices_.size()/3; i++){
        vertices_index_.push_back(i);
    }
}

void Mesh::obj(){
    char c;
    unsigned int i = 0;
    while(1) {
        c = buffer[i];
        if( c == '\0' ) break;

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
                    vertices_.push_back((float) atof(buffer+ k));
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
                    normal_vertices_.push_back((float) atof(buffer+ k));
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
                    texture_vertices_.push_back((float)atof(buffer+ k));
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
                        vertices_index_.push_back(first[0]);
                        vertices_index_.push_back(prev[0]);
                        vertices_index_.push_back(value[0]);

                        texture_vertices_index_.push_back(first[1]);
                        texture_vertices_index_.push_back(prev[1]);
                        texture_vertices_index_.push_back(value[1]);

                        normal_vertices_index_.push_back(first[2]);

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
