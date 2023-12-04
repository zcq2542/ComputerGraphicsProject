#version 410 core

layout (location = 0) in vec3 instancePos;  // Instance position offset

uniform mat4 u_ViewMatrix;
uniform mat4 u_Projection;

void main()
{
    //gl_Position = u_Projection * u_ViewMatrix * vec4( instancePos, 1.0);
    gl_Position = vec4( instancePos, 1.0);
}

