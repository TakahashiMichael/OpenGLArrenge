#version 410
layout(location=0) in vec4 inColor;
layout(location=1) in vec2 inTexCoord;
layout(location=2) in vec3 inWorldPosition;
out vec4 fragColor;
uniform sampler2D texColor;

const int maxLightCount = 4; // ���C�g�̐�.

// ���C�g�f�[�^(�_����).
struct PointLight
{
  vec4 position; //���W(���[���h���W�n).
  vec4 color; // ���邳.
};

// ���C�e�B���O�p�����[�^.
layout(std140) uniform LightData
{
  vec4 ambientColor; // ����.
  PointLight light[maxLightCount]; // ���C�g�̃��X�g.
} lightData;


void main() {
  fragColor = inColor * texture(texColor, inTexCoord);
    vec3 lightColor = lightData.ambientColor.rgb;
  for (int i = 0; i < maxLightCount; ++i) {
    vec3 lightVector = lightData.light[i].position.xyz - inWorldPosition;
    float lightPower = 1.0 / dot(lightVector, lightVector);
    float cosTheta = clamp(dot(vec3(0, 0, 1), normalize(lightVector)), 0, 1);
    lightColor += lightData.light[i].color.rgb * cosTheta * lightPower;
  }
  fragColor.rgb *= lightColor;

}
