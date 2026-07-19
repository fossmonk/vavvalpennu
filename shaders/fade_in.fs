#version 330

// Input variables from Raylib
in vec2 fragTexCoord;
in vec4 fragColor;

// Uniforms passed
uniform sampler2D texture0;
uniform float time;

out vec4 finalColor;

// 2D Hash & Noise Functions
float hash(in vec2 p) {
    p = fract(p * vec2(123.34, 456.21));
    p += dot(p, p + 45.32);
    return fract(p.x * p.y);
}

float noise(in vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    vec2 u = f * f * (3.0 - 2.0 * f);
    return mix(mix(hash(i + vec2(0.0, 0.0)), hash(i + vec2(1.0, 0.0)), u.x),
               mix(hash(i + vec2(0.0, 1.0)), hash(i + vec2(1.0, 1.0)), u.x), u.y);
}

float fbm(in vec2 p) {
    float v = 0.0;
    float a = 0.5;
    vec2 shift = vec2(100.0);
    mat2 rot = mat2(cos(0.5), sin(0.5), -sin(0.5), cos(0.50));
    for (int i = 0; i < 4; ++i) {
        v += a * noise(p);
        p = rot * p * 2.0 + shift;
        a *= 0.5;
    }
    return v;
}

void main()
{
    // Sample the underlying game frame texture
    vec4 texColor = texture(texture0, fragTexCoord);
    
    // Remap coordinates to center (-0.5 to 0.5) for radial expansion
    vec2 centerUv = fragTexCoord - 0.5;
    float dist = length(centerUv);
    
    // Progress calculation matching our C timing configuration
    float progress = time * 0.45; 
    
    // Generate billowing patterns
    vec2 noiseCoords = centerUv * 5.0 - vec2(0.0, time * 0.6);
    float smokeNoise = fbm(noiseCoords);
    
    // Calculate final mask value
    float smokeThreshold = dist + (smokeNoise * 0.3) - progress;
    
    // Clamp ensures it gracefully locks to transparent when time runs out
    float smokeAlpha = clamp(smoothstep(0.0, 0.2, smokeThreshold), 0.0, 1.0);
    
    // Mix game texture with the black smoke color mask
    finalColor = vec4(mix(texColor.rgb, vec3(0.0), smokeAlpha), texColor.a);
}