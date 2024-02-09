﻿#include "engine_pch.h"
#include "Texture.h"

#include "glad/glad.h"

#define STB_IMAGE_IMPLEMENTATION 
#include "stb_image.h"

#include "logger/Logger.h"
#include "utils/PathBuilder.h"

Texture::Texture(const std::string& texPath, TextureType texType, TextureUnit texUnit)
{
    path = texPath;
    type = texType;
    unit = texUnit;

    InitializeTexture(PathBuilder::GetPath(texPath).c_str());
}

void Texture::Bind() const
{
    glActiveTexture(GL_TEXTURE0 + unit);
    
    switch(type)
    {
    case TextureType::DIFFUSE:
        glBindTexture(GL_TEXTURE_2D, textureID);
        break;
    case TextureType::CUBEMAP:
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
        break;
    case TextureType::HDR:
        glBindTexture(GL_TEXTURE_2D, textureID);
        break;
    }
}

void Texture::InitializeTexture(const char* texPath)
{
    switch(type)
    {
    case TextureType::DIFFUSE:
        InitializeTextureDiffuse(texPath);
        break;
    case TextureType::CUBEMAP:
        InitializeTextureCubemap(texPath);
        break;
    case TextureType::HDR:
        InitializeTextureHDR(texPath);
        break;
    }
}

void Texture::InitializeTextureDiffuse(const char* texPath)
{
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_set_flip_vertically_on_load(true);
    
    int width, height, nChannels;
    unsigned char *data = stbi_load(texPath, &width, &height, &nChannels, 0);
    if (!data)
    {
        Logger::Log(LogCategory::ERROR, "Unable to parse texture", "TextureDiffuse::InitializeTexture");
        return;
    }

    switch (nChannels)
    {
    case 1:
        format = TextureFormat::R;
        break;
    case 3:
        format = TextureFormat::RGB;
        break;
    case 4:
        format = TextureFormat::RGBA;
        break;
    default:
        Logger::Log(LogCategory::ERROR, "Unknown number of channels while trying to parse texture", "TextureDiffuse::InitializeTexture");
        return;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(format), width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
}

void Texture::InitializeTextureCubemap(const char* texturePath)
{
    int width;
    int height;
    int nChannels;

    const std::string faces[6] = {
        "right.jpg",
        "left.jpg",
        "top.jpg",
        "bottom.jpg",
        "front.jpg",
        "back.jpg"
    };

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    stbi_set_flip_vertically_on_load(false);

    for (unsigned int i = 0; i < 6; i++)
    {
        std::string fullPath = texturePath;
        fullPath += faces[i];
            
        unsigned char *data = stbi_load(fullPath.c_str(), &width, &height, &nChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        }
        else
        {
            Logger::Log(LogCategory::ERROR, "Cubemap tex failed to load at path: " + fullPath, "TextureCubemap::InitializeTexture");
        }
        
        stbi_image_free(data);
    }
    
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void Texture::InitializeTextureHDR(const char* texturePath)
{
    int width;
    int height;
    int nChannels;

    stbi_set_flip_vertically_on_load(true);
    
    float *data = stbi_loadf(texturePath, &width, &height, &nChannels, 0);
    if (!data)
    {
        const std::string errorMsg = "Unable to read HDR texture file from path: ";
        Logger::Log(LogCategory::ERROR, errorMsg + texturePath, "Texture::InitializeTextureHDR");

        return;
    }
    
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
}
