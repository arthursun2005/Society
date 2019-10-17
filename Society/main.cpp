//
//  main.cpp
//  Society
//
//  Created by Arthur Sun on 9/30/19.
//  Copyright © 2019 Arthur Sun. All rights reserved.
//

#include "society.h"
#include "utils.h"

#define scroll_scale 0.001

#define drag_scale 0.9

#define scl_limit 100.0f

society society;

int roundness = 32;

float dt = 0.016f;
int its = 8;
float acceleration = 20.0f;

GLFWwindow *window;

double mouseX = 0.0, mouseY = 0.0;
double pmouseX = mouseX, pmouseY = mouseY;
double dmouseX = mouseX - pmouseX, dmouseY = mouseY - pmouseY;
int width = 1280;
int height = 840;

int screen_width = 2 * width;
int screen_height = 2 * height;

frame frame;

bool paused = false;

GLuint vaos[2];
GLuint vbos[4];

gl_program programs[2];

gl_texture2d characters;

inline vec2 mouse() {
    float k = 2.0f / frame.scl;
    vec2 mouse((mouseX * 2.0f - width) * k, (mouseY * 2.0f - height) * -k);
    return mouse - frame.offset;
}

void initialize() {
    std::vector<vec2> vertices(roundness);
    
    for(int i = 0; i != roundness; ++i) {
        float a = 2.0f * M_PI * i / (float) roundness;
        vertices[i] = vec2(cosf(a), sinf(a));
    }
    
    std::vector<float> quad = {
        -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f
    };
    
    glGenVertexArrays(sizeof(vaos)/sizeof(*vaos), vaos);
    glGenBuffers(sizeof(vbos)/sizeof(*vbos), vbos);
    
    glBindVertexArray(vaos[0]);
    
    glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindBuffer(GL_ARRAY_BUFFER, vbos[1]);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(1);
    
    glBindBuffer(GL_ARRAY_BUFFER, vbos[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(decltype(vertices)::value_type) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(2);
    
    glVertexAttribDivisor(0, 1);
    glVertexAttribDivisor(1, 1);
    glVertexAttribDivisor(2, 0);
    
    glBindVertexArray(vaos[1]);
    
    glBindBuffer(GL_ARRAY_BUFFER, vbos[3]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(decltype(quad)::value_type) * quad.size(), quad.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindVertexArray(0);
    
    programs[0].initialize("common.glsl", "shape.vs", "fill.fs");
    programs[1].initialize("common.glsl", "text.vs", "text.fs");
    
    creature* c = new creature();
    c->position = vec2(0.0f, 0.0f);
    society.add(c);
}

void destory() {
    glDeleteVertexArrays(sizeof(vaos)/sizeof(*vaos), vaos);
    glDeleteBuffers(sizeof(vbos)/sizeof(*vbos), vbos);
    
    destory_textures({&characters});
    destory_programs(programs, programs + sizeof(programs)/sizeof(*programs));
}

void update() {
    society.step(dt);
}

void load() {
    std::vector<float> buf;
    
    for(creature* q : society.C) {
        buf.push_back(q->position.x);
        buf.push_back(q->position.y);
        buf.push_back(0.5f * diameter_limit);
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(decltype(buf)::value_type) * buf.size(), buf.data(), GL_STREAM_DRAW);
    
    buf.clear();
    
    for(creature* q : society.C) {
        buf.push_back(q->color.r);
        buf.push_back(q->color.g);
        buf.push_back(q->color.b);
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, vbos[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(decltype(buf)::value_type) * buf.size(), buf.data(), GL_STREAM_DRAW);
}

void render() {
    load();
    
    programs[0].bind();
    programs[0].uniform2f("scl", frame.scl/(float)screen_width, frame.scl/screen_height);
    programs[0].uniform2f("offset", frame.offset.x, frame.offset.y);
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glBindVertexArray(vaos[0]);
    glViewport(0, 0, screen_width, screen_height);
    glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, roundness, (int)(society.C.size() + society.I.size()));
}

void mouseCallback(GLFWwindow* window, int button, int action, int mods) {
    if(button == GLFW_MOUSE_BUTTON_LEFT) {
    }
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if(action == GLFW_PRESS) {
        if(key == GLFW_KEY_SPACE) {
            paused = !paused;
        }
    }
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    frame.scl *= exp(scroll_scale * yoffset);
    if(frame.scl > scl_limit) frame.scl = scl_limit;
}

void resizeCallback(GLFWwindow* window, int w, int h) {
    width = w;
    height = h;
    
    screen_width = 2 * width;
    screen_height = 2 * height;
}


int main(int argc, const char * argv[]) {
    if(!glfwInit())
        return EXIT_FAILURE;
    
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    
    window = glfwCreateWindow(width, height, "Society", NULL, NULL);
    
    glfwMakeContextCurrent(window);
    
    glewInit();
    
    glfwSetMouseButtonCallback(window, mouseCallback);
    
    GLFWcursor* cursor = glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR);
    
    glfwSetCursor(window, cursor);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetScrollCallback(window, scrollCallback);
    glfwSetWindowSizeCallback(window, resizeCallback);
    
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_ONE, GL_ONE);
    
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    
    mouseX = width * 0.5;
    mouseY = height * 0.5;
    
    frame.scl = scl_limit;
    frame.offset = vec2(0.0f);
    
    glfwSwapInterval(1);
    
    initialize();
    
    do {
        glfwPollEvents();
        
        bool press = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
        
        glfwGetCursorPos(window, &mouseX, &mouseY);
        
        if(press) {
            dmouseX = mouseX - pmouseX;
            dmouseY = mouseY - pmouseY;
        }else{
            dmouseX *= drag_scale;
            dmouseY *= drag_scale;
        }
        
        frame.offset.x += 4.0f * dmouseX / frame.scl;
        frame.offset.y -= 4.0f * dmouseY / frame.scl;
        
        pmouseX = mouseX;
        pmouseY = mouseY;
        
        {
            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            
            if(!paused)
                update();
            
            render();
        }
        
        glfwSwapBuffers(window);
    } while (glfwWindowShouldClose(window) == GL_FALSE && glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS);
    
    destory();
    
    glfwDestroyWindow(window);
    glfwDestroyCursor(cursor);
    
    glfwTerminate();
    
    return 0;
}
