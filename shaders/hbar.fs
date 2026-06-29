in vec4 fragColor;
out vec4 finalColor;


// Uniforms from C code
uniform float u_topY;
uniform float u_recH;
uniform float u_winHeight;

void main() {
    vec3 baseColor = fragColor.rgb;
    vec3 pureWhite = vec3(1.0, 1.0, 1.0);

    float rlY = u_winHeight - gl_FragCoord.y;
    float localY = (rlY - u_topY)/u_recH;

    float distFromCenter = abs(localY - 0.5);
    float centerGradient = smoothstep(0.95, 0.1, distFromCenter);

    vec3 mixedColor = mix(pureWhite, baseColor, centerGradient * 1.2);
    finalColor = vec4(mixedColor, fragColor.a);
}