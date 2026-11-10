#version 330 core
in vec3 FragPos;
in vec3 Normal;
out vec4 FragColor;

uniform vec3  viewPos;
uniform vec3  objectColor;
uniform float emissive;        // 0..1:лампы, окна, фары

uniform bool  lightEnabled;    // F:toggle
uniform vec3  sunDir;          
uniform vec3  sunColor;
uniform vec3  ambientColor;

uniform bool  headlightsOn;
uniform vec3  spotPos[2];      
uniform vec3  spotDir;         
uniform float spotInner;       
uniform float spotOuter;       

uniform bool  fogEnabled;      // G: 
uniform vec3  fogColor;
uniform float fogDensity;

void main()
{
    vec3 color;
    if (!lightEnabled)
    {
        color = objectColor;                      
    }
    else
    {
        vec3 N = normalize(Normal);
        vec3 V = normalize(viewPos - FragPos);
        vec3 L = normalize(sunDir);
        vec3 H = normalize(L + V);                 // Blinn-Phong: 

        float diff = max(dot(N, L), 0.0);
        float spec = (diff > 0.0) ? pow(max(dot(N, H), 0.0), 48.0) * 0.25 : 0.0;
        vec3 light = ambientColor + sunColor * (diff + spec);

        if (headlightsOn)
        {
            for (int i = 0; i < 2; i++)
            {
                vec3 toLight = spotPos[i] - FragPos;
                float dist = length(toLight);
                vec3 Ls = toLight / dist;
                float theta = dot(Ls, -normalize(spotDir));
                float k = clamp((theta - spotOuter) / (spotInner - spotOuter), 0.0, 1.0); 
                float att = 1.0 / (1.0 + 0.045 * dist + 0.0015 * dist * dist);            
                float nl = 0.35 + 0.65 * max(dot(N, Ls), 0.0);
                light += vec3(1.0, 0.93, 0.75) * nl * k * att * 2.2;
            }
        }
        color = mix(light * objectColor, objectColor * 1.4, emissive);
    }

    if (fogEnabled)
    {
        float d = length(viewPos - FragPos);
        float f = exp(-pow(d * fogDensity, 2.0));
        color = mix(fogColor, color, clamp(f, 0.0, 1.0));
    }
    FragColor = vec4(color, 1.0);
}