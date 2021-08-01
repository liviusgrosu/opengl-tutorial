#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>

#include <GL\glew.h>
#include <GLFW\glfw3.h>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include "CommonValues.h"

#include "GLWindow.h"
#include "Mesh.h"
#include "Shader.h"
#include "Camera.h"
#include "Texture.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
#include "Model.h"

const float toRadians = 3.14159265f / 180.0f;


GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformCameraPosition = 0, uniformSpecularIntensity = 0, uniformShininess = 0;

GLWindow mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;
Shader directionalShadowShader;
Camera mainCamera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;

Material shinyMaterial;
Material dullMaterial;

Model uh60;
GLfloat uh60Angle = 0.0f;

DirectionalLight mainLight;
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

unsigned int pointLightCount = 0;
unsigned int spotLightCount = 0;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

// Vertex Shader
static const char* vShader = "Shaders/shader.vert";

// Fragment Shader
static const char* fShader = "Shaders/shader.frag";

void CalculateAverageNormals(unsigned int *indices, unsigned int indiceCount, GLfloat *vertices, 
								unsigned int verticesDataLength, unsigned int vertexDataLength, unsigned int normalOffset) {
	for (size_t i = 0; i < indiceCount; i += 3) {
		unsigned int in0 = indices[i] * vertexDataLength;
		unsigned int in1 = indices[i + 1] * vertexDataLength;
		unsigned int in2 = indices[i + 2] * vertexDataLength;
		
		// Calculate lines of the triangle to get the normal
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		// Jump straight to the normal data in vertices
		in0 += normalOffset;
		in1 += normalOffset;
		in2 += normalOffset;

		// Provide the normal data
		vertices[in0] += normal.x;
		vertices[in0 + 1] += normal.y;
		vertices[in0 + 2] += normal.z;

		vertices[in1] += normal.x;
		vertices[in1 + 1] += normal.y;
		vertices[in1 + 2] += normal.z;

		vertices[in2] += normal.x;
		vertices[in2 + 1] += normal.y;
		vertices[in2 + 2] += normal.z;
	}

	// Go through all the vertices and normalize again
	// We do this because indices can have more then 1 vertex and we have added them together to get their normal
	// We need to normalize that sum
	for (size_t i = 0; i < verticesDataLength / vertexDataLength; i++) {
		unsigned int nOffset = i * vertexDataLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x;
		vertices[nOffset + 1] = vec.y;
		vertices[nOffset + 2] = vec.z;
	}
}

void CreateObjects() 
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x		y		z		u		v		nx		ny		nz
			-1.0f,	-1.0f,	-0.6f,	0.0f,	0.0f,	0.0f,	0.0f,	0.0f,
			0.0f,	-1.0f,	1.0f,	0.5f,	0.0f,	0.0f,	0.0f,	0.0f,
			1.0f,	-1.0f,	-0.6f,	1.0f,	0.0f,	0.0f,	0.0f,	0.0f,
			0.0f,	1.0f,	0.0f,	0.5f,	1.0f,	0.0f,	0.0f,	0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3,
	};

	GLfloat floorVertices[] = {
		//	x		y		z		u		v		nx		ny		nz
			-10.0f, 0.0f,	-10.0f, 0.0f,	0.0f,	0.0f,	-1.0f,	0.0f,
			10.0f,	0.0f,	-10.0f, 10.0f,	0.0f,	0.0f,	-1.0f,	0.0f,
			-10.0f, 0.0f,	10.0f,	0.0f,	10.0f,	0.0f,	-1.0f,	0.0f,
			10.0f,	0.0f,	10.0f,	10.0f,	10.0f,	0.0f,	-1.0f,	0.0f
	};

	CalculateAverageNormals(indices, 12, vertices, 32, 8, 5);

	Mesh *obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh *obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh* obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);
}

void CreateShaders()
{
	Shader *shader1 = new Shader();
	// Create the shaders from files and store into a list
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);

	directionalShadowShader = Shader();
	directionalShadowShader.CreateFromFiles("Shaders/directional_shadow_map.vert", "Shaders/directional_shadow_map.frag");
}

void RenderScene() {
	glm::mat4 model(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.5f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	brickTexture.UseTexture();
	shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	meshList[0]->RenderMesh();

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(3.0f, 0.0f, -2.5f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	dirtTexture.UseTexture();
	dullMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	meshList[1]->RenderMesh();

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	plainTexture.UseTexture();
	shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	meshList[2]->RenderMesh();

	uh60Angle += 0.01f;
	if (uh60Angle > 360.0f) {
		uh60Angle = 0.1f;
	}

	model = glm::mat4(1.0f);
	model = glm::rotate(model, -uh60Angle * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::translate(model, glm::vec3(-8.0f, glm::sin(uh60Angle / 45.0f), 3.0f));
	model = glm::rotate(model, -20.0f * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::rotate(model, -90.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	uh60.RenderModel();
}

void DirectionalShadowMapPass(DirectionalLight* light) {
	// Switch to the directional light shader
	directionalShadowShader.UseShader();

	glViewport(0, 0, light->GetShadowmap()->GetShadowWidth(), light->GetShadowmap()->GetShadowHeight());

	light->GetShadowmap()->Write();
	glClear(GL_DEPTH_BUFFER_BIT);

	uniformModel = directionalShadowShader.GetModelLocation();
	directionalShadowShader.SetDirectionalLightTransform(&light->CalculateLightTransform());

	RenderScene();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderPass(glm::mat4 projectionMatrix, glm::mat4 viewMatrix) {
	shaderList[0].UseShader();

	uniformModel = shaderList[0].GetModelLocation();
	uniformProjection = shaderList[0].GetProjectionLocation();
	uniformView = shaderList[0].GetViewLocation();

	uniformCameraPosition = shaderList[0].GetCameraPositionLocation();
	uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
	uniformShininess = shaderList[0].GetShininessLocation();

	glViewport(0, 0, 1366, 768);

	// Clear the screen
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniform3f(uniformCameraPosition, mainCamera.GetCameraPosition().x, mainCamera.GetCameraPosition().y, mainCamera.GetCameraPosition().z);

	shaderList[0].SetDirectionalLight(&mainLight);
	shaderList[0].SetPointLights(pointLights, pointLightCount);
	shaderList[0].SetSpotLights(spotLights, spotLightCount);
	shaderList[0].SetDirectionalLightTransform(&mainLight.CalculateLightTransform());

	mainLight.GetShadowmap()->Read(GL_TEXTURE1);
	// Telling the shaders where the texture units will live
	shaderList[0].SetTexture(0);
	shaderList[0].SetDirectionalShadowMap(1);

	// Lower the light of the flashlight
	glm::vec3 lowerLight = mainCamera.GetCameraPosition();
	lowerLight.y -= 0.3f;

	// Update flashlight position and rotation
	//spotLights[0].SetFlash(lowerLight, mainCamera.GetCameraDirection());

	RenderScene();
}

int main() 
{
	mainWindow = GLWindow(1366, 768);
	mainWindow.Initialize();

	CreateObjects();
	CreateShaders();

	mainCamera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 5.0f, 0.5f);

	brickTexture = Texture((char*)"Textures/brick.png");
	brickTexture.LoadTextureWithAlpha();
	dirtTexture = Texture((char*)"Textures/dirt.png");
	dirtTexture.LoadTextureWithAlpha();
	plainTexture = Texture((char*)"Textures/dirt.png");
	plainTexture.LoadTextureWithAlpha();

	shinyMaterial = Material(4.0f, 32);
	dullMaterial = Material(0.3f, 4);

	uh60 = Model();
	uh60.LoadModel("Models/uh60.obj");

	mainLight = DirectionalLight(2048, 2048,
									1.0f, 1.0f, 1.0f, 
									0.1f, 0.3f, 
									0.0f, -10.0f, -15.0f);



	pointLights[0] = PointLight(0.0f, 0.0f, 1.0f,
								0.1f, 0.1f,
								4.0f, 0.0f, 0.0f,
								0.3f, 0.2f, 0.1f);
	pointLightCount++;

	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
								0.0f, 0.4f,
								0.0f, 0.0f, 0.0f,
								0.0f, -1.0f, 0.0f,
								0.3f, 0.0f, 0.0f,
								20.0f);
	spotLightCount++;

	spotLights[1] = SpotLight(1.0f, 1.0f, 1.0f,
								0.0f, 1.0f,
								0.0f, -1.5f, 0.0f,
								-100.0f, -1.0f, 0.0f,
								1.0f, 0.0f, 0.0f,
								55.0f);

	spotLightCount++;

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformCameraPosition = 0, uniformSpecularIntensity = 0, uniformShininess = 0;
	
	// Create the projection perspective for the camera
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (GLfloat)mainWindow.GetBufferWidth() / mainWindow.GetBufferHeight(), 0.1f, 100.0f);

	while (!mainWindow.GetShouldClose()) {
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		lastTime = now;

		glfwPollEvents();

		// Get user inputs
		mainCamera.keyControl(mainWindow.GetKeys(), deltaTime);
		mainCamera.mouseControl(mainWindow.GetXChange(), mainWindow.GetYChange());
		
		DirectionalShadowMapPass(&mainLight);
		RenderPass(projection, mainCamera.CalculateViewMatrix());

		glUseProgram(0);

		mainWindow.SwapBuffers();
	}

	return 0;
}