#include "engine_pch.h"
#include "Framebuffer.h"

#include "glad/glad.h"

#include "logger/Logger.h"

Framebuffer::Framebuffer(int initWidth, int initHeight, FramebufferAttachment fbAttachment, RenderbufferType rbType)
{
    currentWidth  = initWidth;
    currentHeight = initHeight;
    attachmentFB  = fbAttachment;
    typeRB        = rbType;
    
    glGenFramebuffers(1, &fbo);
    Bind();
    
    // Color texture attachment
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, initWidth, initHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    // Render buffer object attachment for depth/stencil
    // Use a texture instead if we need to sample, since reading is suboptimal in Renderbuffers in favour of performance
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, rbType, initWidth, initHeight);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    
    //unsigned int texDepthStencil;
    //glGenTextures(1, &texDepthStencil);
    //glBindTexture(GL_TEXTURE_2D, texDepthStencil);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, windowData.windowW, windowData.windowH, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //glBindTexture(GL_TEXTURE_2D, 0);
    //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, texDepthStencil, 0);
    
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, fbAttachment, GL_RENDERBUFFER, rbo);
    
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        Logger::Log(LogCategory::ERROR, "Framebuffer is not complete", "Framebuffer::Framebuffer");
    }
    
    Unbind();
}

Framebuffer::~Framebuffer()
{
    glDeleteFramebuffers(1, &fbo);
}

void Framebuffer::Bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}

void Framebuffer::Unbind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::Resize(int newWidth, int newHeight)
{
    if (currentWidth == newWidth && currentHeight == newHeight)
    {
        return;
    }
    
    currentWidth  = newWidth;
    currentHeight = newHeight;
    
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, newWidth, newHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0);

    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, newWidth, newHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
}
