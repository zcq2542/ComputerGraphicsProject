#version 410 core

in vec3 v_vertexNormals;
in vec3 v_worldSpaceFragment;
in vec2 v_textureCoords;

in vec3 TangentLightPos;
in vec3 TangentViewPos;
in vec3 TangentFragPos;

out vec4 color;

struct Light{
    vec3 lightColor;
    vec3 lightPos;
    float ambientIntensity;
    float specularStrength;
};

struct Material{
	sampler2D diffuseTexture;
	sampler2D normalTexture;
	sampler2D specularTexture;

	float shininess; 	// Material Shininess
	vec3 ka; 			// Ambient color
	vec3 kd; 			// Diffuse color
	vec3 ks; 			// Specular color
};

uniform Light u_Light[1];
uniform Material u_Material;
uniform vec3 u_ViewDirection;
uniform bool u_HasSpecularTexture;

void main()
{
	
	// Normalize every attribute incoming from vertex shader
	// because they are interpolated values
    vec3 normals = normalize(v_vertexNormals); 
	vec3 lightDirection;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	
	// Store the texture coordinates
	// Compute the final lighting, Combine ambient, diffuse, and specular
	// Assume all the objects have normal and diffuse maps
	vec3 colorDiffuse = texture(u_Material.diffuseTexture, v_textureCoords).rgb;
	vec3 normal = texture(u_Material.normalTexture, v_textureCoords).rgb;
	vec3 normalFromMap = normalize(normal * 2.0 - 1.0);
	lightDirection = normalize(TangentLightPos - TangentFragPos);

	// Ambient lighting
	ambient = u_Light[0].ambientIntensity * u_Light[0].lightColor * u_Material.ka * colorDiffuse;

	// Diffuse lighting
	float diff =  max(0.0, dot(lightDirection, normalFromMap));
	diffuse = u_Light[0].lightColor * (diff * u_Material.kd) * colorDiffuse;

	// Send fragment to output with only ambient and diffuse
	color = vec4(ambient + diffuse, 1.0f);
	
	if (u_HasSpecularTexture) {
		//Specular lighting
		vec3 colorSpecular = texture(u_Material.specularTexture, v_textureCoords).rgb;
		vec3 reflectionDirection = reflect(-lightDirection, normalFromMap);
		float spec = pow(max(0.0, dot(-u_ViewDirection, reflectionDirection)), u_Material.shininess);
		specular = u_Light[0].specularStrength * u_Light[0].lightColor * (spec * u_Material.ks) * colorSpecular;
		//specular = u_Light[0].specularStrength * u_Light[0].lightColor * (spec) * colorSpecular;

		// Send fragment to output with specular
		color = vec4(ambient + diffuse + specular, 1.0f);
	}
}