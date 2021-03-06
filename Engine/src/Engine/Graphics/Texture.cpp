/**
* @Author   Guillaume Labey
*/

#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <stb_image/stb_image.h>

#include <Engine/Utils/Exception.hpp>
#include <Engine/Debug/Logger.hpp>

#include <Engine/Graphics/Texture.hpp>

Texture::Texture(): _comp(0)
{
    glGenTextures(1, &_texture);
}

Texture::Texture(int width, int height): _width(width), _height(height), _comp(0)
{
    glGenTextures(1, &_texture);
}

Texture::~Texture()
{
    glDeleteTextures(1, &_texture);
}

std::unique_ptr<Texture>    Texture::create(GLsizei width,
                                GLsizei height, GLint internalFormat,
                                GLint format,
                                GLenum type,
                                GLint minFilter,
                                GLint maxFilter,
                                GLint wrapS,
                                GLint wrapT)
{
    std::unique_ptr<Texture> texture = std::make_unique<Texture>(width, height);

    texture->bind();
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, NULL);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, maxFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
    texture->unBind();

    return (std::move(texture));
}

bool    Texture::loadFromFile(const std::string &fileName)
{
    LOG_INFO("Loading texture \"%s\"", fileName.c_str());

    // Load image data and force  components number
    unsigned char* data = stbi_load(fileName.c_str(), &_width, &_height, &_comp, 4);

    if (data == nullptr)
    {
        EXCEPT(FileNotFoundException, "Failed to load texture \"%s\"", fileName.c_str());
    }

    load(_width, _height, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, data);
    STBI_FREE(data);

    return (true);
}

void    Texture::load(GLsizei width,
                        GLsizei height,
                        GLint internalFormat,
                        GLint format,
                        GLenum type,
                        unsigned char* data,
                        GLint minFilter,
                        GLint maxFilter,
                        GLint wrapS,
                        GLint wrapT)
{
    _width = width;
    _height = height;
    _data = data;
    bind();

    // Define texture filter and wrap modes
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, maxFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);

    // Copy image data into texture object
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, _width, _height, 0, format, type, _data);
    // Generate image mipmap levels
    glGenerateMipmap(GL_TEXTURE_2D);

    // Unuse texture
    unBind();

    _data = nullptr;
}

void    Texture::bind(GLenum unit) const
{
    glActiveTexture(unit);
    glBindTexture(GL_TEXTURE_2D, _texture);
}

void    Texture::unBind() const
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

unsigned int    Texture::getWidth() const
{
    return _width;
}

unsigned int    Texture::getHeight() const
{
    return _height;
}

unsigned char*  Texture::getData() const
{
    return _data;
}

int     Texture::getComponentsNumber() const
{
    return _comp;
}

GLuint  Texture::getNative()
{
    return (_texture);
}
