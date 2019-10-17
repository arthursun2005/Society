layout (location = 0) in vec3 position;
layout (location = 1) in vec3 colors;
layout (location = 2) in vec2 shape;

uniform vec2 scl;
uniform vec2 offset;

out vec3 color;

void main() {
    color = colors;
    gl_Position = vec4((position.xy + position.z * shape + offset) * scl, 0.0f, 1.0f);
}

