#define MAX_LIGHTS 16
#define POINT_LIGHT 0
#define SPOT_LIGHT 1
#define DIRECTIONAL_LIGHT 2

struct Light{
	int type;
	float intensity;
	vec3 direction;
	vec3 color;
}