#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;

// Output fragment color
out vec4 finalColor;

// NOTE: Add your custom variables here
uniform vec2 mousePos;
uniform vec2 iResolution;
uniform float iTime;
uniform float flashSize;

float random (vec2 noise) {
    return fract(sin(dot(noise.xy,vec2(0.0001,98.233)))*12073103.285);
}


void main()
{
    // Texel color fetching from texture sampler
    vec4 texelColor = texture(texture0, fragTexCoord);

    // NOTE: Implement here your fragment shader code
    if(distance(mousePos, fragTexCoord * iResolution) > flashSize) {
      // Outside flash
      vec2 uv2 = fract(fragTexCoord/iResolution*fract(sin(iTime)));

      texelColor.xyz *= 0.05;
      texelColor.xyz *= vec3(random(uv2.xy))*10;
    } else {
      // Inside flash
      texelColor *= 1.2;
    }

    finalColor = texelColor*colDiffuse*fragColor;
}
