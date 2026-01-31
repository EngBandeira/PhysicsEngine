#pragma once

class Script {
    public:
    char *path;
    char *name;
    unsigned int code;
};

class Scripts_Manager {
    public:
    Script *scripts;
    unsigned int scripts_count = 0;
    const char* user_mod_path = "rust/src/user/mod.rs";

    void init();
    void free_data();

    unsigned int script_alredy_exist(unsigned int code);
    unsigned int get_script(char *path);

    void compile();
    void update_user_mod();

    Scripts_Manager() {}
};
// typedef void (*rustcb)();

// struct Vec3 {
//     float x, y, z;
// };

// extern "C" Vec3 get_player_position() {
//     return Vec3{13.0f, 31.0f, 0.0f};
// }

// extern "C" {
//     rustcb *get_rust_callbacks(int amount);

//     // void export_cpp_functions(
//     //     Vec3 (*cpp_get_player_position)()
//     // );
// }
