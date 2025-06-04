cbuffer CameraBuffer : register(b0)
{
    float4x4 viewMatrix;
    float4x4 projectMatrix;
};
cbuffer ObjectBuffer : register(b1)
{
    float4x4 worldMatrix;
};

struct VSInput
{
    float3 position : POSITION;
    float4 color : COLOR;
};
struct PSInput
{
    float4 positionH : SV_POSITION;
    float4 color : COLOR;
};

PSInput VSMain(VSInput input)
{
    PSInput output;
    float4 worldPos = mul(float4(input.position, 1.0f), worldMatrix);
    float4 viewPos = mul(worldPos, viewMatrix);
    output.positionH = mul(viewPos, projectMatrix);
    
    output.color = input.color;
    return output;
};

float4 PSMain(PSInput input) : SV_Target
{
    return input.color;
}