// shaders.hlsl

// CAMERA
cbuffer CameraBuffer : register(b0)
{
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
};

// MESH
struct VSInput {
    float3 position : POSITION;
    float4 color : COLOR;
};

struct PSInput {
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

PSInput VSMain(VSInput input) {
    PSInput output;

    //camera---------Pour remettre l'ancien affichage commente la parie camera et de-commente le dernier outpout.position
    // Etape 1 : modeleÅ®monde (ici identity)
    float4 worldPosition = float4(input.position, 1.0f);
    // Etape 2 : mondeÅ®camera (view)
    float4 viewPosition = mul(worldPosition, viewMatrix);
    // Etape 3 : projection
    output.position = mul(viewPosition, projectionMatrix);
    //---------------

    //output.position = float4(input.position, 1.0f); //ORIGNAL
    
    output.color = input.color;
    return output;
}

float4 PSMain(PSInput input) : SV_TARGET{
    return input.color;
}