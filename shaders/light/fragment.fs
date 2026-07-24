#version 330 core

uniform sampler2D diffuseMap;
uniform vec3 diffuseColor;
uniform bool hasDiffuseMap;

uniform sampler2D specularMap;
uniform vec3 specularColor;
uniform bool hasSpecularMap;

uniform float specularExp;

struct Light {
	vec3 position;

	vec3 ambient; //idk why the light controls this. just an ambient strength vec3(0.1)
	vec3 diffuse; //light color
	vec3 specular; //always 1.0 to keep shine
};

struct DirectionalLight {
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct PointLight {
	vec3 position;

	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct Spotlight {
	vec3 position;
	vec3 direction;

	float innerCutoff;
	float outerCutoff;
	
	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

#define NUM_POINT_LIGHTS 4

// uniform Light light;
// uniform DirectionalLight dirLight;
// uniform PointLight pointLights[NUM_POINT_LIGHTS];
// uniform Spotlight spotlight;
uniform PointLight pointLight;

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

// vec3 calculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
// 	vec3 lightDir = normalize(light.position - fragPos);
// 	vec3 diffuseMapSample = vec3(texture(material.diffuse, vUv));
// 	vec3 specularMapSample = texture(material.specular, vUv).xyz;

// 	vec3 ambient = light.ambient * diffuseMapSample;

// 	float nDotL = max(dot(normal, lightDir), 0.0);
// 	vec3 diffuse = light.diffuse * nDotL * diffuseMapSample;

// 	vec3 reflectDir = reflect(lightDir, normal);
// 	float spec = max(dot(viewDir, reflectDir), 0.0);
// 	vec3 specular = pow(spec, material.shininess) * specularMapSample * light.specular;

// 	float d = length(light.position - fragPos);
// 	float attenuation = 1.0 / (light.constant + light.linear*d + light.quadratic*d*d);

// 	return (diffuse + ambient + specular) * attenuation;
// }

// vec3 calculateSpotlight(Spotlight light, vec3 normal, vec3 viewDir) {
// 	vec3 lightDir = normalize(light.position - fragPos);
// 	vec3 diffuseMapSample = texture(material.diffuse, vUv).xyz;
// 	vec3 specularMapSample = texture(material.specular, vUv).xyz;

//     float diff = max(dot(normal, lightDir), 0.0);

//     vec3 reflectDir = reflect(-lightDir, normal);
//     float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

//     float d = length(light.position - fragPos);
//     float attenuation = 1.0 / (light.constant + light.linear*d + light.quadratic*d*d);    

//     float theta = dot(lightDir, normalize(-light.direction)); 
//     float epsilon = light.innerCutoff - light.outerCutoff;
//     float intensity = clamp((theta - light.outerCutoff) / epsilon, 0.0, 1.0);

//     vec3 ambient = light.ambient * diffuseMapSample;
//     vec3 diffuse = light.diffuse * diff * diffuseMapSample;
//     vec3 specular = light.specular * spec * specularMapSample;

//     return (ambient + diffuse + specular) * intensity * attenuation;
// }

void main() {
	vec3 normal = normalize(vNormal);
	vec3 dirToCamera = normalize(camPos - fragPos);

	// vec3 result = calculateDirLight(dirLight, normal, viewDir);
	
	// for (int i = 0; i < NUM_POINT_LIGHTS; i++) {
	// 	result += calculatePointLight(pointLights[i], normal, fragPos, viewDir);
	// }

	// result += calculateSpotlight(spotlight, normal, viewDir);

	// vec3 diffuseMapSample = texture(material.diffuse, vUv).xyz;
	// vec3 specularMapSample = texture(material.specular, vUv).xyz;

	vec3 dirToLight = normalize(pointLight.position - fragPos);

	float d = length(pointLight.position - fragPos);
	float attenuation = 1.0 / (pointLight.constant + pointLight.linear*d + pointLight.quadratic*d*d);


	
	vec3 baseDiffuseColor = hasDiffuseMap ? texture(diffuseMap, vUv).xyz : diffuseColor;
	float nDotL = max(dot(dirToLight, normal), 0.0);
	vec3 diffuse = nDotL * baseDiffuseColor * pointLight.diffuse;

	vec3 baseSpecularColor = hasSpecularMap ? texture(specularMap, vUv).xyz : specularColor;
	vec3 halfway = normalize(dirToLight + dirToCamera);
	float specularStrength = max(dot(halfway, normal), 0.0f);
	float spec = pow(specularStrength, specularExp);
	vec3 specular = spec * baseSpecularColor * pointLight.specular;

	vec3 ambient = 0.1 * baseDiffuseColor;
	


	FragColor = vec4((diffuse + ambient + specular), 1.0);
	// FragColor = vec4(normal, 1.0);
}
// #version 330 core
// out vec4 FragColor;

// in vec2 TexCoords;

// uniform sampler2D texture_diffuse1;
// uniform sampler2D texture_specular1;

// void main()
// {    
//     vec4 tex = texture(texture_diffuse1, TexCoords);
//     vec4 tex2 = texture(texture_specular1, TexCoords);
//     // if (tex.a < 0.1)
//     //     discard;

//     FragColor = tex;
//     // FragColor = vec4(TexCoords, 0.0, 1.0);
// }