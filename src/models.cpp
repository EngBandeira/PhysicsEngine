#include <stdio.h>

#include "models.hpp"

#define BUFFER_LENGHT 64

model::model (const char *localPath)
{
    FILE *file = fopen (localPath, "r");
    if (file == NULL)
        {
            char r[100];
            sprintf (r, "cannot find to load model:%s\n", localPath);
            sendError (r);
            exit (1);
        }
    char c;
    char buffer[BUFFER_LENGHT];
    unsigned char bufferIndex = 0;
    while ((c = fgetc (file)) != EOF)
        {
            if (c == '#' && bufferIndex == 0)
                {
                    while (fgetc (file) != '\n')
                        ;
                    continue;
                }
            if (c == '\n')
                {
                    std::cout << buffer << "  :  ";

                    char command[16];
                    int i;
                    for (i = 0; i < 16; i++)
                        {
                            if (buffer[i] == ' ')
                                break;
                            command[i] = buffer[i];
                        }
                    command[i] = 0;
                    std::cout << command << '\n';

                    bufferIndex = 0;
                    continue;
                }
            if (bufferIndex < BUFFER_LENGHT)
                {
                    buffer[bufferIndex++] = c;
                }
            else
                {
                    sendError ("line too big");
                    exit (1);
                }
        }

    fclose (file);
}
model::~model ()
{
    // free(vertex);
}
