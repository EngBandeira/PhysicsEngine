#include <GL/glew.h>
#include <string.h>

#include "shader.hpp"
#include "utils.hpp"

// #pragma region PARSER
// #define UNIFORM_FLAG_FIRST 0b00000001
// #define UNIFORM_FLAG_SECOND 0b00000010

// #define SIMPLE_OPERATORS_LENGHT 23

// Uniform::~Uniform ()
// {
//     free (name);
//     free (type);
// }

// char SIMPLE_OPERATORS[SIMPLE_OPERATORS_LENGHT]
//     = { '(', ')', '+', '-', '=', '~', '!', '*', '/', '%', '<', '>',
//         '&', '^', '|', '#', '[', ']', '{', '}', ',', '.', ';' };
// #define DOUBLE_OPERATORS_LENGHT 12

// unsigned char DOUBLE_OPERATORS[DOUBLE_OPERATORS_LENGHT][2]
//     = { { 2, 4 },   { 3, 4 },   { 7, 4 },  { 8, 4 },  { 4, 4 },   { 6, 4 },
//         { 10, 10 }, { 11, 11 }, { 10, 4 }, { 11, 4 }, { 12, 12 }, { 14, 14 } };
// //  += -= *= /= == != << >> <= >=  && ||
// char
// isAnOperator (char c[2])
// {
//     for (int i = 0; i < SIMPLE_OPERATORS_LENGHT; i++)
//         {
//             if (c[0] == SIMPLE_OPERATORS[i])
//                 {
//                     for (char i = 0; i < DOUBLE_OPERATORS_LENGHT; i++)
//                         {
//                             if (c[0] == DOUBLE_OPERATORS[i][0]
//                                 && c[1] == DOUBLE_OPERATORS[i][1])
//                                 return 2;
//                         }
//                     return 1;
//                 }
//         }
//     return 0;
// };

// unsigned char flag;
// Uniform unf;
// char tokenBuffer[256];
// unsigned char tokenBufferLenght;
// void
// sendToken (Shader *shader)
// {
//     char test[8] = "uniform";
//     tokenBuffer[tokenBufferLenght] = 0;
//     if (flag != 0)
//         {
//             if (flag & UNIFORM_FLAG_FIRST)
//                 {
//                     unf.type = (char *)malloc (tokenBufferLenght + 1);
//                     memcpy (unf.type, tokenBuffer, tokenBufferLenght + 1);
//                     flag = UNIFORM_FLAG_SECOND;
//                 }
//             else if (flag & UNIFORM_FLAG_SECOND)
//                 {
//                     unf.name = (char *)malloc (tokenBufferLenght + 1);
//                     memcpy (unf.name, tokenBuffer, tokenBufferLenght + 1);
//                     shader->uniforms.push_back (unf);
//                     flag = 0;
//                 }
//         }
//     switch (tokenBufferLenght)
//         {
//         case 7:
//             if (strcmp (tokenBuffer, test) == 0)
//                 flag = UNIFORM_FLAG_FIRST;
//             break;

//         default:
//             break;
//         }
//     // std::cout <<tokenBuffer<<"\n";
//     tokenBufferLenght = 0;
// }

// void
// parseShaders (char *file, unsigned int fileLenght, Shader *shader)
// {
//     char c, isOp;
//     for (unsigned short i = 0; i < fileLenght; i++)
//         {
//             c = file[i];
//             if (c == '/' && file[i + 1] == '/')
//                 {
//                     while (file[i] != '\n')
//                         {
//                             i++;
//                         }
//                     continue;
//                 }
//             if (c == '\n')
//                 c = ' ';
//             if (c == ' ')
//                 {
//                     if (tokenBufferLenght != 0)
//                         {
//                             sendToken (shader);
//                         }
//                     continue;
//                 }
//             if ((isOp = isAnOperator ((char[2]){ c, file[i + 1] })) != 0)
//                 {
//                     if (tokenBufferLenght != 0)
//                         {
//                             sendToken (shader);
//                         }
//                     if (isOp == 1) // SIMPLE OP
//                         {
//                             tokenBuffer[tokenBufferLenght++] = c;
//                             sendToken (shader);
//                             continue;
//                         }
//                     else
//                         {
//                             tokenBuffer[tokenBufferLenght++] = c;
//                             tokenBuffer[tokenBufferLenght++] = file[i + 1];
//                             sendToken (shader);
//                             continue;
//                         }
//                 }
//             tokenBuffer[tokenBufferLenght++] = c;
//         }
// }
// 
// #pragma endregion

Shader::Shader (char *localPath, GLenum shadersType)
{
    unsigned int fileLenght;
    char *file = readFile (localPath, &fileLenght);
    // parseShaders (file, fileLenght, this);

    shader = glCreateShader (shadersType);
    glShaderSource (shader, 1, &file, NULL);
    glCompileShader (shader);
    free (file);
}
void
Shader::attach (unsigned int program)
{
    glAttachShader (program, shader);
}
Shader::~Shader () { glDeleteShader (shader); }
