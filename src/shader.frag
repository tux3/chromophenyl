uniform sampler2D sampler;
uniform vec2 u_textureSize;
uniform float u_time;
uniform float u_colAlpha;
uniform float u_globalSpeed;

varying vec2 texcoord0;

vec3 coloredNoiseCos(vec2 uv, float time,
float baseScale, float scaleRange,
float moveSpeed, float scaleSpeed)
{
    float scaleFactor = baseScale + scaleRange*cos(time*scaleSpeed);
    float scaledTime = time*moveSpeed/scaleFactor;
    vec3 scaleFactorVec = vec3(scaleFactor, scaleFactor*1.1, scaleFactor*0.9);
    return cos(scaleFactorVec*(scaledTime+uv.xyx+vec3(0,2,4)));
}

vec3 coloredNoiseSin(vec2 uv, float time,
float baseScale, float scaleRange,
float moveSpeed, float scaleSpeed)
{
    float scaleFactor = baseScale*.9 + scaleRange*1.1*sin(time*scaleSpeed*1.1 + 1.);
    float scaledTime = time*moveSpeed/scaleFactor;
    vec3 scaleFactorVec = vec3(scaleFactor*1.05, scaleFactor*0.95, scaleFactor);
    return sin(scaleFactorVec*(scaledTime+uv.yxy+vec3(3,1.5,0)));
}

vec3 coloredNoise(vec2 uv, float time, float brightness, float alpha,
float baseScale, float scaleRange,
float moveSpeed, float scaleSpeed)
{
    vec3 noise = coloredNoiseCos(uv, time, baseScale, scaleRange, moveSpeed, scaleSpeed)
    * coloredNoiseCos(uv, time, baseScale/2.3, scaleRange, moveSpeed, scaleSpeed)
    * coloredNoiseSin(uv, time, baseScale/1.9, scaleRange, moveSpeed, scaleSpeed)
    * coloredNoiseSin(uv, time, baseScale, scaleRange, moveSpeed, scaleSpeed);

    float brightnessBase = 0.5 * brightness;
    return (brightnessBase + (1.-brightnessBase) * noise) * alpha;
}

highp float random(vec2 coords) {
    return fract(sin(dot(coords.xy, vec2(12.9898,78.233))) * 43758.5453);
}

void main()
{
    float warpStrength = 0.2;
    float warpRes = 3.5;
    float warpSpeed = 0.1 * u_globalSpeed;

    float moveSpeed = -0. * u_globalSpeed;
    float scaleSpeed = 0.05 * u_globalSpeed;
    float baseScale = 3.5;
    float scaleRange = 2.5;
    float colBrightness = 0.;

    vec2 uv = texcoord0;
    uv = uv/u_textureSize;
    uv.t = 1.0 - uv.t;
    vec4 tex = texture(sampler, uv);

    uv.y += sin(warpRes * uv.x + u_time * warpSpeed) * warpStrength;
    vec3 colNoise = coloredNoise(uv, u_time, colBrightness, u_colAlpha,
    baseScale, scaleRange,
    moveSpeed, scaleSpeed);

    vec4 composite = vec4(colNoise.zyx + tex.xyz, 1.0);

    // Dither with noise
    highp float NOISE_GRANULARITY = 0.5/255.0;
    composite += mix(-NOISE_GRANULARITY, NOISE_GRANULARITY, random(texcoord0.xy));

    gl_FragColor = composite;
}
