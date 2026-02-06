#include "common.hpp"
#include "scripts_mananger.hpp"
#include "engine.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>

void Scripts_Manager::init() {
    scripts = (Script*)malloc(0);
}
void Scripts_Manager::free_data() {
    free(scripts);
}

unsigned int Scripts_Manager::script_alredy_exist(unsigned int code) {
    for(unsigned int i = 0; i < scripts_count; i++) {
        if(scripts[i].code == code)
            return i;
    }
    return -1;
}
// a/pinto.rs
// 0123456789
// 10 - 2 - 3
unsigned int Scripts_Manager::get_script(char *path) {
    Script script;
    script.code = utils.sha256(path);
    unsigned int i = script_alredy_exist(script.code);
    if(i != (unsigned int)-1) {
        return i;
    }

    if(!access(path, F_OK)) {
        char *c = (char*)malloc(sizeof("cannot open: ") + strlen(path));
        sprintf(c, "cannot open: %s", path);
        logger.sendError(c);
    }

    unsigned int p = strlen(path) + 1;
    script.path = (char*)malloc(p);
    memcpy(script.path, path, p);
    script.path[p - 1] = 0;

    unsigned int init = utils.get_last_c(path, p, '/') + 1;
    script.name = (char*)malloc(p - init - 3);
    memcpy(script.name, path + init, p - init - 4);
    script.name[p - init - 4] = 0;

    scripts = (Script*)realloc(scripts, sizeof(Script) * ++scripts_count);
    scripts[scripts_count - 1] = script;
    return scripts_count - 1;
}


void Scripts_Manager::compile() {
    system("rm -r rust/src/user/*");
    char command[512];
    for(unsigned int i = 0; i < scripts_count; i++) {
        sprintf(command, "cp %s  rust/src/user/", scripts[i].path);
        system(command);
    }

    unsigned int scripts_count = 0, scripts_i = 0;

    for(unsigned int i = 0; i < engine.objects_count; i++) {
        scripts_count += engine.objects[i].scripts_count;
    }
    unsigned int *scripts = (unsigned int*)malloc(sizeof(int) * scripts_count);
    unsigned int *objects_scripts_count = (unsigned int*)malloc(sizeof(int) * engine.objects_count);

    for(unsigned int i = 0; i < engine.objects_count; i++) {
        memcpy(scripts + scripts_i, engine.objects[i].scripts, engine.objects[i].scripts_count);
        scripts_i += engine.objects[i].scripts_count;
        objects_scripts_count[i] = engine.objects[i].scripts_count;
    }


    update_user_mod();
}


void Scripts_Manager::start() {
    // rCallback *callbacks = get_rust_callbacks(scripts_count);
    // callbacks[0]()
}
void Scripts_Manager::update() {
    // rCallback *callbacks = get_rust_callbacks(scripts_count);
    // callbacks[0]()
}

void Scripts_Manager::update_user_mod() {
    FILE *file = fopen(user_mod_path, "wr");

    fwrite("use crate::common;\n", 1, sizeof("use crate::common;\n") - 1, file);

    for(unsigned int i = 0; i < scripts_count; i++) {

        fprintf(file, "mod %s;\n",scripts[i].name);
    }

    fwrite("pub fn get_rust_callbacks(array_ptr: *mut *mut common::Callback) { unsafe {\n", 1,
        sizeof("pub fn get_rust_callbacks(array_ptr: *mut *mut common::Callback) { unsafe {\n") - 1, file);

    for(unsigned int i = 0; i < scripts_count; i++) {
        fprintf(file, "\t*array_ptr.offset(%d) = %s::start as *mut extern \"C\" fn();\n", 2*i, scripts[i].name);
        fprintf(file, "\t*array_ptr.offset(%d) = %s::update as *mut extern \"C\" fn();\n", 2*i+1, scripts[i].name);
    }
    fwrite("}}", 1, sizeof("}}") - 1, file);
    fclose(file);
}
