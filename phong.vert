#version 140
#extension GL_ARB_compatibility: enable


void main()
{	
    // object coordinate in H.C
	//vec4 P_obj = gl_Vertex;
	
	// Clip Coordinate
	gl_Position = gl_ModelViewProjectionMatrix * (gl_Vertex);
	
	gl_FrontColor = vec4(1,0,0,1);
    gl_BackColor  = vec4(0,0,0,0); 
}
