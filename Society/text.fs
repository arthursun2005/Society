layout (location = 0) out vec4 A;

uniform sampler2D T;
uniform vec2 offset;
uniform vec2 scl;

void main() {
    float q = texture(T, (gl_FragCoord.xy + offset) * scl).x;
    A = vec4(0.0f, q * 2.0f, 0.0f, 1.0f);
}
