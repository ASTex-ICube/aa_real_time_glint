#version 330
uniform sampler2D img_input;
layout(location = 0) out vec4 img_output1;
layout(location = 1) out vec4 img_output2;

uniform int Width;
uniform int Height;
uniform float Scale;

void main() {

  
  ivec2 xy = ivec2(gl_FragCoord.xy);
	
  ivec2 xp1y = xy + ivec2(1,0);
  ivec2 xm1y = xy - ivec2(1,0);
  ivec2 xyp1 = xy + ivec2(0,1);
  ivec2 xym1 = xy - ivec2(0,1);

  xp1y.x = xp1y.x % Width;
  xm1y.x = xm1y.x % Width;

  xyp1.y = xyp1.y % Height;
  xym1.y = xym1.y % Height;

 
  float hxp1y  = texelFetch(img_input,xp1y,0).x;
  float hxm1y  = texelFetch(img_input,xm1y,0).x;
  float hxyp1  = texelFetch(img_input,xyp1,0).x;
  float hxym1  = texelFetch(img_input,xym1,0).x;
  
  float s_x,s_y,s_xx,s_yy,s_xy;

  s_x = hxp1y - hxm1y;
  s_y = hxyp1 - hxym1;
  s_x *= Scale * 0.5;
  s_y *= Scale * 0.5;
  s_xy = s_x*s_y;
  s_xx = s_x*s_x;
  s_yy = s_y*s_y;
  
  // output to a specific pixel in the image
  img_output1 = vec4(s_x, s_y,0.,0.);
  img_output2 = vec4(s_xx, s_yy, s_xy, 0.);

}