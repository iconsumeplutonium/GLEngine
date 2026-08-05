#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;
layout (location = 3) in vec3 aTangent;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix;

uniform sampler2D normalMap;
uniform bool hasNormalMap;

out vec3 vNormal;
out vec3 fragPos;
out vec2 vUv;

void main() {
	if (hasNormalMap) {
		vec3 T = normalize((model * vec4(aTangent, 0.0)).xyz);
		vec3 N = normalize((model * vec4(aNormal, 0.0)).xyz);
		vec3 B = cross(N, T);

		mat3 TBN = mat3(T, B, N);

		vNormal = normalize(TBN * (texture(normalMap, aUV).xyz * 2.0 - 1.0));
	} else {
		vNormal = normalize(normalMatrix * aNormal);
	}

	fragPos = (model * vec4(aPos, 1.0)).xyz;
	vUv = aUV;
	gl_Position = projection * view * model * vec4(aPos, 1.0);
}