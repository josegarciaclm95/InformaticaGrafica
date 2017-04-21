#version 130

in vec4 color;  // interpolated between vertices
in vec3 mynormal ; 
in vec4 myvertex ;

//layout (location = 0) out vec4 FragColor;
//out vec4 FragColor; // color result for this fragment

void main(void)
{
    //FragColor = color;
	gl_FragColor = color;
}
