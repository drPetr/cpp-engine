#version 330 core

layout (location = 0) in vec3 pos;
uniform mat4 gWorld;
out vec4 colorPos; /* input to fragment shader */


void main() {
    gl_Position = gWorld * vec4( pos, 1.0 );
    colorPos = vec4( clamp( pos, 0.0, 1.0), 1.0 );
}