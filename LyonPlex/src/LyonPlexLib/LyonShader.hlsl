// shaders.hlsl

struct VSInput {
    float3 position : POSITION;
    float4 color : COLOR;
};

struct PSInput {
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

PSInput VSMain(VSInput input) {
    PSInput o;
    o.position = float4(input.position, 1.0f);
    o.color = input.color;
    return o;
}

float4 PSMain(PSInput input) : SV_TARGET{
    return input.color;
}