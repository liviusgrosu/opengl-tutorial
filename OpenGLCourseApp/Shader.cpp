#include "Shader.h"

Shader::Shader()
{
	shaderID = 0;
	uniformModel = 0;
	uniformProjection = 0;

    pointLightCount = 0;
    spotLightCount = 0;
}

void Shader::CreateFromString(const char* vertexCode, const char* fragmentCode)
{
	CompileShader(vertexCode, fragmentCode);
}

void Shader::CreateFromFiles(const char* vertexLocation, const char* fragmentLocation)
{
    // Get the code from their respective files
	std::string vertexString = ReadFile(vertexLocation);
	std::string fragmentString = ReadFile(fragmentLocation);
	const char* vertexCode = vertexString.c_str();
	const char* fragmentCode = fragmentString.c_str();

    // Generate executables of the shaders
	CompileShader(vertexCode, fragmentCode);
}

void Shader::CreateFromFiles(const char* vertexLocation, const char* geometryLocation, const char* fragmentLocation)
{
    // Get the code from their respective files
    std::string vertexString = ReadFile(vertexLocation);
    std::string fragmentString = ReadFile(fragmentLocation);
    std::string geometryString = ReadFile(geometryLocation);
    const char* vertexCode = vertexString.c_str();
    const char* fragmentCode = fragmentString.c_str();
    const char* geometryCode = geometryString.c_str();

    // Generate executables of the shaders
    CompileShader(vertexCode, geometryCode, fragmentCode);
}

std::string Shader::ReadFile(const char* fileLocation)
{
	std::string content;
	std::ifstream fileStream(fileLocation, std::ios::in);

	if (!fileStream.is_open()) {
		printf("Failed to read %s! File doesn't exist.", fileLocation);
		return "";
	}

	std::string line = "";
	while (!fileStream.eof()) {
		std::getline(fileStream, line);
		content.append(line + "\n");
	}

	fileStream.close();
	return content;
}

GLuint Shader::GetProjectionLocation() {
	return uniformProjection;
}

GLuint Shader::GetModelLocation() {
	return uniformModel;
}

GLuint Shader::GetViewLocation() {
	return uniformView;
}

GLuint Shader::GetAmbientColourLocation() {
	return uniformDirectionalLight.uniformColour;
}

GLuint Shader::GetAmbientIntensityLocation() {
	return uniformDirectionalLight.uniformAmbientIntensity;
}

GLuint Shader::GetDiffuseIntensityLocation() {
	return uniformDirectionalLight.uniformDiffuseIntensity;
}

GLuint Shader::GetDirectionLocation() {
	return uniformDirectionalLight.uniformDirection;
}

GLuint Shader::GetCameraPositionLocation() {
    return uniformCameraPosition;
}

GLuint Shader::GetSpecularIntensityLocation() {
    return uniformSpecularIntensity;
}

GLuint Shader::GetShininessLocation() {
    return uniformShininess;
}

GLuint Shader::GetOmniLightPositionLocation()
{
    return uniformOmniLightPos;
}

GLuint Shader::GetFarPlaneLocation()
{
    return uniformFarPlane;
}

void Shader::SetDirectionalLight(DirectionalLight* directionalLight)
{
    directionalLight->UseLight(uniformDirectionalLight.uniformAmbientIntensity, uniformDirectionalLight.uniformColour, 
                                uniformDirectionalLight.uniformDiffuseIntensity, uniformDirectionalLight.uniformDirection);
}

void Shader::SetPointLights(PointLight* pointLight, unsigned int lightCount, unsigned int textureUnit, unsigned int offset) {
    if (lightCount > MAX_POINT_LIGHTS) {
        lightCount = MAX_POINT_LIGHTS;
    }

    glUniform1i(uniformPointLightCount, lightCount);

    for (size_t i = 0; i < lightCount; i++) {
        pointLight[i].UseLight(uniformPointLight[i].uniformAmbientIntensity, uniformPointLight[i].uniformColour, 
                                uniformPointLight[i].uniformDiffuseIntensity, uniformPointLight[i].uniformPosition,
                                uniformPointLight[i].uniformConstant, uniformPointLight[i].uniformLinear, uniformPointLight[i].uniformExponent);

        pointLight[i].GetShadowmap()->Read(GL_TEXTURE0 + textureUnit + i);
        glUniform1i(uniformOmniShadowmap[i + offset].shadowMap, textureUnit + i);
        glUniform1f(uniformOmniShadowmap[i + offset].farPlane, pointLight[i].GetFarPlane());
    }
}

void Shader::SetSpotLights(SpotLight* spotLight, unsigned int lightCount, unsigned int textureUnit, unsigned int offset)
{
    if (lightCount > MAX_SPOT_LIGHTS) {
        lightCount = MAX_SPOT_LIGHTS;
    }

    glUniform1i(uniformSpotLightCount, lightCount);

    for (size_t i = 0; i < lightCount; i++) {
        spotLight[i].UseLight(uniformSpotLight[i].uniformAmbientIntensity, uniformSpotLight[i].uniformColour,
            uniformSpotLight[i].uniformDiffuseIntensity, uniformSpotLight[i].uniformPosition, uniformSpotLight[i].uniformDirection,
            uniformSpotLight[i].uniformConstant, uniformSpotLight[i].uniformLinear, uniformSpotLight[i].uniformExponent, uniformSpotLight[i].uniformEdge);
        
        spotLight[i].GetShadowmap()->Read(GL_TEXTURE0 + textureUnit + i);
        glUniform1i(uniformOmniShadowmap[i + offset].shadowMap, textureUnit + i);
        glUniform1f(uniformOmniShadowmap[i + offset].farPlane, spotLight[i].GetFarPlane());
    }
}

void Shader::SetTexture(GLuint textureUnit) {
    glUniform1i(uniformTexture, textureUnit);
}

void Shader::SetDirectionalShadowMap(GLuint textureUnit) {
    glUniform1i(uniformDirectionalShadowMap, textureUnit);
}

void Shader::SetDirectionalLightTransform(glm::mat4* lightTransform) {
    glUniformMatrix4fv(uniformDirectionalLightTransform, 1, GL_FALSE, glm::value_ptr(*lightTransform));
}

void Shader::SetOmniLightMatrices(std::vector<glm::mat4> lightMatrices)
{
    for (size_t i = 0; i < 6; i++) {
        glUniformMatrix4fv(uniformLightMatrices[i], 1, GL_FALSE, glm::value_ptr(lightMatrices[i]));
    }
}

void Shader::UseShader() {
	glUseProgram(shaderID);
}

void Shader::ClearShader() {
	if (shaderID != 0) {
		glDeleteProgram(shaderID);
		shaderID = 0;
	}

	uniformModel = 0;
	uniformProjection = 0;
}

void Shader::CompileShader(const char* vertexCode, const char* fragmentCode) {
    shaderID = glCreateProgram();

    if (!shaderID) {
        printf("Error creating shader program...\n");
        return;
    }

    AddShader(shaderID, vertexCode, GL_VERTEX_SHADER);
    AddShader(shaderID, fragmentCode, GL_FRAGMENT_SHADER);

    CompileProgram();
}

void Shader::CompileShader(const char* vertexCode, const char* geometryCode, const char* fragmentCode)
{
    shaderID = glCreateProgram();

    if (!shaderID) {
        printf("Error creating shader program...\n");
        return;
    }

    AddShader(shaderID, vertexCode, GL_VERTEX_SHADER);
    AddShader(shaderID, geometryCode, GL_GEOMETRY_SHADER);
    AddShader(shaderID, fragmentCode, GL_FRAGMENT_SHADER);

    CompileProgram();
}

void Shader::Validate()
{
    GLint result = 0;
    GLchar errorLog[1024] = { 0 };

    // Validate the program and get log results
    glValidateProgram(shaderID);
    glGetProgramiv(shaderID, GL_VALIDATE_STATUS, &result);
    if (!result) {
        glGetProgramInfoLog(shaderID, sizeof(errorLog), NULL, errorLog);
        printf("Error validating program: '%s'\n", errorLog);
        return;
    }
}


void Shader::CompileProgram()
{
    GLint result = 0;
    GLchar errorLog[1024] = { 0 };

    // Link the program and get log results
    glLinkProgram(shaderID);
    glGetProgramiv(shaderID, GL_LINK_STATUS, &result);
    if (!result) {
        glGetProgramInfoLog(shaderID, sizeof(errorLog), NULL, errorLog);
        printf("Error linking program: '%s'\n", errorLog);
        return;
    }

    uniformModel = glGetUniformLocation(shaderID, "model");
    uniformProjection = glGetUniformLocation(shaderID, "projection");
    uniformView = glGetUniformLocation(shaderID, "view");

    // Directional Light
    uniformDirectionalLight.uniformColour = glGetUniformLocation(shaderID, "directionalLight.base.colour");
    uniformDirectionalLight.uniformAmbientIntensity = glGetUniformLocation(shaderID, "directionalLight.base.ambientIntensity");
    uniformDirectionalLight.uniformDiffuseIntensity = glGetUniformLocation(shaderID, "directionalLight.base.diffuseIntensity");

    uniformDirectionalLight.uniformDirection = glGetUniformLocation(shaderID, "directionalLight.direction");

    // Material
    uniformCameraPosition = glGetUniformLocation(shaderID, "cameraPosition");
    uniformSpecularIntensity = glGetUniformLocation(shaderID, "material.specularIntensity");
    uniformShininess = glGetUniformLocation(shaderID, "material.shininess");

    // Point Lights
    uniformPointLightCount = glGetUniformLocation(shaderID, "pointLightCount");

    for (size_t i = 0; i < MAX_POINT_LIGHTS; i++) {
        char locationBuffer[100] = { '\0' };
        snprintf(locationBuffer, sizeof(locationBuffer), "pointLights[%d].base.colour", i);
        uniformPointLight[i].uniformColour = glGetUniformLocation(shaderID, locationBuffer);

        snprintf(locationBuffer, sizeof(locationBuffer), "pointLights[%d].base.ambientIntensity", i);
        uniformPointLight[i].uniformAmbientIntensity = glGetUniformLocation(shaderID, locationBuffer);

        snprintf(locationBuffer, sizeof(locationBuffer), "pointLights[%d].base.diffuseIntensity", i);
        uniformPointLight[i].uniformDiffuseIntensity = glGetUniformLocation(shaderID, locationBuffer);

        snprintf(locationBuffer, sizeof(locationBuffer), "pointLights[%d].position", i);
        uniformPointLight[i].uniformPosition = glGetUniformLocation(shaderID, locationBuffer);

        snprintf(locationBuffer, sizeof(locationBuffer), "pointLights[%d].constant", i);
        uniformPointLight[i].uniformConstant = glGetUniformLocation(shaderID, locationBuffer);

        snprintf(locationBuffer, sizeof(locationBuffer), "pointLights[%d].linear", i);
        uniformPointLight[i].uniformLinear = glGetUniformLocation(shaderID, locationBuffer);

        snprintf(locationBuffer, sizeof(locationBuffer), "pointLights[%d].exponent", i);
        uniformPointLight[i].uniformExponent = glGetUniformLocation(shaderID, locationBuffer);
    }

    // Spot Light
    uniformSpotLightCount = glGetUniformLocation(shaderID, "spotLightCount");

    for (size_t i = 0; i < MAX_SPOT_LIGHTS; i++) {
        char locationBuffer[100] = { '\0' };
        snprintf(locationBuffer, sizeof(locationBuffer), "spotLights[%d].base.base.colour", i);
        uniformSpotLight[i].uniformColour = glGetUniformLocation(shaderID, locationBuffer);

        snprintf(locationBuffer, sizeof(locationBuffer), "spotLights[%d].base.base.ambientIntensity", i);
        uniformSpotLight[i].uniformAmbientIntensity = glGetUniformLocation(shaderID, locationBuffer);

        snprintf(locationBuffer, sizeof(locationBuffer), "spotLights[%d].base.base.diffuseIntensity", i);
        uniformSpotLight[i].uniformDiffuseIntensity = glGetUniformLocation(shaderID, locationBuffer);

        snprintf(locationBuffer, sizeof(locationBuffer), "spotLights[%d].base.position", i);
        uniformSpotLight[i].uniformPosition = glGetUniformLocation(shaderID, locationBuffer);

        snprintf(locationBuffer, sizeof(locationBuffer), "spotLights[%d].base.constant", i);
        uniformSpotLight[i].uniformConstant = glGetUniformLocation(shaderID, locationBuffer);

        snprintf(locationBuffer, sizeof(locationBuffer), "spotLights[%d].base.linear", i);
        uniformSpotLight[i].uniformLinear = glGetUniformLocation(shaderID, locationBuffer);

        snprintf(locationBuffer, sizeof(locationBuffer), "spotLights[%d].base.exponent", i);
        uniformSpotLight[i].uniformExponent = glGetUniformLocation(shaderID, locationBuffer);

        snprintf(locationBuffer, sizeof(locationBuffer), "spotLights[%d].direction", i);
        uniformSpotLight[i].uniformDirection = glGetUniformLocation(shaderID, locationBuffer);

        snprintf(locationBuffer, sizeof(locationBuffer), "spotLights[%d].edge", i);
        uniformSpotLight[i].uniformEdge = glGetUniformLocation(shaderID, locationBuffer);
    }

    uniformTexture = glGetUniformLocation(shaderID, "theTexture");
    uniformDirectionalLightTransform = glGetUniformLocation(shaderID, "directionalLightTransform");
    uniformDirectionalShadowMap = glGetUniformLocation(shaderID, "directionalShadowMap");

    uniformOmniLightPos = glGetUniformLocation(shaderID, "lightPos");
    uniformFarPlane = glGetUniformLocation(shaderID, "farPlane");

    for (size_t i = 0; i < 6; i++) {
        char locationBuffer[100] = { '\0' };
        snprintf(locationBuffer, sizeof(locationBuffer), "lightMatrices[%d]", i);
        uniformLightMatrices[i] = glGetUniformLocation(shaderID, locationBuffer);
    }

    for (size_t i = 0; i < MAX_POINT_LIGHTS + MAX_SPOT_LIGHTS; i++) {
        char locationBuffer[100] = { '\0' };
        snprintf(locationBuffer, sizeof(locationBuffer), "omniShadowMaps[%d].shadowMap", i);
        uniformOmniShadowmap[i].shadowMap = glGetUniformLocation(shaderID, locationBuffer);

        snprintf(locationBuffer, sizeof(locationBuffer), "omniShadowMaps[%d].farPlane", i);
        uniformOmniShadowmap[i].farPlane = glGetUniformLocation(shaderID, locationBuffer);
    }
}

void Shader::AddShader(GLuint program, const char* shaderCode, GLenum shaderType) {
    GLuint mainShader = glCreateShader(shaderType);
    const GLchar* glShaderCode[1];
    glShaderCode[0] = shaderCode;

    GLint codeLength[1];
    codeLength[0] = strlen(shaderCode);

    // Compile the shader
    glShaderSource(mainShader, 1, glShaderCode, codeLength);
    glCompileShader(mainShader);

    GLint result = 0;
    GLchar errorLog[1024] = { 0 };

    // Get the compile log result
    glGetShaderiv(mainShader, GL_COMPILE_STATUS, &result);
    if (!result) {
        glGetProgramInfoLog(mainShader, sizeof(errorLog), NULL, errorLog);
        printf("Error compiling the %d shader: '%s'\n", shaderType, errorLog);
        return;
    }

    // Supply the generated shader to the shader program object
    glAttachShader(program, mainShader);
}



Shader::~Shader() {
	ClearShader();
}
