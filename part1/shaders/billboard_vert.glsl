#version 410 core

layout (location = 0) in vec3 instancePos;  // Instance position offset

void main()
{
    gl_Position = vec4( instancePos, 1.0);
}

