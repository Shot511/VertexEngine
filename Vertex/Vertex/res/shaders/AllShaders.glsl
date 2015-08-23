#VS

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord;

out vec3 o_normal;
out vec2 o_texcoord;

void main()
{
    o_normal   = normal;
    o_texcoord = texcoord;

    gl_Position = vec4(position.xy, -0.5f, 1.0f);
}

#FS

in vec3 o_normal;
in vec2 o_texcoord;

out vec4 fragColor;

void main()
{
    fragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}