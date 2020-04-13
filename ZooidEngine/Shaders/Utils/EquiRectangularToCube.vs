#version 430
layout (location=0) in vec3 Pos;

out vec3 FragPos;

layout (std140) uniform draw_data
{
    mat4 projection;
    mat4 view;
};

void main()
{
    FragPos = Pos;
    gl_Position = projection * view * vec4(Pos, 1.0);
}