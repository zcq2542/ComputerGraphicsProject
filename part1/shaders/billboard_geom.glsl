#version 410 core

layout (points) in;
layout (triangle_strip, max_vertices = 4) out; 

uniform mat4 u_ViewMatrix;
uniform mat4 u_Projection;
uniform vec3 u_EyePosition;

out vec2 texCoord;
out vec3 fragNormal; // Output normal for the fragment shader
out vec3 fragPos;

void main()
{   
    mat4 viewProjectionMatrix = u_Projection * u_ViewMatrix;
    
    vec3 pos = gl_in[0].gl_Position.xyz;
    vec3 toCamera = normalize(u_EyePosition - pos);
    vec3 up = vec3(0.0, 1.0, 0.0);
    vec3 right = cross(toCamera, up);
    vec3 normal = -toCamera;
    vec3 fragPos;
    float halfSize = 0.5; // Half size of the billboard quad

    // Bottom left
    gl_Position = viewProjectionMatrix * vec4(pos - right * halfSize, 1.0);
    texCoord = vec2(0.0, 0.0);
    fragNormal = normal;
    fragPos = pos - right * halfSize;
    EmitVertex();

    // Top left
    gl_Position = viewProjectionMatrix * vec4(pos - right * halfSize + up, 1.0);
    texCoord = vec2(0.0, 1.0);
    fragNormal = normal;
    fragPos = pos - right * halfSize + up;
    EmitVertex();

    // Bottom right
    gl_Position = viewProjectionMatrix * vec4(pos + right * halfSize, 1.0);
    texCoord = vec2(1.0, 0.0);
    fragNormal = normal;
    fragPos = pos + right * halfSize;
    EmitVertex();

    // Top right
    gl_Position = viewProjectionMatrix * vec4(pos + right * halfSize + up, 1.0);
    texCoord = vec2(1.0, 1.0);
    fragNormal = normal;
    fragPos = pos + right * halfSize + up;
    EmitVertex();

    EndPrimitive();
}

