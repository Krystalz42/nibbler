#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in mat4 instanceMatrix;

smooth out vec2 TexCoords;
smooth out vec3 Normal;
smooth out vec3 Position;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    TexCoords = aTexCoords;
    Normal = mat3(transpose(inverse(instanceMatrix))) * aNormal;
    Position = vec3(instanceMatrix * vec4(aPos, 1.f));
    gl_Position = projection * view * instanceMatrix * vec4(aPos, 1.0);
}