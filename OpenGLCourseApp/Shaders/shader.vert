#version 330

// 0 is the location id of this input variable
layout(location = 0) in vec3 pos;

out vec4 vColour;

uniform mat4 model;
uniform mat4 projection;

void main()
{
    // gl_position is a built in output variable
    gl_Position = projection * model * vec4(pos, 1.0);
    vColour = vec4(clamp(pos, 0.0f, 1.0f), 1.0f);
}