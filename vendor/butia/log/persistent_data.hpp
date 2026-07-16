#pragma once

struct PersistentDataReturn {
    enum Type { Integer, String };
    union Value { int number; char *string; };
    Type kind;
    Value value;
};

class PersistentData {
public:
    char *path;

    PersistentDataReturn *get();
    PersistentDataReturn *get(char *file_path);
};
