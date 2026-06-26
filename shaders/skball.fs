#version 330

in vec2 fragTexCoord;
out vec4 finalColor;

uniform sampler2D texture0;
uniform float u_time;

void main() {
    // 1. Shift coordinates so center of rotation is at (0, 0)
    vec2 centeredUV = fragTexCoord - vec2(0.5);

    // 2. Calculate distance from the center BEFORE rotating
    // (This ensures our mask stays perfectly still while the texture rotates inside it)
    float dist = length(centeredUV);

    // 3. Calculate rotation angle over time
    float speed = 4.0;
    float angle = u_time * speed; 
    float cosAngle = cos(angle);
    float sinAngle = sin(angle);

    // 4. Apply the 2D rotation matrix transformation
    vec2 rotatedUV;
    rotatedUV.x = centeredUV.x * cosAngle - centeredUV.y * sinAngle;
    rotatedUV.y = centeredUV.x * sinAngle + centeredUV.y * cosAngle;

    // Shift coordinates back to standard [0, 1] space
    rotatedUV += vec2(0.5);

    // 5. Sample the texture
    vec4 texColor = texture(texture0, rotatedUV);

    float mask = smoothstep(0.5, 0.495, dist);

    finalColor = vec4(texColor.rgb, texColor.a * mask);
}