#version 330 core

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

uniform float radius;
uniform float power;

// Output fragment color
out vec4 finalColor;

void main() {
    float r = radius; // Radius for alpha cutoff
    vec2 p = fragTexCoord - vec2(0.5); // Center the coordinates
    float distance = length(p);

    if (distance <= 0.5) {
        float s = distance - r; // Distance from edge of inner radius
        if (s <= 0.0) {
            finalColor = fragColor*1.5; // Fully opaque inside radius
        } else {
            float t = 1.0 - s / (0.5 - r); // Calculate alpha based on distance
            finalColor = mix(vec4(fragColor.xyz, 0), fragColor*1.5, pow(t, power)); // Set alpha using a power function for smoother transitions
        }
    } else {
        finalColor = vec4(0.0); // Fully transparent outside radius
    }
}