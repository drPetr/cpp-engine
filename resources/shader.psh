#version 330 core

in vec2 texCoord0;
in vec4 colorPos;

uniform sampler2D gTex;

out vec4 fragColor;

void main() {
    vec4 cl = texture2D( gTex, texCoord0.st );
    if( cl.w < 1.0 ) {
        discard;
    }
    fragColor = cl;
}