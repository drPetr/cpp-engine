#version 330 core

in vec4 colorPos;

out vec4 fragColor;

void main() {
    fragColor = colorPos;
}