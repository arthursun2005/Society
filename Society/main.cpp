//
//  main.cpp
//  Society
//
//  Created by Arthur Sun on 9/30/19.
//  Copyright © 2019 Arthur Sun. All rights reserved.
//

#include "society.h"
#include "utils.h"

#include <iomanip>

#define scroll_scale 0.001

#define drag_scale 0.9

#define scl_limit 400.0f

#define text_texture_width 16

#define text_texture_height 16

society society;

int roundness = 32;

float dt = 0.033f;
int rep1 = 1, rep2 = 8, rep3 = 256;
int rep = rep1;
float acceleration = 20.0f;

GLFWwindow *window;

double mouseX = 0.0, mouseY = 0.0;
double pmouseX = mouseX, pmouseY = mouseY;
double dmouseX = mouseX - pmouseX, dmouseY = mouseY - pmouseY;
int width = 1280;
int height = 840;

int side_width = 750;
int screen_width = 2 * width - side_width;
int screen_height = 2 * height;

int character_width, character_height;

int lines;

float text_ratio = 0.2f;

frame frame;

bool paused = false;

float current_time = 0.0f;

GLuint vaos[3];
GLuint vbos[5];

gl_program programs[4];

gl_texture2d characters;

obj* d_obj = nullptr;
obj* obj;

vec2 mouse() {
    float k = 1.0f / frame.scl;
    vec2 mouse((mouseX * 4.0f - screen_width) * k, (mouseY * 4.0f - screen_height) * -k);
    return mouse - frame.offset;
}

std::string random_name() {
    size_t size = ne_random(3, 12);
    std::string str;
    str.resize(size);
    for(char& q : str) {
        q = ne_random('a', 'z');
    }
    return str;
}

struct obj* mouse_search() {
    struct obj* g = nullptr;
    if(mouseX > 0.0 && mouseY > 0.0 && 2.0 * mouseX < screen_width && 2.0 * mouseY < screen_height) {
        vec2 m = mouse();
        size_t h1 = m.hash();
        std::vector<proxy*>::iterator p = std::lower_bound(society.P.begin(), society.P.end(), h1 - hash_y_step - hash_x_step, [] (const proxy* a, size_t b) {
            return a->hash < b;
        });
        std::vector<proxy*>::iterator end = society.P.end();
        size_t h2 = h1 + hash_y_step + hash_x_step;
        float b = FLT_MAX;
        while(p != end && (*p)->hash <= h2) {
            vec2 q = (*p)->obj->position - m;
            float d = dot(q, q);
            if(d < (*p)->obj->radius * (*p)->obj->radius && d < b) {
                g = (*p)->obj;
                b = d;
            }
            ++p;
        }
    }
    return g;
}

void generate_text_texture(const char* file) {
    std::ifstream is(file);
    
    is >> character_width >> character_height;
    
    std::vector<float> pixels((text_texture_width * character_width) * (text_texture_height * character_height), 0.0f);
    
    int d;
    char e;
    while(is >> d) {
        int x = d % text_texture_width;
        int y = d / text_texture_width;
        int j = x * character_width + y * text_texture_width * character_width * character_height;
        for(int i = 0; i != character_width * character_height; ++i) {
            is >> e;
            pixels[j + (i % character_width) + text_texture_width * character_width * (character_height - 1 - (i / character_width))] = (e == '0' ? 0.0f : 1.0f);
        }
    }
    
    characters.initialize(GL_LINEAR);
    characters.image(GL_R32F, GL_RED, text_texture_width * character_width, text_texture_height * character_height, GL_FLOAT, pixels.data());
    
    characters.bind();
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
    
    glBindVertexArray(vaos[2]);
    
    glBindBuffer(GL_ARRAY_BUFFER, vbos[4]);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindVertexArray(0);
    
    generate_text_texture("characters");
    
    programs[0].initialize("common.glsl", "shape.vs", "color.fs");
    programs[1].initialize("common.glsl", "pass.vs", "text.fs");
    programs[2].initialize("common.glsl", "pass.vs", "fill.fs");
    programs[3].initialize("common.glsl", "point.vs", "fill.fs");
}

void draw_character(char q, int x, int y, int w, int h) {
    if(x < -w || x > 2 * width + w || y < -h || y > 2 * height + h) return;
    
    int tx = q % text_texture_width;
    int ty = q / text_texture_width;
    
    programs[1].bind();
    programs[1].uniform1i("T", characters.id);
    programs[1].uniform2f("scl", 1.0f / (float) (text_texture_width * w), 1.0f / (float) (text_texture_height * h));
    programs[1].uniform2f("offset", tx * w - x, ty * h - y);
    
    glEnable(GL_BLEND);
    
    glBindVertexArray(vaos[1]);
    glViewport(x, y, w, h);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisable(GL_BLEND);
}

void draw_string(const std::string& str, int x, int y, int w, int h) {
    int p = x;
    for(char q : str) {
        if(q == '\n') {
            p = x;
            y -= h;
        }else{
            draw_character(q, p, y, w, h);
            p += w;
        }
    }
}

void draw_string(const std::string& str, const vec2& p, int w, int h) {
    int x = (p.x + frame.offset.x) * frame.scl * 0.5f + 0.5f * screen_width;
    int y = (p.y + frame.offset.y) * frame.scl * 0.5f + 0.5f * screen_height;
    
    for(char q : str) {
        draw_character(q, x, y, w, h);
        x += w;
    }
}

void destory() {
    glDeleteVertexArrays(sizeof(vaos)/sizeof(*vaos), vaos);
    glDeleteBuffers(sizeof(vbos)/sizeof(*vbos), vbos);
    
    destory_textures({&characters});
    destory_programs(programs, programs + sizeof(programs)/sizeof(*programs));
}

void update() {
    if(!paused) {
        for(int n = 0; n != rep; ++n)
            society.step(dt);
        
        current_time += rep * dt;
    }
    
    obj = mouse_search();
    if(obj == nullptr) obj = d_obj;
}

void load() {
    std::vector<float> buf;
    
    for(creature* q : society.C) {
        buf.push_back(q->position.x);
        buf.push_back(q->position.y);
        buf.push_back(q->radius);
    }
    
    for(item* q : society.I) {
        buf.push_back(q->position.x);
        buf.push_back(q->position.y);
        buf.push_back(q->radius);
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(decltype(buf)::value_type) * buf.size(), buf.data(), GL_STREAM_DRAW);
    
    buf.clear();
    
    for(creature* q : society.C) {
        buf.push_back(q->color.r);
        buf.push_back(q->color.g);
        buf.push_back(q->color.b);
    }
    
    for(item* q : society.I) {
        buf.push_back(q->color.r);
        buf.push_back(q->color.g);
        buf.push_back(q->color.b);
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, vbos[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(decltype(buf)::value_type) * buf.size(), buf.data(), GL_STREAM_DRAW);
    
    buf.clear();
    
    for(creature* q : society.C) {
        vec2 n = mul(q->dir, sight_left);
        ne_node** inputs = q->brain.inputs();
        for(int i = 0; i != sight_lines; ++i) {
            buf.push_back(q->position.x);
            buf.push_back(q->position.y);
            
            float s = inputs[1 + 4 * i]->value;
            
            buf.push_back(q->position.x + n.x * s);
            buf.push_back(q->position.y + n.y * s);
            n = mul(n, sight_step);
        }
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, vbos[4]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(decltype(buf)::value_type) * buf.size(), buf.data(), GL_STREAM_DRAW);
    
    lines = (int)(buf.size() / 2);
}

void render() {
    load();
    
    programs[0].bind();
    programs[0].uniform2f("scl", frame.scl/(float)screen_width, frame.scl/screen_height);
    programs[0].uniform2f("offset", frame.offset.x, frame.offset.y);
    
    glBindVertexArray(vaos[0]);
    glViewport(0, 0, screen_width, screen_height);
    glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, roundness, (int)(society.C.size() + society.I.size()));
    
    programs[3].bind();
    programs[3].uniform2f("scl", frame.scl/(float)screen_width, frame.scl/screen_height);
    programs[3].uniform2f("offset", frame.offset.x, frame.offset.y);
    programs[3].uniform3f("color", 0.0f, 1.0f, 1.0f);
    
    glBindVertexArray(vaos[2]);
    glViewport(0, 0, screen_width, screen_height);
    glDrawArrays(GL_LINES, 0, lines);;
    
    if(frame.scl > scl_limit * 0.5f) {
        float tw;
        
        for(creature* q : society.C) {
            tw = q->radius * text_ratio;
            draw_string(q->name, q->position + vec2(-(float)q->name.size() * tw, q->radius + tw), tw * frame.scl, tw * frame.scl);
        }
        
        for(item* q : society.I) {
            tw = q->radius * text_ratio;
            draw_string(q->name, q->position + vec2(-(float)q->name.size() * tw, q->radius + tw), tw * frame.scl, tw * frame.scl);
        }
    }
    
    programs[2].bind();
    programs[2].uniform3f("color", 0.0f, 0.0f, 0.0f);
    
    glBindVertexArray(vaos[1]);
    glViewport(screen_width, 0, side_width, screen_height);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    int sz = 28;
    std::stringstream ss;
    if(obj == nullptr) {
        ss << "society\n";
        ss << "population: " << society.C.size() << '\n';
        ss << "objects: " << society.I.size() << '\n';
        ss << "rep: " << rep << '\n';
        ss << std::fixed << std::setprecision(2);
        ss << "current time: " << current_time << '\n';
    }else{
        ss << "name: " << obj->name << '\n';
        ss << "radius: " << obj->radius << '\n';
        ss << "density: " << obj->density << '\n';
    }
    draw_string(ss.str(), screen_width, screen_height - sz, sz, sz);
}

void mouseCallback(GLFWwindow* window, int button, int action, int mods) {
    if(button == GLFW_MOUSE_BUTTON_LEFT) {
        d_obj = mouse_search();
    }
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if(action == GLFW_PRESS) {
        if(key == GLFW_KEY_SPACE) {
            paused = !paused;
        }
        
        if(key == GLFW_KEY_C) {
            int q = 10;
            for(int i = 1 - q; i != q; ++i) {
                for(int j = 1 - q; j != q; ++j) {
                    creature* q = new creature();
                    q->position = mouse() + vec2(i * 3.0f, j * 3.0f);
                    q->radius = 0.75f;
                    q->density = 1.0f;
                    q->velocity = vec2(0.0f, 0.0f);
                    q->name = random_name();
                    q->dir = vec2(1.0f, 0.0f);
                    society.add(q);
                }
            }
        }
        
        if(key == GLFW_KEY_R) {
            item* q = new item();
            q->position = mouse();
            q->radius = 1.0f;
            q->density = 15.0f;
            q->velocity = vec2(0.0f, 0.0f);
            q->color = color(0.7f, 0.7f, 0.7f);
            q->name = "rock";
            society.add(q);
        }
    }
    
    if(action == GLFW_RELEASE) {
        if(key == GLFW_KEY_1) {
            rep = rep1;
        }else if(key == GLFW_KEY_2) {
            rep = rep2;
        }else if(key == GLFW_KEY_3) {
            rep = rep3;
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
    
    screen_width = 2 * width - side_width;
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
    
    glfwSwapInterval(2);
    
    initialize();
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

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
