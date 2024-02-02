struct DirectionalLight
{
	vec3 ambient;
	vec3 diffuse;
	vec3 direction;
	float intensity;
};

struct PointLight
{
	vec3 ambient;
	vec3 diffuse;
	vec3 position;
	float intensity;
	float constantAttenuation;
	float linearAttenuation;
	float quadraticAttenuation;
};

struct SpotLight
{
	vec3 ambient;
	vec3 diffuse;
	vec3 position;
	vec3 direction;
	float intensity;
	float constantAttenuation;
	float linearAttenuation;
	float quadraticAttenuation;
	float cutOffAngleCos;
	float cutOffAngleOutCos;
};

struct Material
{
	vec4 diffuse;
	vec3 specular;
	float shininess;
};

uniform Material material;

#define MAX_DIR_LIGHTS 4
#define MAX_POINT_LIGHTS 32
#define MAX_SPOT_LIGHTS 32
uniform DirectionalLight directionalLight[MAX_DIR_LIGHTS];
uniform PointLight pointLight[MAX_POINT_LIGHTS];
uniform SpotLight spotLight[MAX_SPOT_LIGHTS];
uniform int activeLightsDir;
uniform int activeLightsPoint;
uniform int activeLightsSpot;

vec3 CalculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDirection)
{
	vec3 lightVector = normalize(-light.direction);

	float diff = max(dot(normal, lightVector), 0.0);

	vec3 reflectVector = reflect(-lightVector, normal);
	float spec = pow(max(dot(viewDirection, reflectVector), 0.0), material.shininess);

	vec3 ambient = light.ambient * material.diffuse.xyz;
	vec3 diffuse = light.diffuse * diff * material.diffuse.xyz;
	vec3 specular = light.diffuse * spec * material.specular;

	return light.intensity * (ambient + diffuse + specular);
}

vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 viewDirection)
{
	vec3 lightVector = normalize(light.position - WorldPosition);

	float diff = max(dot(normal, lightVector), 0.0);

	vec3 reflectVector = reflect(-lightVector, normal);
	float spec = pow(max(dot(viewDirection, reflectVector), 0.0), material.shininess);

	float distance = length(light.position - WorldPosition);
	float attenuation = 1.0 / (light.constantAttenuation + light.linearAttenuation * distance 
		+ light.quadraticAttenuation * (distance * distance));

	vec3 ambient = light.ambient * material.diffuse.xyz;
	vec3 diffuse = light.diffuse * diff * material.diffuse.xyz;
	vec3 specular = light.diffuse * spec * material.specular;

	return light.intensity * attenuation * (ambient + diffuse + specular);
}

vec3 CalculateSpotLight(SpotLight light, vec3 normal, vec3 viewDirection)
{
	vec3 lightVector = normalize(light.position - WorldPosition);

	float diff = max(dot(normal, lightVector), 0.0);

	vec3 reflectVector = reflect(-lightVector, normal);
	float spec = pow(max(dot(viewDirection, reflectVector), 0.0), material.shininess);

	float distance = length(light.position - WorldPosition);
	float attenuation = 1.0 / (light.constantAttenuation + light.linearAttenuation * distance 
		+ light.quadraticAttenuation * (distance * distance));

	float theta = dot(lightVector, normalize(-light.direction));
	float epsilon = light.cutOffAngleCos - light.cutOffAngleOutCos;
	float intensity = clamp((theta - light.cutOffAngleOutCos) / epsilon, 0.0, 1.0);

	vec3 ambient = light.ambient * material.diffuse.xyz;
	vec3 diffuse = light.diffuse * diff * material.diffuse.xyz, TexCoords;
	vec3 specular = light.diffuse * spec * material.specular;

	return light.intensity * attenuation * intensity * (ambient + diffuse + specular);
}