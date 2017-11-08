#version 330


in vec2 TexCoord0;
in vec3 Normal0;                                                                   
in vec3 WorldPos0;

uniform sampler2D texture_diffuse1;   

out vec4 FragColor;
                                                                
void main()
{                                    
    
    vec2 TexCoord = TexCoord0;
    vec3 Normal   = normalize(Normal0);
    vec3 WorldPos = WorldPos0;
                                                                                    
    FragColor = texture(texture_diffuse1, TexCoord.xy) ;
	  
}
