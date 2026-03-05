struct Light
{
    float3 Strength; // Couleur/Intensité de la lumière
    float FalloffStart; // Pour les Point Lights
    float3 Direction; // Direction (pour DirLight)
    float FalloffEnd; // Pour les Point Lights
    float3 Position; // Position (pour Point/Spot)
    float SpotPower; // Pour les Spotlights
};

struct Material
{
    float4 DiffuseAlbedo; // Couleur de base
    float3 FresnelR0; // Réflexion (métal ou plastique)
    float Shininess; // Brillance (0.0 à 1.0)
};

float3 ComputeDiffuse(Light L, float3 normal)
{
    float lightIntensity = max(dot(normal,L.Direction), 0.0f);
    
    return L.Strength * lightIntensity;
}

float3 ComputePointLight(Light L, float3 pos, float3 normal)
{
    float3 lightVec = pos - L.Position;
    float d = length(lightVec);
    
    if (d > L.FalloffEnd)
        return float3(0.0f, 0.0f, 0.0f);

    lightVec /= d;

    float att = saturate((L.FalloffEnd - d) / (L.FalloffEnd - L.FalloffStart));

    float diffuse = max(dot(normal, lightVec), 0.0f);

    return L.Strength * diffuse * att;
}

float3 ComputeSpotLight(Light L, float3 pos, float3 normal)
{
    float3 lightVec = pos - L.Position;
    float d = length(lightVec);

    if (d > L.FalloffEnd)
        return float3(0.0f, 0.0f, 0.0f);

    lightVec /= d;

    float att = saturate((L.FalloffEnd - d) / (L.FalloffEnd - L.FalloffStart));
    
    float spotFactor = pow(max(dot(-lightVec, normalize(-L.Direction)), 0.0f), L.SpotPower);

    float diffuse = max(dot(normal, lightVec), 0.0f);

    return L.Strength * diffuse * att * spotFactor;
}