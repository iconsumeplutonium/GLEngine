#version 460 core

uniform sampler2D diffuseMap;
uniform vec3 diffuseColor;
uniform bool hasDiffuseMap;

uniform sampler2D specularMap;
uniform vec3 specularColor;
uniform bool hasSpecularMap;

uniform float specularExp;

struct DirectionalLight {
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct PointLight {
	vec4 position;

	vec4 diffuse;
	vec4 ambient;
	vec4 specular;

	float constant;
	float linear;
	float quadratic;
};

struct Spotlight {
	vec4 position;
	vec4 direction;

	vec4 diffuse;
	vec4 ambient;
	vec4 specular;

	float innerCutoff;
	float outerCutoff;
	
	float constant;
	float linear;
	float quadratic;
};


#define MAX_POINT_LIGHTS 10
#define MAX_SPOTLIGHTS 10
layout (std140, binding = 0) uniform PointLights {
	PointLight pointLights[MAX_POINT_LIGHTS];
	Spotlight spotlights[MAX_SPOTLIGHTS];
	int numPointLights;
	int numSpotlights;
};

uniform vec3 camPos;

in vec3 vNormal;
in vec3 fragPos;
in vec2 vUv;

out vec4 FragColor;

// vec3 calculateDirLight(DirectionalLight light, vec3 normal, vec3 viewDir) {
// 	vec3 lightDir = normalize(-light.direction);
// 	vec3 diffuseMapSample = texture(material.diffuse, vUv).xyz;
// 	vec3 specularMapSample = texture(material.specular, vUv).xyz;

// 	vec3 ambient = light.ambient * diffuseMapSample;

// 	float nDotL = max(dot(normal, lightDir), 0.0);
// 	vec3 diffuse = light.diffuse * nDotL * diffuseMapSample;

// 	vec3 reflectDir = reflect(-lightDir, normal);
// 	float spec = max(dot(viewDir, reflectDir), 0.0);
// 	vec3 specular = pow(spec, material.shininess) * specularMapSample * light.specular;

// 	return diffuse + ambient + specular;
// }

vec3 calculatePointLight(PointLight light, vec3 baseDiffuseColor, vec3 baseSpecularColor, vec3 dirToCamera) {
	vec3 ambient = 0.1 * baseDiffuseColor * light.ambient.xyz;

	vec3 dirToLight = normalize(light.position.xyz - fragPos);

	float d = length(light.position.xyz - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear*d + light.quadratic*d*d);


	float nDotL = max(dot(dirToLight, vNormal), 0.0);
	vec3 diffuse = nDotL * baseDiffuseColor * light.diffuse.xyz;

	vec3 halfway = normalize(dirToLight + dirToCamera);
	float specularStrength = max(dot(halfway, vNormal), 0.0f);
	float spec = pow(specularStrength, specularExp);
	vec3 specular = spec * baseSpecularColor * light.specular.xyz;

	return (diffuse + ambient + specular) * attenuation;
}

vec3 calculateSpotlight(Spotlight light, vec3 baseDiffuseColor, vec3 baseSpecularColor, vec3 dirToCamera) {
	vec3 dirToLight = normalize(light.position.xyz - fragPos);

	float d = length(light.position.xyz - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear*d + light.quadratic*d*d);


	float nDotL = max(dot(dirToLight, vNormal), 0.0);
	vec3 diffuse = nDotL * baseDiffuseColor * light.diffuse.xyz;

	vec3 halfway = normalize(dirToLight + dirToCamera);
	float specularStrength = max(dot(halfway, vNormal), 0.0f);
	float spec = pow(specularStrength, specularExp);
	vec3 specular = spec * baseSpecularColor * light.specular.xyz;


    float theta = dot(-dirToLight, normalize(light.direction.xyz)); 
    float epsilon = light.innerCutoff - light.outerCutoff;
    float falloffCone = clamp((theta - light.outerCutoff) / epsilon, 0.0, 1.0);

    vec3 ambient = 0.1 * baseDiffuseColor * light.ambient.xyz;

	return (diffuse*falloffCone + ambient + specular*falloffCone) * attenuation;
}

void main() {
	vec3 dirToCamera = normalize(camPos - fragPos);
	
	vec3 baseDiffuseColor = hasDiffuseMap ? texture(diffuseMap, vUv).xyz : diffuseColor;
	vec3 baseSpecularColor = hasSpecularMap ? texture(specularMap, vUv).xyz : specularColor;

	vec3 color = vec3(0.0);

	if (numPointLights > 0) {
		for (int i = 0; i < numPointLights; i++) {
			color += calculatePointLight(pointLights[i], baseDiffuseColor, baseSpecularColor, dirToCamera);
		}
	}

	if (numSpotlights > 0) {
		for (int i = 0; i < numSpotlights; i++) {
			color += calculateSpotlight(spotlights[i], baseDiffuseColor, baseSpecularColor, dirToCamera);
		}
	}

	
	FragColor = vec4(color, 1.0);
}