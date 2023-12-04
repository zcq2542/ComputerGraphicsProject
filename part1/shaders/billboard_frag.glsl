#version 410 core

in vec2 texCoord;
in vec3 fragNormal;
in vec3 fragPos;
out vec4 fragColor;

uniform sampler2D textureSampler;
uniform vec3 u_ViewDirection;
uniform vec3 u_EyePosition;
uniform vec3 u_HeadLightCol;
uniform float u_HeadLightScope;
uniform int u_HeadLightOn;
uniform float u_HeadLightStrength;

float calculateAngle(vec3 A, vec3 B) {
    float dotProduct = dot(normalize(A), normalize(B));
    return acos(clamp(dotProduct, -1.0, 1.0)); // Clamping for numerical stability
}

vec4 HeadLight(){
    vec4 headLight = vec4(0,0,0,1);
    if(u_HeadLightOn != 0){
        vec3 headLightDirection;
        vec3 ambient;
        vec3 diffuse = vec3(0.0f, 0.0f, 0.0f);
        vec3 specular;
        float headLightAmbientIntensity = 0.0f;
        float specularStrength = 0.8f;

        float constant = 1.0f;    // Constant attenuation
        float linear = 0.01f;     //0.09 Linear attenuation
        float quadratic = 0.032f;  //0.032; // Quadratic attenuation

        headLightDirection = normalize(u_EyePosition - fragPos);
        vec3 colorDiffuse = texture(textureSampler, texCoord).rgb;

        // Ambient lighting
        ambient = headLightAmbientIntensity * u_HeadLightCol * colorDiffuse;
        
        float angle = calculateAngle(-headLightDirection, u_ViewDirection);
        //angle = 0;
        //if(angle < u_HeadLightScope){
        //if(angle < 0.4){
            float headLightStren = -1/(u_HeadLightScope * u_HeadLightScope) * (angle*angle) + 1;
            headLightStren = 1.0f;
            headLightStren *= u_HeadLightStrength;
            // Calculate distance from light source to fragment
            float distance = length(u_EyePosition - fragPos);

            // Calculate attenuation (decay) based on distance
            float attenuation = 1.0f / (constant + linear * distance + quadratic * (distance * distance));
            attenuation = 0.5;
            // diffuse light
            float diff = 1.0;
            //float diff =  max(0.0, dot(headLightDirection, fragNormal));
            diffuse = attenuation * headLightStren * u_HeadLightCol * (diff) * colorDiffuse ;
            //diffuse = colorDiffuse;

            // specular light
            vec3 reflectionDirection = reflect(headLightDirection, fragNormal);
            float spec = pow(max(0.0, dot(u_ViewDirection, reflectionDirection)), 32);
            specular = attenuation * headLightStren *  specularStrength * u_HeadLightCol * (spec) * colorDiffuse;

            // Send fragment to output with specular
            //headLight = vec4(ambient + diffuse + specular, 1.0f);
            headLight = vec4(diffuse, 1.0f);
       // }
    }
    return headLight;
}

void main()
{
    //fragColor = vec4(1,1,1,1);
    //fragColor = vec4(fragPos,1);
    fragColor = HeadLight();
    //fragColor = vec4(u_HeadLightCol * texture(textureSampler, texCoord).rgb, 1);
    //fragColor = vec4(texture(textureSampler, texCoord).rgb, 1);
   /* 
    if (fragColor.r <= 0.1 && fragColor.g <= 0.1 && fragColor.b <= 0.1) {
        discard;
    }*/
    /*
    if (fragColor.r <= 0.05 && fragColor.g <= 0.05 && fragColor.b <= 0.05) {
        discard;
    }
    */
}

