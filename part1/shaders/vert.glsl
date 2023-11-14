#version 410 core
// From Vertex Buffer Object (VBO)
// The only thing that can come 'in', that is
// what our shader reads, the first part of the
// graphics pipeline.
layout(location=0) in vec3 position;
layout(location=1) in vec3 vertexNormals;
layout(location=2) in vec2 textureCoords; // Texture coordinates
layout(location=3) in vec3 tangents; 
layout(location=4) in vec3 bitangents;

// Uniform variables
uniform mat4 u_ModelMatrix;
uniform mat4 u_ViewMatrix;
uniform mat4 u_Projection; // We'll use a perspective projection

uniform vec3 u_ViewDirection; // camera view direction

// Uniform Light Variables
uniform vec3 u_LightPos;

// Pass vertex colors into the fragment shader
out vec3 v_vertexNormals;
out vec3 v_worldSpaceFragment;
out vec2 v_textureCoords; // Pass texture coordinates to the fragment shader
out vec3 TangentLightPos;
out vec3 TangentViewPos;
out vec3 TangentFragPos;

mat3 calculateTBN() {
    vec3 T = normalize(vec3(u_ModelMatrix * vec4(tangents, 0.0)));
    vec3 B = normalize(vec3(u_ModelMatrix * vec4(bitangents, 0.0)));
    vec3 N = normalize(vec3(u_ModelMatrix * vec4(vertexNormals, 0.0)));
    return mat3(T, B, N);
}

struct Light{
    vec3 lightColor;
    vec3 lightPos;
    float ambientIntensity;
    float specularStrength;
};

uniform Light u_Light[1];

void main()
{
  // Send to fragment shader the vertex colors
  //v_vertexColors = vertexColors;

  // Update the normals based on modelMatrix as we are rotating the model
  //v_vertexNormals= normalize(mat3(transpose(inverse(u_ModelMatrix))) * vertexNormals);
  v_vertexNormals = vertexNormals;

  // Pass texture coordinates to the fragment shader
  v_textureCoords = textureCoords;

  // Calculate in world space the position of the vertex
  v_worldSpaceFragment = vec3(u_ModelMatrix * vec4(position, 1.0f));

  // calculate TBN
  mat3 TBN = calculateTBN();
  mat3 transTBN = transpose(TBN);
  TangentLightPos = transTBN * (u_Light[0].lightPos);
  TangentViewPos = transTBN * u_ViewDirection;
  TangentFragPos = transTBN * v_worldSpaceFragment;

  // Compute the MVP matrix
  gl_Position = u_Projection * u_ViewMatrix * u_ModelMatrix * vec4(position,1.0f);

}


