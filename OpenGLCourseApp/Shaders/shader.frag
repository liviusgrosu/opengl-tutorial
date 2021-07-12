#version 330

in vec4 vColour;
in vec2 texCoord;

out vec4 colour;

// This is taking from the active texture buffer (ex: GL_TEXTURE0)
uniform sampler2D textureSampler;

void main()
{
	colour = texture(textureSampler, texCoord);
}