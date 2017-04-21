#version 130

out vec4 color;  // Sent to the rasterizer for interpolation
out vec3 mynormal ; 
out vec4 myvertex ;

void main(void)
{
    gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex ; 
    color = gl_Color ; 
    mynormal = gl_Normal ; 
    myvertex = gl_Vertex ; 
}
