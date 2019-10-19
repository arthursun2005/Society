layout (location = 0) in vec2 position;

uniform vec2 scl;
uniform vec2 offset;

void main() {
    gl_Position = vec4((position + offset) * scl, 0.0f, 1.0f);
}
