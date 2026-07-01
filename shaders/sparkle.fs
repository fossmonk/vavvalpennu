in vec2 fragTexCoord; // Input texture coordinates (0.0 to 1.0)
out vec4 finalColor;  // Output pixel color

uniform float time;

void main() {
    // Shift coordinates so (0.0, 0.0) is exactly in the center
    vec2 uv = fragTexCoord - vec3(0.5).xy;
    
    // Calculate basic distance from the center
    float dist = length(uv);
    
    // Create the Cross Flares (The sharp starburst lines)
    // We calculate a very sharp falloff away from the X and Y axes
    float flareX = 0.01 / max(abs(uv.x), 0.001) * max(0.0, 0.2 - abs(uv.y));
    float flareY = 0.01 / max(abs(uv.y), 0.001) * max(0.0, 0.2 - abs(uv.x));
    float starburst = flareX + flareY;
    
    // Create the Core Glow (Soft radial falloff)
    float coreGlow = 0.04 / max(dist, 0.01);
    
    // Animate a subtle twinkling effect using a sine wave over time
    float twinkle = 0.8 + 0.2 * sin(time * 8.0);
    
    // Combine the shapes together
    float intensity = (starburst + coreGlow) * twinkle;
    
    // Smoothly fade out everything towards the edges to prevent square clipping
    intensity *= smoothstep(0.5, 0.1, dist);
    
    // Define the White-Gold Color Palette
    // Core is pure white, outer glow is a rich warm gold
    vec3 goldColor = vec3(1.0, 0.85, 0.5);  // Base Gold
    vec3 whiteCore = vec3(1.0, 1.0, 0.95);  // Bright White-Cream Core
    
    // Mix the colors based on intensity (brighter areas shift to white)
    vec3 finalRGB = mix(goldColor * intensity, whiteCore * intensity, smoothstep(0.5, 2.0, intensity));
    
    // Output final color with transparency based on brightness intensity
    finalColor = vec4(finalRGB, clamp(intensity, 0.0, 1.0));
}