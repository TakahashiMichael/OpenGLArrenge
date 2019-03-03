#version 410
layout(location=0) in vec4 inColor;
layout(location=1) in vec2 inTexCoord;
layout(location=2) in vec3 inWorldPosition;
out vec4 fragColor;
uniform sampler2D texColor;

const int maxLightCount = 4; // ライトの数.

// ライトデータ(点光源).
struct PointLight
{
  vec4 position; //座標(ワールド座標系).
  vec4 color; // 明るさ.
};

// ライティングパラメータ.
layout(std140) uniform LightData
{
  vec4 ambientColor; // 環境光.
  PointLight light[maxLightCount]; // ライトのリスト.
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

	const float numShades = 5;  //影の段階.
	fragColor.rgb = ceil(fragColor.rgb * numShades)*(1 / numShades);
}
