#version 130

in vec4 color;  // Interpolated between vertices
in vec3 mynormal ; 
in vec4 myvertex ;

// Lighting parameters
const int maxLights = 10;  // Max number of lights 
uniform bool enablelighting;
uniform vec4 lightposn[maxLights];  // Positions of lights 
uniform vec4 lightambient[maxLights]; 
uniform vec4 lightdiffuse[maxLights];  
uniform vec4 lightspecular[maxLights];   
uniform bool lightenabled[maxLights];  
uniform int numused;  // Number of lights used

// Now, set the material parameters,  
// I use ambient, diffuse, specular, shininess as in OpenGL  
uniform vec4 ambient; 
uniform vec4 diffuse; 
uniform vec4 specular; 
uniform vec4 emission; 
uniform float shininess; 

void main (void) 
{       
    if (enablelighting) {   
	    
        vec4 finalcolor; 

        const vec3 eyepos = vec3(0,0,0); 
        vec4 _mypos = gl_ModelViewMatrix * myvertex; 
        vec3 mypos = _mypos.xyz / _mypos.w ; // Dehomogenize current location 
        vec3 vdirn = normalize(eyepos - mypos); 

        // Compute normal, needed for shading. 
        // Simpler is vec3 normal = normalize(gl_NormalMatrix * mynormal); 
        vec3 _normal = (gl_ModelViewMatrixInverseTranspose*vec4(mynormal,0.0)).xyz; 
        vec3 normal = normalize(_normal); 

		finalcolor = emission;

		// Lights
		for (int i=0; i<numused; i++) {
				
			if (lightenabled[i]) {

				// Ambient term
				finalcolor += ambient * lightambient[i];

				vec3 ldirn;
				if (lightposn[i].w == 0) {

					// Directional light
					ldirn = normalize(lightposn[i].xyz);
				} else {

					// Point light
					vec3 lposn = lightposn[i].xyz / lightposn[i].w ; 
					ldirn = normalize (lposn - mypos) ; // No attenuation
				}

				float lDotN = dot(ldirn, normal);
				if (lDotN > 0) {
				
					// Diffuse (lambert) reflection
					vec4 lambert =  diffuse * lightdiffuse[i] * max (lDotN, 0.0) ;  

					// Specular (mirror) reflection
					vec3 rdirn = (2 * lDotN * normal) - ldirn;  // R = 2(L�N)N-L
					float rDotV = dot(rdirn, vdirn);
					vec4 mirror = specular * lightspecular[i] * pow (max(rDotV, 0.0), shininess) ;
						
					// Specular	reflection, using the half vector	 
					//vec3 halfvec = normalize (ldirn + vdirn) ; 
					//float nDotH = dot(normal, halfvec) ; 
					//vec4 mirror = specular * lightspecular[i] * pow (max(nDotH, 0.0), shininess) ; 

					finalcolor += lambert + mirror;
				}
			}
		}

        gl_FragColor = finalcolor; 
    } else {
		gl_FragColor = color; 
    }
}