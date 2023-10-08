#version 450 core

layout(location = 0) in vec3 inputPosition;
layout(location = 1) in vec2 inputUv;
layout(location = 2) in vec4 inputColor;

layout(binding = 1) uniform TimeInfo {
	float time;
	float deltaTime;
} timeInfo;

layout(binding = 2) uniform sampler2D textureSampler;

layout(location = 0) out vec4 outputColor;

vec3 permute(in vec3 x) { return mod(((x*34.0)+1.0)*x, 289.0); }

float snoise(in vec2 v){
	const vec4 C = vec4(0.211324865405187, 0.366025403784439, -0.577350269189626, 0.024390243902439);
	vec2 i  = floor(v + dot(v, C.yy) );
	vec2 x0 = v -   i + dot(i, C.xx);
	vec2 i1;
	i1 = (x0.x > x0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
	vec4 x12 = x0.xyxy + C.xxzz;
	x12.xy -= i1;
	i = mod(i, 289.0);
	vec3 p = permute( permute( i.y + vec3(0.0, i1.y, 1.0 ))
	+ i.x + vec3(0.0, i1.x, 1.0 ));
	vec3 m = max(0.5 - vec3(dot(x0,x0), dot(x12.xy,x12.xy),		dot(x12.zw,x12.zw)), 0.0);
	m = m*m ;
	m = m*m ;
	vec3 x = 2.0 * fract(p * C.www) - 1.0;
	vec3 h = abs(x) - 0.5;
	vec3 ox = floor(x + 0.5);
	vec3 a0 = x - ox;
	m *= 1.79284291400159 - 0.85373472095314 * ( a0*a0 + h*h );
	vec3 g;
	g.x  = a0.x  * x0.x  + h.x  * x0.y;
	g.yz = a0.yz * x12.xz + h.yz * x12.yw;
	return 130.0 * dot(m, g);
}

vec4 distortColor(in vec4 color, in float factor) {
	vec2 distortedOffset = vec2(snoise(inputUv * 10.0 + timeInfo.time), snoise(inputUv * 10.0 + timeInfo.time * 0.5));
	vec2 distortedUv = inputUv + factor * distortedOffset;

	return texture(textureSampler, distortedUv);
}

vec4 pixelizeColor(in float divisions, in float factor, in float noiseScale) {
	vec2 pixelatedUv = floor(inputUv * divisions) / divisions;
	vec2 animatedUv = vec2(snoise(pixelatedUv * noiseScale + timeInfo.time), snoise(pixelatedUv * noiseScale + timeInfo.time * 0.5));

	animatedUv = pixelatedUv + factor * animatedUv;

	//vec3 averageColor = vec3(0.0);
	//for (int x = 0; x < pixelBlock.x; x++) {
	//	for (int y = 0; y < pixelBlock.y; y++) {
	//		ivec2 texCoord = baseCoord + ivec2(x, y);
	//		averageColor += texelFetch(texture, texCoord, 0).rgb;
	//	}
	//}
	//averageColor /= float(pixelBlock.x * pixelBlock.y);

	return texture(textureSampler, animatedUv);
}

void main() {
	vec4 originalColor = texture(textureSampler, inputUv);

	//color = distortColor(color, 0.01);
	vec4 pixelatedColor = pixelizeColor(48, 0.2, 8.0);

	outputColor = mix(originalColor, pixelatedColor, 0.4);
}