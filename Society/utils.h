//
//  utils.h
//  Society
//
//  Created by Arthur Sun on 10/16/19.
//  Copyright © 2019 Arthur Sun. All rights reserved.
//

#ifndef utils_h
#define utils_h

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <fstream>
#include <sstream>
#include <iostream>

#include "common.h"

struct frame {
    double scl;
    vec2 offset;
};

inline GLuint compile_shader(const std::string& str, int type) {
    GLuint shader = glCreateShader(type);
    const char* c_str = str.c_str();
    glShaderSource(shader, 1, &c_str, NULL);
    
    glCompileShader(shader);
    
    int status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    
    if(status == GL_FALSE) {
        int length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        GLchar log[length];
        glGetShaderInfoLog(shader, length, NULL, log);
        std::cout << log << std::endl;
    }
    
    return shader;
}

inline GLuint compile_program(const std::initializer_list<GLuint>& shaders) {
    GLuint program = glCreateProgram();
    
    for(GLuint shader : shaders) {
        glAttachShader(program, shader);
    }
    
    glLinkProgram(program);
    
    int status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if(status == GL_FALSE) {
        int length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
        GLchar log[length];
        glGetProgramInfoLog(program, length, NULL, log);
        std::cout << log << std::endl;
    }
    
    return program;
}

inline void destory_shaders(GLuint program, const std::initializer_list<GLuint>& shaders) {
    for(GLuint shader : shaders) {
        glDetachShader(program, shader);
        glDeleteShader(shader);
    }
}

inline std::string ftos(const char* file) {
    std::ifstream is(file);
    std::stringstream ss;
    ss << is.rdbuf();
    is.close();
    return ss.str();
}

struct gl_program {
    GLuint program;
    
    void initialize(const char* header, const char* shader1) {
        std::string q1 = ftos(header);
        std::string q2 = ftos(shader1);
        GLuint s1 = compile_shader(q1 + q2, GL_COMPUTE_SHADER);
        program = compile_program({s1});
        destory_shaders(program, {s1});
    }
    
    void initialize(const char* header, const char* shader1, const char* shader2) {
        std::string q1 = ftos(header);
        std::string q2 = ftos(shader1);
        std::string q3 = ftos(shader2);
        GLuint s1 = compile_shader(q1 + q2, GL_VERTEX_SHADER);
        GLuint s2 = compile_shader(q1 + q3, GL_FRAGMENT_SHADER);
        program = compile_program({s1, s2});
        destory_shaders(program, {s1, s2});
    }
    
    void initialize(const char* header, const char* shader1, const char* shader2, const char* shader3) {
        std::string q1 = ftos(header);
        std::string q2 = ftos(shader1);
        std::string q3 = ftos(shader2);
        std::string q4 = ftos(shader3);
        GLuint s1 = compile_shader(q1 + q2, GL_VERTEX_SHADER);
        GLuint s2 = compile_shader(q1 + q3, GL_GEOMETRY_SHADER);
        GLuint s3 = compile_shader(q1 + q4, GL_FRAGMENT_SHADER);
        program = compile_program({s1, s2, s3});
        destory_shaders(program, {s1, s2, s3});
    }

    
    void destory() {
        glDeleteProgram(program);
    }
    
    void bind() {
        glUseProgram(program);
    }
    
    void uniform1i(const char* n, int i0) const {
        glUniform1i(glGetUniformLocation(program, n), i0);
    }
    
    void uniform2i(const char* n, int i0, int i1) const {
        glUniform2i(glGetUniformLocation(program, n), i0, i1);
    }
    
    void uniform3i(const char* n, int i0, int i1, int i2) const {
        glUniform3i(glGetUniformLocation(program, n), i0, i1, i2);
    }
    
    void uniform4i(const char* n, int i0, int i1, int i2, int i3) const {
        glUniform4i(glGetUniformLocation(program, n), i0, i1, i2, i3);
    }
    
    void uniform1ui(const char* n, uint i0) const {
        glUniform1ui(glGetUniformLocation(program, n), i0);
    }
    
    void uniform2ui(const char* n, uint i0, uint i1) const {
        glUniform2ui(glGetUniformLocation(program, n), i0, i1);
    }
    
    void uniform3ui(const char* n, uint i0, uint i1, uint i2) const {
        glUniform3ui(glGetUniformLocation(program, n), i0, i1, i2);
    }
    
    void uniform4ui(const char* n, uint i0, uint i1, uint i2, uint i3) const {
        glUniform4ui(glGetUniformLocation(program, n), i0, i1, i2, i3);
    }
    
    void uniform1f(const char* n, float f0) const {
        glUniform1f(glGetUniformLocation(program, n), f0);
    }
    
    void uniform2f(const char* n, float f0, float f1) const {
        glUniform2f(glGetUniformLocation(program, n), f0, f1);
    }
    
    void uniform3f(const char* n, float f0, float f1, float f2) const {
        glUniform3f(glGetUniformLocation(program, n), f0, f1, f2);
    }
    
    void uniform4f(const char* n, float f0, float f1, float f2, float f3) const {
        glUniform4f(glGetUniformLocation(program, n), f0, f1, f2, f3);
    }
    
    void uniform1fv(const char* n, const float* f, int s) const {
        glUniform1fv(glGetUniformLocation(program, n), s, f);
    }
    
    void uniform2fv(const char* n, const float* f, int s) const {
        glUniform2fv(glGetUniformLocation(program, n), s, f);
    }
    
    void uniform3fv(const char* n, const float* f, int s) const {
        glUniform3fv(glGetUniformLocation(program, n), s, f);
    }
    
    void uniform4fv(const char* n, const float* f, int s) const {
        glUniform4fv(glGetUniformLocation(program, n), s, f);
    }
};

inline void destory_programs(const std::initializer_list<gl_program*>& programs) {
    for(gl_program* p : programs)
        p->destory();
}

inline void destory_programs(gl_program* begin, gl_program* end) {
    while(begin != end) {
        begin->destory();
        ++begin;
    }
}

struct gl_texture {
    static GLuint texture_count;
    
    GLuint id;
    GLuint texture;
    GLuint fbo;
    
    gl_texture() {}
    
    gl_texture(const gl_texture& t) = delete;
    
    gl_texture& operator = (const gl_texture& t) = delete;
    
    void initialize(GLenum);
    
    void destory() {
        glDeleteTextures(1, &texture);
        glDeleteFramebuffers(1, &fbo);
    }
    
    void bind();
    
    static void reset_count() {
        texture_count = 0;
    }
    
    void clear(int b) {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glClear(b);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
};

struct gl_texture2d : public gl_texture {
    void initialize(GLenum mode) {
        glGenFramebuffers(1, &fbo);
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    
    void image(GLenum iformat, GLenum format, GLuint x, GLuint y, GLenum type, void* pixels) {
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, iformat, x, y, 0, format, type, pixels);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
        assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    
    void bind() {
        id = texture_count++;
        glActiveTexture(GL_TEXTURE0 + id);
        glBindTexture(GL_TEXTURE_2D, texture);
    }
};

inline void destory_textures(const std::initializer_list<gl_texture*>& textures) {
    for(gl_texture* t : textures)
        t->destory();
}

GLuint gl_texture::texture_count = 0;

#endif /* utils_h */
