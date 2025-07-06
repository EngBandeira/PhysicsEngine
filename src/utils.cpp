#include "utils.hpp"
#include <stdio.h>

char *
readFile (const char *localPath, unsigned int *fileLenght)
{
    FILE *file = fopen (localPath, "r");
    char *buffer;
    if (file == NULL)
        {
            char message[1000];
            sprintf (message, "cannot open file %s", localPath);
            sendError (message);
            exit (1);
        }
    fseek (file, 0L, SEEK_END);
    unsigned int sz = ftell (file) +1;
    fseek (file, 0, SEEK_SET);
    buffer = (char *)malloc (sz * sizeof (char));
    buffer[sz-1] = EOF;
    fread (buffer, sizeof (char), sz, file);
    fclose (file);
    *fileLenght = sz;
    return buffer;
}
void
sendError (const char *str)
{
    fprintf (stderr, "error: %s\n", str);
}


std::ostream &
operator<< (std::ostream &os, vec3 v)
{
    return os << v.x << " , " << v.y << " , " << v.z;
}
std::ostream &
operator<< (std::ostream &os, vec2 v)
{
    return os << v.x << " , " << v.y;
}