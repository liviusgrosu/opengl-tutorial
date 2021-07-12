#version 330

// 0 is the location id of this input variable
layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 tex;

out vec4 vColour;
out vec2 texCoord;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;

void main()
{
    // gl_position is a built in output variable
    gl_Position = projection * view * model * vec4(pos, 1.0);
    vColour = vec4(clamp(pos, 0.0f, 1.0f), 1.0f);
	
	texCoord = tex;
}