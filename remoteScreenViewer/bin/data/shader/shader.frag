#version 120

uniform sampler2DRect tex0;
uniform sampler2DRect tex1;
uniform float blur;

varying vec2 texCoordVarying;

void main()
{
    // Get color value from
    vec4 src = texture2DRect(tex0, texCoordVarying);

    // Get alpha value
    vec4 buf = texture2DRect(tex1, texCoordVarying);

    // Set
    gl_FragColor = vec4(mix(src, buf, vec4(blur)).rgb, 1.0);
}