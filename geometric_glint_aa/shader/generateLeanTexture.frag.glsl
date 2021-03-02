#version 330

// The MIT License
// Copyright © 2021 Xavier Chermain (ICUBE), Simon Lucas(ICUBE), Basile Sauvage (ICUBE), Jean-Michel Dishler (ICUBE) and Carsten Dachsbacher (KIT)
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions: The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software. THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

// Implementation of
// Real-Time Geometric Glint Anti-Aliasing with Normal Map Filtering
// 2021 Xavier Chermain (ICUBE), Simon Lucas(ICUBE), Basile Sauvage (ICUBE), Jean-Michel Dishler (ICUBE) and Carsten Dachsbacher (KIT)
// Accepted for [i3D 2021](http://i3dsymposium.github.io/2021/).

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