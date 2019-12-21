#include <readShader.hpp>
#include <iostream>

const GLchar *readShader(const char *fn) {
    FILE *infile = fopen(fn, "rb");
    if(!infile) {
        std::cerr << "Unable to open file:" << fn <<"; errno:" << errno <<std::endl;
        return NULL;
    }
    fseek(infile, 0, SEEK_END);
    int len = ftell(infile);
    fseek(infile, 0, SEEK_SET);

    GLchar *source = new GLchar[len + 1];
    fread(source, 1, len, infile);
    source[len] = 0;

    return const_cast<const GLchar *>(source);
}
