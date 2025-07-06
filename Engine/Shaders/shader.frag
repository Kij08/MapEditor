#version 450

layout(set = 0, binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec3 fragColour;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 surfaceNormal;
layout(location = 3) in float Ka;
layout(location = 4) in float Kd;
layout(location = 5) in float Ks;
layout(location = 6) in vec3 fragPos;


layout(location = 0) out vec4 outColour;

void main() {
    
    //Blinn-Phong
    
    vec3 ambientColour = vec3(44, 0, 46);
    vec3 lightPos = vec3(-600, 0, 700); 
    vec3 lightColour = vec3(245, 193, 244);
    vec3 cameraPosition = vec3(2000, 0 ,0); //Camera position in world space

    vec3 NormalisedSF = normalize(surfaceNormal); //Should transform the normal vec to world space as well, but since Im not applying any non uniform scaling its not a big deal
    vec3 lightDir = normalize(lightPos - fragPos);
    
    float diff = max(dot(NormalisedSF, lightDir), 0.0);
    vec3 diffuseColour = diff * lightColour;

    vec3 viewDirection = normalize(cameraPosition - fragPos);
    vec3 reflectDirection = reflect(-lightDir, NormalisedSF);
    vec3 halfwayDirection = normalize(lightDir + viewDirection);

    float specular = pow(max(dot(NormalisedSF, halfwayDirection), 0.0), 256);
    vec3 specularColour = vec3(255, 255, 255) * specular; //Specular is white


    outColour = texture(texSampler, fragTexCoord);
    //outColour = outColour * vec4(((Ka * ambientColour) + (Kd * diffuseColour) + (Ks * specularColour)), 1.0);
}