#include "utils/structs.glsl"

uniform sampler2D inport;
uniform ImageParameters inportParameters;
uniform ImageParameters outportParameters;

uniform sampler2D vfColor;

float passThrough(vec2 coord){
    return texture(inport, coord).x;
}

float magnitude( vec2 coord ){
    //TASK 1: find the magnitude of the vectorfield at the position coords
    vec2 velo = texture(vfColor , coord.xy).xy;

    return sqrt(velo.x * velo.x + velo.y * velo.y);
}

float divergence(vec2 coord){
    //TASK 2: find the divergence of the vectorfield at the position coords
    vec2 pixelSize = inportParameters.reciprocalDimensions;

    vec2 v1 = texture(vfColor, vec2(coord.x + pixelSize.x, coord.y)).xy;
    vec2 v2 = texture(vfColor, vec2(coord.x - pixelSize.x, coord.y)).xy;

    vec2 v3 = texture(vfColor, vec2(coord.x, coord.y + pixelSize.y)).xy;
    vec2 v4 = texture(vfColor, vec2(coord.x, coord.y - pixelSize.y)).xy;

    vec2 dvdx = (v1 - v2) / (2*pixelSize.x);
    vec2 dvdy = (v3 - v4) / (2*pixelSize.y);

    return dvdx.x + dvdy.y;

}

float rotation(vec2 coord){
    //TASK 3: find the curl of the vectorfield at the position coords
    vec2 pixelSize = inportParameters.reciprocalDimensions;

    
    vec2 v1 = texture(vfColor, vec2(coord.x + pixelSize.x, coord.y)).xy;
    vec2 v2 = texture(vfColor, vec2(coord.x - pixelSize.x, coord.y)).xy;

    vec2 v3 = texture(vfColor, vec2(coord.x, coord.y + pixelSize.y)).xy;
    vec2 v4 = texture(vfColor, vec2(coord.x, coord.y - pixelSize.y)).xy;

    vec2 dvdx = (v1 - v2) / (2*pixelSize.x);
    vec2 dvdy = (v3 - v4) / (2*pixelSize.y);

    return dvdx.x - dvdy.y;

}

void main(void) {
    vec2 texCoords = gl_FragCoord.xy * outportParameters.reciprocalDimensions;

    float v = OUTPUT(texCoords);

    FragData0 = vec4(v);
}
