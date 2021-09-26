#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";
const char *fragmentShader1Source = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "uniform vec4 ourColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = ourColor;\n"
    "}\n\0";

void drawTriangle(unsigned int shaderProgram,unsigned int VAOs[],int index, int vertexColorLocation, float r, float g, float b, float a,int flag){
    // draw triangle in while loop
    glUseProgram(shaderProgram);
    glBindVertexArray(VAOs[index]);
    glUniform4f(vertexColorLocation, r, g, b, a);
    if(flag==0)glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // set drawing to line
    else glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void setUpTriangle(unsigned int VAOs[], unsigned int VBOs[], int index,float triangle[]){
    glBindVertexArray(VAOs[index]);    // note that we bind to a different VAO now
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[index]);    // and a different VBO
//    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, 36, triangle, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0); // because the vertex data is tightly packed we can also specify 0 as the vertex attribute's stride to let OpenGL figure it out
    glEnableVertexAttribArray(0);
}

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    // build and compile our shader program
    // ------------------------------------
    // we skipped compile log checks this time for readability (if you do encounter issues, add the compile-checks! see previous code samples)
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    unsigned int fragmentShaderOrange = glCreateShader(GL_FRAGMENT_SHADER); // the first fragment shader that outputs the color orange
    unsigned int fragmentShaderYellow = glCreateShader(GL_FRAGMENT_SHADER); // the second fragment shader that outputs the color yellow




    unsigned int shaderProgramOrange = glCreateProgram();
    unsigned int shaderProgramYellow = glCreateProgram(); // the second shader program


    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }


    glShaderSource(fragmentShaderOrange, 1, &fragmentShader1Source, NULL); //?
    glCompileShader(fragmentShaderOrange);
    glCompileShader(fragmentShaderYellow);


    // link the first program object
    glAttachShader(shaderProgramOrange, vertexShader);
    glAttachShader(shaderProgramOrange, fragmentShaderOrange);
    glLinkProgram(shaderProgramOrange);
    // then link the second program object using a different fragment shader (but same vertex shader)
    // this is perfectly allowed since the inputs and outputs of both the vertex and fragment shaders are equally matched.
    glAttachShader(shaderProgramYellow, vertexShader);
    glAttachShader(shaderProgramYellow, fragmentShaderYellow);
    glLinkProgram(shaderProgramYellow);




    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    
    
    
    float firstTriangle[] = {
        -0.45f, -0.3897f, 0.0f,  // left
        0.45f,  -0.3897f, 0.0f,  // right
        0.0f, 0.3897f, 0.0f   // top
    };
    float secondTriangle1[] = {
        0.05f, -0.33197, 0.0f,  // left
        0.25f,  -0.3897, 0.0f,  // right
        0.15f, 0.01445, 0.0f   // top
    };
    float secondTriangle2[] = {
        -0.35f, -0.2165, 0.0f,  // left
        -0.2,  -0.07216, 0.0f,  // right
        0.05f, -0.331965, 0.0f   // top
    };
    float secondTriangle3[] = {
        -0.2,  -0.07216, 0.0f,  // left
        0.15f, 0.01445, 0.0f,  // right
        0.1, 0.2165, 0.0f   // top
    };
    float thirdTriangle[] = {
        -0.03333,  -0.24536, 0.0f,
        0.1167, -0.101, 0.0f,
        -0.0833,  -0.0433, 0.0f,
    };
        


    unsigned int VBOs[9], VAOs[9];
    glGenVertexArrays(9, VAOs); // we can also generate multiple VAOs or buffers at the same time
    glGenBuffers(9, VBOs);
    // first triangle setup
    // --------------------
    setUpTriangle(VAOs, VBOs, 0, firstTriangle);
    // glBindVertexArray(0); // no need to unbind at all as we directly bind a different VAO the next few lines
    // second triangle setup
    // ---------------------
    setUpTriangle(VAOs, VBOs, 1, secondTriangle1);
    
    // third
    setUpTriangle(VAOs, VBOs, 2, secondTriangle2);

    // fourth

    setUpTriangle(VAOs, VBOs, 3, secondTriangle3);

    setUpTriangle(VAOs, VBOs, 4, thirdTriangle);


    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {

        float timeValue = glfwGetTime();
        float greenValue = sin(timeValue) / 2.0f + 0.5f;
        float greenValue2= sin(timeValue+0.5) / 2.0f + 0.5f;
        float greenValue3= sin(timeValue+1) / 2.0f + 0.5f;
//        float greenValue = sin(timeValue);
//        float greenValue2= sin(timeValue+0.4);
//        float greenValue3= sin(timeValue+0.8) ;
        cout<<"timeValue:"<<timeValue<<endl;
        // input
        // -----
        processInput(window);
        glClearColor(0.15f, 0.1f, 0.2f+greenValue/4, 0.5f);
        glClear(GL_COLOR_BUFFER_BIT);
        int vertexColorLocation = glGetUniformLocation(shaderProgramOrange, "ourColor");
        glm::mat4 transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        transform = glm::translate(transform, glm::vec3(0.5f, -0.5f, 0.0f));
        transform = glm::rotate(transform, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
        
        // render the first trriangle
        // ------
        // now when we draw the triangle we first use the vertex and orange fragment shader from the first program
//        glUseProgram(shaderProgramOrange);
//        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);// set drawing to fill
//
//        // draw the first triangle using the data from our first VAO
//        glBindVertexArray(VAOs[0]);
//        glUniform4f(vertexColorLocation, greenValue, 0.6, 0.8f, 1.0f);
//        glDrawArrays(GL_TRIANGLES, 0, 3);    // this call should output an orange triangle
        drawTriangle(shaderProgramOrange, VAOs,0, vertexColorLocation, greenValue, 0.6, 0.8, 1.0, 1);

        // when we draw the second triangle we want to use a different shader program so we switch to the shader program with our yellow fragment shader.
        drawTriangle(shaderProgramYellow, VAOs,1, vertexColorLocation, 0.3, 0.7, greenValue, 1, 0);

        // third one
        drawTriangle(shaderProgramYellow, VAOs,2, vertexColorLocation, 0.3, 0.7, greenValue2, 1, 0);

        drawTriangle(shaderProgramYellow, VAOs,3, vertexColorLocation, 0.3, 0.7, greenValue3, 1, 0);
        
        drawTriangle(shaderProgramOrange, VAOs,4, vertexColorLocation, greenValue, 0.6, 0.8, 1.0, 1);




        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(4, VAOs);
    glDeleteBuffers(4, VBOs);
    glDeleteProgram(shaderProgramOrange);
    glDeleteProgram(shaderProgramYellow);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
