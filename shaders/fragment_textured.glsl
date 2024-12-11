uniform sampler2D diffuse;
uniform vec2 lightPosition;

varying vec2 texCoordVar;
varying vec2 varPosition;

float attenuate(float dist, float a, float b) {
    return 1.0 / (1.0 + (a * dist) + (b * dist * dist));
}

void main()
{
    vec4 color = texture2D(diffuse, texCoordVar);
    float dist = distance(lightPosition, varPosition);
    float brightness = attenuate(dist, 0.5, 0.05) * 2.0;
    brightness = max(0.5, brightness);
    gl_FragColor = vec4(color.rgb * brightness, color.a);
}
