#pragma once
#include <glad/glad.h>

namespace vtarga {

unsigned char *load_targa(const char *filename, GLenum &format, int &widht, int &height);

}
