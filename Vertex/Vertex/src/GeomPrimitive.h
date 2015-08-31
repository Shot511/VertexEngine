#pragma once

#include "Model.h"

class GeomPrimitive
{
public:
    GeomPrimitive()  = delete;
    ~GeomPrimitive() = delete;

    friend class Model;

private:
    static void genCube    (VEBuffers &buffers, float radius);
    static void genTorus   (VEBuffers &buffers, float innerRadius, float outerRadius, unsigned int slices, unsigned int stacks);
    static void genCylinder(VEBuffers &buffers, float height, float r, unsigned int slices);
    static void genCone    (VEBuffers &buffers, float height, float r, unsigned int slices, unsigned int stacks);
    static void genQuad    (VEBuffers &buffers, float width, float height);
    static void genPlane   (VEBuffers &buffers, float width, float height, unsigned int slices, unsigned int stacks);
    static void genSphere  (VEBuffers &buffers, float r, unsigned int slices);
};

