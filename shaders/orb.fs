in vec2 fragTexCoord;
out vec4 finalColor;

uniform sampler2D texture0;
uniform sampler2D u_noiseTex;
uniform float u_time;
uniform vec3 u_shade;

void main() {
    // High-speed timelines for noise scrolling
    float fastTimeA = u_time * 2.5; 
    float fastTimeB = u_time * 3.2;

    vec2 uvScrollA = fragTexCoord + vec2(fastTimeA * 0.15, fastTimeA * 0.12);
    vec2 uvScrollB = fragTexCoord + vec2(fastTimeB * -0.10, fastTimeB * 0.18);

    float noiseA = texture(u_noiseTex, uvScrollA).r;
    float noiseB = texture(u_noiseTex, uvScrollB).g;
    float combinedNoise = (noiseA + noiseB) * 0.5;

    // Heavy warping distortion calculation
    vec2 distortedUV = fragTexCoord + (vec2(combinedNoise) - 0.5) * 0.5;

    // Shift coordinates so center of rotation is at (0, 0)
    vec2 centeredUV = distortedUV - vec2(0.5);

    // Calculate rotation angle over time
    float speed = 3.0;
    float angle = u_time * speed; 
    float cosAngle = cos(angle);
    float sinAngle = sin(angle);

    // Apply the 2D rotation matrix transformation
    vec2 rotatedUV;
    rotatedUV.x = centeredUV.x * cosAngle - centeredUV.y * sinAngle;
    rotatedUV.y = centeredUV.x * sinAngle + centeredUV.y * cosAngle;

    // Shift coordinates back from (0,0) to standard (0.5, 0.5) space
    distortedUV = rotatedUV + vec2(0.5);

    // Apply Circular Mask
    float distFromCenter = distance(fragTexCoord, vec2(0.5));
    float edgeMask = smoothstep(0.5, 0.45, distFromCenter);
    distortedUV = mix(fragTexCoord, distortedUV, edgeMask);

    // Sample the main asset using new coord grid
    vec4 orbColor = texture(texture0, distortedUV);

    // Aggressive, stable glow pulse
    float pulse = sin(u_time * 12.0) * 0.15 + 0.95;
    orbColor.rgb *= pulse;
    orbColor.rgb *= u_shade.rgb;

    finalColor = orbColor;
}