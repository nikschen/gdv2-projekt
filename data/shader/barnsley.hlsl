// -----------------------------------------------------------------------------
// Define the constant buffers.
// -----------------------------------------------------------------------------
cbuffer VSPerFrameConstants : register(b0)
{
    float4x4 c_VSViewProjectionMatrix;
}

cbuffer VSPerObjectConstants : register(b1)
{
    float4x4 c_VSWorldMatrix;
}

cbuffer PSPerObjectConstants : register(b0)
{
    float   c_PSRandNr;
    int     c_PSVerticalResolution;
    int     c_PSHorizontalResolution;
    float   m_PSTiltFactorR;
}

// -----------------------------------------------------------------------------
// Define input and output data of the vertex shader.
// -----------------------------------------------------------------------------
struct VSInput
{
    float3 m_Position : OSPOSITION;
    float2 m_PositionNormed : POSITION_NORMED;
};

struct PSInput
{
    float4 m_Position : SV_POSITION;
    float2 m_PositionNormed : TEXCOORD0;
};

// -----------------------------------------------------------------------------
// Vertex Shaders
// -----------------------------------------------------------------------------
PSInput VSMain(VSInput _Input)
{
    float4 WSPosition;
    float4 CSPosition;
    PSInput Result;

    WSPosition              = mul(float4(_Input.m_Position, 1.0f), c_VSWorldMatrix);
    CSPosition              = mul(WSPosition, c_VSViewProjectionMatrix);   

    Result.m_Position       = CSPosition;
    Result.m_PositionNormed = _Input.m_PositionNormed;

    return Result;
}

float hash(float n) //shader level random number generator
{
    return frac(sin(n));
}

float4 Barnsley(float2 _Position, int2 _Resolution)
{
    float2 Pos = _Position * 1200; //correct zoom
    float2 nextPoint = float2(0.0f,0.0f);
    float2 uvCoords = Pos.xy / _Resolution.xy;
    uvCoords = uvCoords * 12.0 - float2(10.0f, 0.0f); //correct coordinates and 'zoom' to fit fractal into window
        
    // p => starting hash|random number
    float p = hash(dot(Pos.xy, _Resolution) + c_PSRandNr);
    

    const float iterations = 1024.0f;
    const float tolerance = 0.045f;
    float4 Color;
    
    
    for (float i = 0; i < tolerance; i += tolerance / iterations)
    {
        p = frac(p / 0.12345f); //new pseudo random number per iteration
        
        //affine transformation matrices & vectors

        float2x2 stemMatrix                 = float2x2( 0.00f,  0.00f, 
                                                        0.00f,  0.16f);
        
        float2x2 smallLeavesMatrix          = float2x2( 0.85f,  m_PSTiltFactorR, 
                                                       -0.04f,  0.85f);
        
        float2x2 largestLefthandLeafMatrix  = float2x2( 0.20f, -0.26f, 
                                                        0.23f,  0.22f);
        
        float2x2 largestRighthandLeafMatrix = float2x2(-0.15f,  0.28f,
                                                        0.26f,  0.24f);
        
        float2 smallLeavesAddVector             = float2(0.00f, 1.60f);
        
        float2 largestLefthandLeafAddVector     = float2(0.00f, 1.60f); 
        
        float2 largestRighthandLeafAddVector    = float2(0.00f, 0.44f);
        
        
        //decide which point is next with given probabilities (part of the barnsley algorithm to ensure a good result)
        if (p < 0.01f)
        {
            nextPoint = mul(stemMatrix, nextPoint); 
        }
        else if (p < 0.84)
        {
            nextPoint = mul(smallLeavesMatrix, nextPoint);
            nextPoint += smallLeavesAddVector;
        }
        else if (p < 0.92)
        {
            nextPoint = mul(largestLefthandLeafMatrix, nextPoint);
            nextPoint += largestLefthandLeafAddVector;
        }
        else
        {
            nextPoint = mul(largestRighthandLeafMatrix, nextPoint);
            nextPoint += largestRighthandLeafAddVector;
        }
        
        float result = sqrt(dot(uvCoords - nextPoint, uvCoords - nextPoint)); //get distance between current uv coordinates and next possible point of the fern (length of vector between both) via square root of the dot product
		 
        // check if current pixel is near the potential next point of the fern based on the given tolerance 
        if (result < tolerance)
        {
			Color = float4(0.0, 1.0, 0.0, 1.0);
            return Color;
        }
    }
    
    return Color;
    
    
}

// -----------------------------------------------------------------------------
// Pixel Shader
// -----------------------------------------------------------------------------
float4 PSMain(PSInput _Input) : SV_Target
{
    return Barnsley(_Input.m_PositionNormed, int2(c_PSVerticalResolution, c_PSHorizontalResolution));
}

// used resources:
// https://en.wikipedia.org/wiki/Barnsley_fern
// https://www.algorithm-archive.org/contents/barnsley/barnsley.html
// https://de.serlo.org/mathe/1777/l%C3%A4nge-eines-vektors
