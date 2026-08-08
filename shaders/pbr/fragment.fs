#version 460 core


uniform sampler2D albedoMap;
uniform vec3 albedoColor;
uniform bool hasAlbedoMap;

uniform sampler2D metallicRoughnessMap;
uniform float metallicFactor;
uniform float roughnessFactor;
uniform bool hasMetalRoughMap;

uniform sampler2D aoMap;
uniform float aoFactor;
uniform bool hasAOMap;

uniform bool hasNormalMap;


struct DirectionalLight {
	vec4 direction;

	vec4 diffuse;
	vec4 ambient;
	vec4 specular;
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
uniform sampler2D normalMap;

in vec3 vNormal;
in vec3 fragPos;
in vec2 vUv;
// in vec3 vTangent;
// in vec3 vBitangent;
in mat3 TBN;

out vec4 FragColor;

// why is this not fucking built in
float clampdot(vec3 a, vec3 b) {
	return max(dot(a, b), 0.0);
}

vec3 calculateDirLight(DirectionalLight light) {
	return light.diffuse.xyz; //no attenuation for dir lights
}

vec3 calculatePointLight(PointLight light) {
	float d = length(light.position.xyz - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear*d + light.quadratic*d*d);

	vec3 radiance = light.diffuse.xyz * attenuation;

	return radiance;
}

vec3 calculateSpotlight(Spotlight light) {
	vec3 dirToLight = normalize(light.position.xyz - fragPos);

	float d = length(light.position.xyz - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear*d + light.quadratic*d*d);

    float theta = dot(-dirToLight, normalize(light.direction.xyz)); 
    float epsilon = light.innerCutoff - light.outerCutoff;
    float falloffCone = clamp((theta - light.outerCutoff) / epsilon, 0.0, 1.0);


	return light.diffuse.xyz * falloffCone * attenuation;
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

vec3 getLoForLight(vec3 H, vec3 radiance, vec3 dirToCamera, vec3 dirToLight, vec3 F0, vec3 albedo, float roughness, float metallicness, float ao, vec3 normal) {
	// normal function (trowbridge-reitz ggx)
	float N = trGGX(normal, H, roughness);

	// geometry (schlick ggx or something)
	float G = smithGeometry(normal, dirToCamera, dirToLight, roughness);

	// fresnel
	vec3 F = schlickFresnel(clampdot(H, dirToCamera), F0);

	// cook torrence (i think)
	vec3 numerator = N * G * F;
	float denominator = 4.0 * clampdot(normal, dirToCamera) * clampdot(normal, dirToLight) + 0.001;
	vec3 specular = numerator / denominator;

	vec3 kS = F;
	vec3 kD = (vec3(1.0) - kS) * (1.0 - metallicness);

	float NdotL = clampdot(normal, dirToLight);
	vec3 Lo = ((kD * albedo / 3.14159) + specular) * radiance * NdotL; // Lo

	return Lo;
}

void main() {
	vec3 dirToCamera = normalize(camPos - fragPos); // V

	vec3 normal = normalize(vNormal);
	if (hasNormalMap) {
		// mat3 TBN = mat3(normalize(vTangent), normalize(vBitangent), normalize(vNormal));
		normal = normalize(TBN * (texture(normalMap, vUv).xyz * 2.0 - 1.0));
	}

	vec3 albedo = hasAlbedoMap ? texture(albedoMap, vUv).xyz : albedoColor;
	float roughness = hasMetalRoughMap ? texture(metallicRoughnessMap, vUv).g : roughnessFactor;
	float metallicness = hasMetalRoughMap ? texture(metallicRoughnessMap, vUv).b : metallicFactor;
	float ao = hasAOMap ? texture(aoMap, vUv).r : aoFactor;

	vec3 F0 = vec3(0.04);
	F0 = mix(F0, albedo, metallicness);

	vec3 color = vec3(0.0);

	if (numPointLights > 0) {
		for (int i = 0; i < numPointLights; i++) {
			vec3 dirToLight = normalize(pointLights[i].position.xyz - fragPos); // L
			vec3 H = normalize(dirToLight + dirToCamera);

			vec3 radiance = calculatePointLight(pointLights[i]);
			color += getLoForLight(H, radiance, dirToCamera, dirToLight, F0, albedo, roughness, metallicness, ao, normal);
		}
	}

	if (numSpotlights > 0) {
		for (int i = 0; i < numSpotlights; i++) {
			vec3 dirToLight = normalize(spotlights[i].position.xyz - fragPos); // L
			vec3 H = normalize(dirToLight + dirToCamera);

			vec3 radiance = calculateSpotlight(spotlights[i]);
			color += getLoForLight(H, radiance, dirToCamera, dirToLight, F0, albedo, roughness, metallicness, ao, normal);
		}
	}

	if (numDirLights > 0) {
		for (int i = 0; i < numDirLights; i++) {
			vec3 dirToLight = normalize(-dirLights[i].direction.xyz); // L
			vec3 H = normalize(dirToLight + dirToCamera);

			vec3 radiance = calculateDirLight(dirLights[i]);
			color += getLoForLight(H, radiance, dirToCamera, dirToLight, F0, albedo, roughness, metallicness, ao, normal);
		}
	}

	vec3 ambient = vec3(0.03) * albedo * ao;
	color += ambient;

	color /= color + vec3(1.0);
	color = pow(color, vec3(1.0/2.2));
	
	FragColor = vec4(color, 1.0);
}