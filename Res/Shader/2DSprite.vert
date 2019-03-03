#version 410

layout(location=0) in vec3 vPosition;
layout(location=1) in vec4 vColor;
layout(location=2) in vec2 vTexCoord;

layout(location=0) out vec4 outColor;
layout(location=1) out vec2 outTexCoord;

uniform mat4x4 matMVP;
uniform mat4x4 matTexture;
uniform vec4 uniColor;


void main() {
  outColor = vColor * uniColor;
  outTexCoord = (matTexture * vec4(vTexCoord,0.0,1.0)).xy;
  gl_Position = matMVP * vec4(vPosition, 1.0);
}
