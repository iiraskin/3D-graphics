// Include standard headers
#include <stdio.h>
#include <stdlib.h>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <common/shader.hpp>

const double PI = 3.141592653589793238463;

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
	window = glfwCreateWindow( 1024, 768, "Funny man", NULL, NULL);
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

	// Dark blue background
	glClearColor(0.3f, 0.8f, 0.0f, 0.0f);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	GLuint programID1 = LoadShaders( "SimpleVertexShader_2.vertexshader", "SimpleFragmentShader1.fragmentshader" );

	// Triangles with second color
	GLuint programID2 = LoadShaders("SimpleVertexShader_2.vertexshader", "SimpleFragmentShader2.fragmentshader");
	// Triangles with third color
	GLuint programID3 = LoadShaders("SimpleVertexShader_2.vertexshader", "SimpleFragmentShader3.fragmentshader");
	GLuint programID4 = LoadShaders("SimpleVertexShader_2.vertexshader", "SimpleFragmentShader4.fragmentshader");
	GLuint programID5 = LoadShaders("SimpleVertexShader_2.vertexshader", "SimpleFragmentShader5.fragmentshader");

	// Get a handle for our "MVP" uniform
	GLuint MatrixID1 = glGetUniformLocation(programID1, "MVP");
	GLuint MatrixID2 = glGetUniformLocation(programID2, "MVP");
	GLuint MatrixID3 = glGetUniformLocation(programID3, "MVP");
	GLuint MatrixID4 = glGetUniformLocation(programID4, "MVP");
	GLuint MatrixID5 = glGetUniformLocation(programID5, "MVP");

	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);

	static const GLfloat g_vertex_buffer_data[] = {
		// Body
		-0.3f, 0.6f, 0.0f,
		 0.3f, 0.6f, 0.0f,
		 0.0f,  -0.2f, 0.0f,

		-0.25f, -0.2f, 0.0f,
		 0.25f, -0.2f, 0.0f,
		 0.0f,  0.3f, 0.0f,
		 // Head
		 -0.15f, 0.7f, 0.0f,
		 0.15f, 0.7f, 0.0f,
		 0.0f,  0.5f, 0.0f,

		 -0.15f, 0.7f, 0.0f,
		 0.15f, 0.7f, 0.0f,
		 0.0f,  0.9f, 0.0f,
		 // Left hand
		 -0.3f, 0.6f, 0.0f,
		 -0.2f, 0.5f, 0.0f,
		 -0.5f,  0.2f, 0.0f,

		 -0.7f, 0.5f, 0.0f,
		 -0.6f, 0.6f, 0.0f,
		 -0.5f,  0.2f, 0.0f,
		 // Right hand
		 0.3f, 0.6f, 0.0f,
		 0.2f, 0.5f, 0.0f,
		 0.5f,  0.2f, 0.0f,

		 0.3f, -0.1f, 0.0f,
		 0.4f,  -0.2f, 0.0f,
		 0.5f,  0.2f, 0.0f,
		 // Left leg
		 -0.2f, -0.1f, 0.0f,
		 0.0f, -0.2f, 0.0f,
		 -0.4f,  -0.5f, 0.0f,

		 -0.5f, -0.9f, 0.0f,
		 -0.3f,  -0.9f, 0.0f,
		 -0.4f,  -0.5f, 0.0f,
		 // Right leg
		 0.2f, -0.1f, 0.0f,
		 0.0f, -0.2f, 0.0f,
		 0.4f,  -0.5f, 0.0f,

		 0.5f, -0.9f, 0.0f,
		 0.3f,  -0.9f, 0.0f,
		 0.4f,  -0.5f, 0.0f,
		 // Face
		 0.0f, 0.6f, 0.0f,
		 -0.075f, 0.63f, 0.0f,
		 0.075f,  0.63f, 0.0f,

		 -0.02f, 0.73f, 0.0f,
		 -0.08f,  0.73f, 0.0f,
		 -0.05f,  0.77f, 0.0f,

		 0.02f, 0.73f, 0.0f,
		 0.08f,  0.73f, 0.0f,
		 0.05f,  0.77f, 0.0f,

		 -0.05f, 0.73f, 0.0f,
		 -0.065f,  0.75f, 0.0f,
		 -0.035f,  0.75f, 0.0f,

		 0.05f, 0.73f, 0.0f,
		 0.065f,  0.75f, 0.0f,
		 0.035f,  0.75f, 0.0f,
	};

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	float position = 0; // Number of position of camera.

	do{

		// Clear the screen
		glClear( GL_COLOR_BUFFER_BIT );

		// Camera matrix
		glm::mat4 View = glm::lookAt(
			glm::vec3(3, 2 + cos(position), 2 + sin(position)),
			glm::vec3(0, 0, 0), // and looks at the origin
			glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
		);

		position += 0.001;
		if (position >= 2 * PI)
			position -= 2 * PI;

		glm::mat4 Model = glm::mat4(1.0f);
		glm::mat4 MVP = Projection * View * Model;

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// Draw the triangle !
		glUniformMatrix4fv(MatrixID1, 1, GL_FALSE, &MVP[0][0]);
		glUseProgram(programID1);
		glDrawArrays(GL_TRIANGLES, 0, 6); // 3 indices starting at 0 -> 1 triangle

		glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
		glUseProgram(programID2);
		glDrawArrays(GL_TRIANGLES, 6, 6);

		glUniformMatrix4fv(MatrixID3, 1, GL_FALSE, &MVP[0][0]);
		glUseProgram(programID3);
		glDrawArrays(GL_TRIANGLES, 12, 24);

		glUniformMatrix4fv(MatrixID4, 1, GL_FALSE, &MVP[0][0]);
		glUseProgram(programID4);
		glDrawArrays(GL_TRIANGLES, 36, 9);

		glUniformMatrix4fv(MatrixID5, 1, GL_FALSE, &MVP[0][0]);
		glUseProgram(programID5);
		glDrawArrays(GL_TRIANGLES, 45, 6);

		glDisableVertexAttribArray(0);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

	// Cleanup VBO
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteVertexArrays(1, &VertexArrayID);
	glDeleteProgram(programID1);
	glDeleteProgram(programID2);
	glDeleteProgram(programID3);
	glDeleteProgram(programID4);
	glDeleteProgram(programID5);
	
	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}
