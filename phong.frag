#version 140
#extension GL_ARB_compatibility: enable

in vec4 p;
in vec3 normal, lightDir;

void main()
{	
	gl_FragColor =  gl_Color;
}