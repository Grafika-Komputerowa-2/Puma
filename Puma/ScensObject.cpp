#include "ScensObject.h"

Shader ScensObject::SwapShaders(Shader newShader)
{
    Shader oldShader = shader;
    shader = newShader;
    return oldShader;
}
