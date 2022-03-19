#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 texCoord;

uniform mat4 gWorld;

out vec2 texCoord0;
out vec4 colorPos; 

void main() {
    gl_Position = gWorld * vec4( pos, 1.0 );
    texCoord0 = texCoord;
    colorPos = vec4( clamp( pos, 0.0, 1.0), 1.0 );
}
