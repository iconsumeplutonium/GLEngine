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
#define MAX_DIR_LIGHTS 10
layout (std140, binding = 0) uniform Lights {
	PointLight pointLights[MAX_POINT_LIGHTS];
	Spotlight spotlights[MAX_SPOTLIGHTS];
	DirectionalLight dirLights[MAX_DIR_LIGHTS];

	int numPointLights;
	int numSpotlights;
	int numDirLights;
};

uniform vec3 camPos;

uniform vec3 albedo;
uniform float metallicness;
uniform float roughness;
uniform float ao;

in vec3 vNormal;
in vec3 fragPos;
in vec2 vUv;

out vec4 FragColor;

vec3 calculateDirLight(DirectionalLight light, vec3 baseDiffuseColor, vec3 baseSpecularColor, vec3 dirToCamera) {
	vec3 dirToLight = normalize(-light.direction);

	vec3 ambient = light.ambient * baseDiffuseColor;

	float nDotL = max(dot(vNormal, dirToLight), 0.0);
	vec3 diffuse = light.diffuse * nDotL * baseDiffuseColor;

	vec3 halfway = normalize(dirToCamera + dirToLight);
	float spec = max(dot(halfway, vNormal), 0.0);
	vec3 specular = pow(spec, specularExp) * baseSpecularColor * light.specular;

	return diffuse + ambient + specular;
}

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

vec3 schlickFresnel(float hDotv, vec3 F0) {
	return F0 + (1.0 - F0) * (pow(1.0 - hDotv, 5.0));
}

float trGGX(vec3 N, vec3 H, float roughness) {
	float a = roughness*roughness;
	float a2 = a*a;
	float NdotH = max(dot(N, H), 0.0);

	float denominator = ((NdotH*NdotH) * (a2-1.0) + 1.0);

	return a2 / (denominator * denominator * 3.14159);
}

float schlickGGX(float NdotV, float roughness) {
	float r = roughness + 1.0;
	float k = (r*r) / 8.0;

	return NdotV / ((NdotV*(1.0-k)) + k);
}

float smithGeometry(vec3 N, vec3 V, vec3 dirToLight, float roughness) {
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, dirToLight), 0.0);

	return schlickGGX(NdotV, roughness) * schlickGGX(NdotL, roughness);
}

void main() {
	vec3 dirToCamera = normalize(camPos - fragPos); // V

	vec3 F0 = vec3(0.04);
	F0 = mix(F0, albedo, metallicness);

	vec3 color = vec3(0.0);

	if (numPointLights > 0) {
		for (int i = 0; i < numPointLights; i++) {
			vec3 dirToLight = normalize(pointLights[i].position.xyz - fragPos); // L
			vec3 H = normalize(dirToLight + dirToCamera);

			float distToLight = length(pointLights[i].position.xyz - fragPos);
			float attentuation = 1.0 / (distToLight * distToLight);
			vec3 radiance = pointLights[i].diffuse.xyz; //* attentuation;

			// normal function (trowbridge-reitz ggx)
			float N = trGGX(vNormal, H, roughness);

			// geometry (schlick ggx or something)
			float G = smithGeometry(vNormal, dirToCamera, dirToLight, roughness);

			// fresnel
			vec3 F = schlickFresnel(max(dot(H, dirToCamera), 0.0), F0);

			// cook torrence (i think)
			vec3 numerator = N * G * F;
			float denominator = 4.0 * max(dot(vNormal, dirToCamera), 0.0) * max(dot(vNormal, dirToLight), 0.0) + 0.001;
			vec3 specular = numerator / denominator;

			vec3 kS = F;
			vec3 kD = (vec3(1.0) - kS) * (1.0 - metallicness);

			float NdotL = max(dot(vNormal, dirToLight), 0.0);
			color += ((kD * albedo / 3.14159) + specular) * radiance * NdotL; // Lo


			// color += calculatePointLight(pointLights[i], baseDiffuseColor, baseSpecularColor, dirToCamera);
		}
	}

	vec3 ambient = vec3(0.03) * albedo * ao;
	color += ambient;

	color /= color + vec3(1.0);
	color = pow(color, vec3(1.0/2.2));
	
	// vec3 baseDiffuseColor = hasDiffuseMap ? texture(diffuseMap, vUv).xyz : diffuseColor;
	// vec3 baseSpecularColor = hasSpecularMap ? texture(specularMap, vUv).xyz : specularColor;


	// if (numPointLights > 0) {
	// 	for (int i = 0; i < numPointLights; i++) {
	// 		color += calculatePointLight(pointLights[i], baseDiffuseColor, baseSpecularColor, dirToCamera);
	// 	}
	// }

	// if (numSpotlights > 0) {
	// 	for (int i = 0; i < numSpotlights; i++) {
	// 		color += calculateSpotlight(spotlights[i], baseDiffuseColor, baseSpecularColor, dirToCamera);
	// 	}
	// }

	// if (numDirLights > 0) {
	// 	for (int i = 0; i < numDirLights; i++) {
	// 		color += calculateDirLight(dirLights[i], baseDiffuseColor, baseSpecularColor, dirToCamera);
	// 	}
	// }
	
	FragColor = vec4(color, 1.0);
}