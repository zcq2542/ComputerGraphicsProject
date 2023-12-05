#version 410 core

in vec3 v_vertexNormals;
in vec3 v_worldSpaceFragment;
in vec2 v_textureCoords;

in vec3 TangentLightPos;
in vec3 TangentViewPos;
in vec3 TangentFragPos;
in vec3 TangentHeadLightPos;

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
//uniform vec3 u_EyePosition;
uniform vec3 u_HeadLightCol;
uniform float u_HeadLightScope;
uniform bool u_HasSpecularTexture;
uniform int u_HeadLightOn;
uniform float u_HeadLightStrength;

float calculateAngle(vec3 A, vec3 B) {
    float dotProduct = dot(normalize(A), normalize(B));
    return acos(clamp(dotProduct, -1.0, 1.0)); // Clamping for numerical stability
}

vec4 HeadLight(){
    //vec3 u_HeadLightCol = vec3(0.96f, 0.85f, 0.65f);
    vec4 headLight = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    if(u_HeadLightOn != 0){
        vec3 normals = normalize(v_vertexNormals);
        vec3 headLightDirection;
        vec3 ambient;
        vec3 diffuse = vec3(0.0f, 0.0f, 0.0f);
        vec3 specular;
        float headLightAmbientIntensity = 0.0f;
        float specularStrength = 0.8f;
        
        float constant = 1.0;    // Constant attenuation
        float linear = 0.01;     //0.09 Linear attenuation
        float quadratic = 0.032;  //0.032; // Quadratic attenuation


        // Store the texture coordinates
        // Compute the final lighting, Combine ambient, diffuse, and specular
        // Assume all the objects have normal and diffuse maps
        vec3 colorDiffuse = texture(u_Material.diffuseTexture, v_textureCoords).rgb;
        vec3 normal = texture(u_Material.normalTexture, v_textureCoords).rgb;
        vec3 normalFromMap = normalize(normal * 2.0 - 1.0);
        headLightDirection = normalize(TangentHeadLightPos - TangentFragPos);

        // Ambient lighting
        ambient = headLightAmbientIntensity * u_HeadLightCol * u_Material.ka * colorDiffuse;
        float angle = calculateAngle(-headLightDirection, TangentViewPos);
        // Diffuse lighting
        if(angle < u_HeadLightScope){
            float headLightStren = -1/(u_HeadLightScope * u_HeadLightScope) * (angle*angle) + 1;
            headLightStren *= u_HeadLightStrength;
            // Calculate distance from light source to fragment
            float distance = length(TangentHeadLightPos - TangentFragPos);

            // Calculate attenuation (decay) based on distance
            float attenuation = 1.0 / (constant + linear * distance + quadratic * (distance * distance));    
            
            float diff =  max(0.0, dot(headLightDirection, normalFromMap));
            diffuse = attenuation * headLightStren * u_HeadLightCol * (diff * u_Material.kd) * colorDiffuse;
        
            // Send fragment to output with only ambient and diffuse
            headLight = vec4(ambient + diffuse, 1.0f);


            if (u_HasSpecularTexture) {
                //Specular lighting
                vec3 colorSpecular = texture(u_Material.specularTexture, v_textureCoords).rgb;
                vec3 reflectionDirection = reflect(headLightDirection, normalFromMap);
                float spec = pow(max(0.0, dot(TangentViewPos, reflectionDirection)), u_Material.shininess);
                specular = attenuation * headLightStren *  specularStrength * u_HeadLightCol * (spec * u_Material.ks) * colorSpecular;
                //specular = u_Light[0].specularStrength * u_Light[0].lightColor * (spec) * colorSpecular;

                // Send fragment to output with specular
                headLight += vec4(specular, 1.0f);
            }

        }    
    }
    return headLight;
}

void main()
{

    color = HeadLight();

}
