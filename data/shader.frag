#version 330 core
out vec4 FragColor;

in VS_OUT {
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
} fs_in;

uniform vec3 viewPos;
uniform vec3 lightDir;
uniform vec3 lightColor;
uniform vec3 lightAmbient;

uniform vec3 albedo;

void main()
{
	// ambient
	vec3 ambient = lightAmbient * albedo;
	
	// diffuse
	vec3 normal = normalize(fs_in.Normal);
	float diff = max(dot(lightDir, normal), 0.0);
	vec3 diffuse = diff * albedo * lightColor;
	
	// specular (Blinn-Phong)
	vec3 viewDir = normalize(viewPos - fs_in.FragPos);
	vec3 reflectDir = reflect(-lightDir, normal);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
	vec3 specular = vec3(0.2) * spec * lightColor;
	
	FragColor = vec4(ambient + diffuse + specular, 1.0);
}