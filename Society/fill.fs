layout (location = 0) out vec4 A;

in vec3 color;

void main() {
    A = vec4(color, 1.0f);
}

