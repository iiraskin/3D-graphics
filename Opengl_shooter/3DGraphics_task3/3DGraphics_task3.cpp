// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <random>
#include <vector>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

#include <GL/glut.h> 

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <common/shader.hpp>
#include <common/controls.hpp>
#include "common/text2D.hpp" 
#include "common/texture.hpp" 

#include "models.hpp"


int main( void )
{
	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow( 1024, 768, "Spruce", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS); 

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	//GLuint programID = LoadShaders("TransformVertexShader.vertexshader", "ColorFragmentShader.fragmentshader");
	GLuint programID1 = LoadShaders("textureVertex.vertexshader", "textureFragment.fragmentshader");

	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID1, "MVP");
	
	GLuint vertexbuffer;
	GLuint colorbuffer;
	GLuint uvbuffer;

	glGenBuffers(1, &vertexbuffer);
	glGenBuffers(1, &colorbuffer);
	glGenBuffers(1, &uvbuffer);

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(0, 10000);

	MakeSpruce tree_maker;
	MakeFireboll fireboll_maker;
	MakeStump stump_maker;

	std::vector<Spruce> trees;
	std::vector<Stump> stumps;
	std::vector<Fireboll> firebolls;

	// Âîçìîæíûå öâåòà ôîíà
	std::vector<glm::vec4> backgrounds = { {0.7f, 1.0f, 0.7f, 0.0f},
										   {0.7f, 0.7f, 1.0f, 0.0f},
										   {1.0f, 0.7f, 0.7f, 0.0f},
										   {0.0f, 0.0f, 0.0f, 0.0f},
										   {1.0f, 1.0f, 1.0f, 0.0f} };

	double interval = dis(gen) / 10000.0 * 10 + 5; // Çàäåðæêà äî ïîÿâëåíèÿ ïåðâîãî äåðåâà
	double start = glfwGetTime();
	int old_mause_state = GLFW_RELEASE;
	int old_background_state = GLFW_RELEASE;
	int backgorund_id = 0;
	glClearColor(backgrounds[backgorund_id][0], backgrounds[backgorund_id][1], backgrounds[backgorund_id][2], backgrounds[backgorund_id][3]);

	//load Texture
	GLuint Texture = loadBMP_custom("fireearth.bmp");
	GLuint TextureID = glGetUniformLocation(programID1, "myTextureSampler");
	
	do{
		// Èçìåíåíèå ôîíà ïî íàæàòèþ "B"
		int new_background_state = glfwGetKey(window, GLFW_KEY_B);
		if (new_background_state == GLFW_RELEASE && old_background_state == GLFW_PRESS) {
			backgorund_id = (backgorund_id + 1) % backgrounds.size();
			glClearColor(backgrounds[backgorund_id][0], backgrounds[backgorund_id][1], backgrounds[backgorund_id][2], backgrounds[backgorund_id][3]);
		}
		old_background_state = new_background_state;

		// Óäàëåíèå äåðåâüåâ, â êîòîðûå ïîïàëè ôàéåðáîëû
		for (int i = 0; i < firebolls.size(); ++i) {
			for (int j = 0; j < trees.size(); ++j) {
				// Ó÷èòûâàåò, ÷òî ôàåðáîë äâèãàåòñÿ
				double delta_time = glfwGetTime() - firebolls[i].birth;
				glm::vec3 boll_centre = firebolls[i].centre + firebolls[i].direction * (float)delta_time * firebolls[i].speed;

				double distance = std::sqrt(std::pow(boll_centre.x - trees[j].centre.x, 2) +
									  	    std::pow(boll_centre.y - trees[j].centre.y, 2) +
										    std::pow(boll_centre.z - trees[j].centre.z, 2));
				if (distance < firebolls[i].radius + trees[j].radius) {
					// Ñîçäàíèå ïíÿ
					stumps.resize(stumps.size() + 1);
					stump_maker.add_tree(&stumps[stumps.size() - 1], trees[j]);
					// Óäàëåíèå äåðåâà
					std::swap(trees[j], trees[trees.size() - 1]);
					trees.pop_back();
					// Óäàëåíèå ôàéåðáîëà
					std::swap(firebolls[i], firebolls[firebolls.size() - 1]);
					firebolls.pop_back();
					break;
				}
			}
		}
		
		// Óäàëåíèå ñòàðûõ ïíåé (ñðîê æèçíè ïíÿ - 2 ìèíóòû)
		for (int i = 0; i < stumps.size(); ++i) {
			double cur_time = glfwGetTime();
			if (cur_time - stumps[i].birth > 120) {
				std::swap(stumps[i], stumps[stumps.size() - 1]);
				stumps.pop_back();
			}
		}
		
		// Äîáàâëåíèå íîâûõ ôàéåðáîëîâ
		int new_mause_state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
		if (new_mause_state == GLFW_RELEASE && old_mause_state == GLFW_PRESS) {
			firebolls.resize(firebolls.size() + 1);
			fireboll_maker.add_boll(getPosition(), getDirection(), &firebolls[firebolls.size() - 1]);
		}
		old_mause_state = new_mause_state;

		// Äîáàâëåíèå íîâûõ äåðåâüåâ.
		double end = glfwGetTime();
		if (end - start > interval) {
			trees.resize(trees.size() + 1);
			tree_maker.add_tree(&trees[trees.size() - 1]);
			interval = dis(gen) / 10000.0 * 5;
			start = glfwGetTime();
		}

		std::vector<GLfloat> g_vertex_buffer_data = {};
		std::vector<GLfloat> g_color_buffer_data = {};
		std::vector<GLfloat> g_uv_buffer_data = {};

		// Äîáàâëåíèå âñåõ îáúåêòîâ â áóôåðû.

		for (Fireboll fireboll : firebolls) {
			// Íóæíî ó÷åñòü, ÷òî ôàåðáîëû äâèãàþòñÿ.
			Fireboll fireboll_copy = fireboll;
			fireboll_maker.find_position(&fireboll_copy);
			g_vertex_buffer_data.insert(g_vertex_buffer_data.end(), fireboll_copy.boll.begin(), fireboll_copy.boll.end());
			g_color_buffer_data.insert(g_color_buffer_data.end(), fireboll_copy.boll_colors.begin(), fireboll_copy.boll_colors.end());
			g_uv_buffer_data.insert(g_uv_buffer_data.end(), fireboll_copy.uves.begin(), fireboll_copy.uves.end());
		}

		for (Spruce tree : trees) {
			g_vertex_buffer_data.insert(g_vertex_buffer_data.end(), tree.tree.begin(), tree.tree.end());
			g_color_buffer_data.insert(g_color_buffer_data.end(), tree.tree_colors.begin(), tree.tree_colors.end());
			g_uv_buffer_data.insert(g_uv_buffer_data.end(), tree.uves.begin(), tree.uves.end());
		}

		for (Stump stump : stumps) {
			g_vertex_buffer_data.insert(g_vertex_buffer_data.end(), stump.tree.begin(), stump.tree.end());
			g_color_buffer_data.insert(g_color_buffer_data.end(), stump.tree_colors.begin(), stump.tree_colors.end());
			g_uv_buffer_data.insert(g_uv_buffer_data.end(), stump.uves.begin(), stump.uves.end());
		}

		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glBufferData(GL_ARRAY_BUFFER, g_vertex_buffer_data.size() * 4, g_vertex_buffer_data.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
		glBufferData(GL_ARRAY_BUFFER, g_color_buffer_data.size() * 4, g_color_buffer_data.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
		glBufferData(GL_ARRAY_BUFFER, g_uv_buffer_data.size() * 4, g_uv_buffer_data.data(), GL_STATIC_DRAW);

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID1);
		
		// Compute the MVP matrix from keyboard and mouse input
		computeMatricesFromInputs();

		glm::mat4 ProjectionMatrix = getProjectionMatrix();
		glm::mat4 ViewMatrix = getViewMatrix();
		glm::mat4 ModelMatrix = glm::mat4(1.0);
		glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
		
		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);
		glUniform1i(TextureID, 0);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// 2nd attribute buffer : UVs
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
		glVertexAttribPointer(
			1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			2,                                // size : U+V => 2
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// 3rd attribute : colors
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
		glVertexAttribPointer(
			2,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, g_vertex_buffer_data.size() / 3);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &colorbuffer);
	glDeleteBuffers(1, &uvbuffer);
	glDeleteProgram(programID1);
	glDeleteTextures(1, &Texture);
	glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

