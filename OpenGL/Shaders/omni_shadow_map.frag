version 330

in vec4 FragPos;

uniform vec3 lightPos;
uniform float farplane;

void main()
{
	float distance = length(FragPos.xyz - lightPos);
	distance = distance / farplane;
	gl_FragDepth = distance;
}