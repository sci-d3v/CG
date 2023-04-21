#ifdef GL_ES
precision mediump float;
#endif
//Use ctrl+shift+G for live preview a fragment shaders
uniform vec2 u_resolution;
vec4 RussianFlag(vec2 position)
{
vec4 color = vec4(0);
if (position.y <= 0.22)
color = vec4(1,vec2(0),1);
else if (position.y <= 0.44)
color = vec4(vec2(0),vec2(1));
else if (position.y <= 0.66)
color = vec4(1);
return color;
}
void main()
{
vec2 position = gl_FragCoord.xy/u_resolution.xy;
gl_FragColor = RussianFlag(position);
}