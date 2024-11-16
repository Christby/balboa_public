#include "hw3.h"
#include "3rdparty/glad.h" // needs to be included before GLFW!
#include "3rdparty/glfw/include/GLFW/glfw3.h"
#include "hw3_scenes.h"
#include "3rdparty/glm/glm/glm.hpp"
#include "3rdparty/glm/glm/gtc/matrix_transform.hpp"
#include "3rdparty/glm/glm/gtc/type_ptr.hpp"

using namespace hw3;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f,  0.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);

bool firstMouse = true;
float yaw = -90.0f;
float pitch = 0.0f;
float lastX = 800.0f / 2.0;
float lastY = 600.0f / 2.0;
float fov = 45.0f;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = static_cast<float>(120.0 * deltaTime);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
  
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; 
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw   += xoffset;
    pitch += yoffset;

    if(pitch > 89.0f)
        pitch = 89.0f;
    if(pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(direction);
}  

void hw_3_1(const std::vector<std::string> &params) {
    // initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // create a windowed mode window and its OpenGL context
    GLFWwindow* window = glfwCreateWindow(800, 600, "HW3 - OpenGL Window", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // load OpenGL function pointers with glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }

    glViewport(0, 0, 800, 600);

    // render loop
    while (!glfwWindowShouldClose(window)) {
        // input
        processInput(window);

        // render
        glClearColor(0.8f, 0.8f, 0.8f, 0.5f);
        glClear(GL_COLOR_BUFFER_BIT);

        // check and call events and swap the buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
}

unsigned int compileShader(unsigned int type, const char* source) {
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    // Check for compilation errors
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    return shader;
}

void hw_3_2(const std::vector<std::string> &params) {
    // initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // create a window
    GLFWwindow* window = glfwCreateWindow(800, 600, "HW3 - Rotating Triangle", NULL, NULL);
    if (!window) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // load OpenGL function pointers with glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }

    // define the triangle vertices
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };

    // create VAO and VBO
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // create vertex and fragment shaders
    const char* vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        uniform float rotationAngle;
        void main() {
            float cosAngle = cos(rotationAngle);
            float sinAngle = sin(rotationAngle);
            mat3 rotation = mat3(
                cosAngle, -sinAngle, 0.0,
                sinAngle,  cosAngle, 0.0,
                0.0,       0.0,      1.0
            );
            vec3 rotatedPos = rotation * aPos;
            gl_Position = vec4(rotatedPos, 1.0);
        }
    )";

    const char* fragmentShaderSource = R"(
        #version 330 core
        out vec4 FragColor;
        void main() {
            FragColor = vec4(1.0, 0.5, 0.2, 1.0);
        }
    )";

    // compile shaders
    unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    // link shaders into a program
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // check for linking errors
    int success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Set up rendering loop
    while (!glfwWindowShouldClose(window)) {
        // Process input
        processInput(window);

        // Clear the color buffer
        glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Activate the shader program
        glUseProgram(shaderProgram);

        // Set the rotation angle (using time for continuous rotation)
        float timeValue = glfwGetTime();
        float angle = timeValue; // Rotating counterclockwise
        int rotationLoc = glGetUniformLocation(shaderProgram, "rotationAngle");
        glUniform1f(rotationLoc, angle);

        // Render the triangle
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Clean up
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwDestroyWindow(window);
    glfwTerminate();
}

void hw_3_3(const std::vector<std::string> &params) {
    // HW 3.3: Render a scene
    if (params.size() == 0) {
        return;
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;

    // Initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Create GLFW Window
    GLFWwindow* window = glfwCreateWindow(800, 600, "HW3 - 3D Meshes with Transformations", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Load OpenGL Function Pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return;
    }

    // Enable Depth Testing and Gamma Correction
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_FRAMEBUFFER_SRGB);

    // Compile Shaders
    const char* vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aColor;
        uniform mat4 transform;
        out vec3 vertexColor;
        void main() {
            gl_Position = transform * vec4(aPos, 1.0);
            vertexColor = aColor;
        }
    )";

    const char* fragmentShaderSource = R"(
        #version 330 core
        in vec3 vertexColor;
        out vec4 FragColor;
        void main() {
            FragColor = vec4(vertexColor, 1.0);
        }
    )";

    unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // projection matrix
    Real width = scene.camera.resolution.x;
    Real height = scene.camera.resolution.y;
    Real alpha = width / height;
    Real s = scene.camera.s;
    Real z_near = scene.camera.z_near;
    Real z_far = scene.camera.z_far;

    Matrix4x4 projection_matrix = Matrix4x4::identity();
    projection_matrix(0, 0) = 1.0 / (alpha * s);
    projection_matrix(1, 1) = 1.0 / s;
    projection_matrix(2, 2) = -z_far / (z_far - z_near);
    projection_matrix(2, 3) = -(z_far * z_near) / (z_far - z_near);
    projection_matrix(3, 2) = -1.0;
    projection_matrix(3, 3) = 0.0;

    Matrix4x4 cam_to_world = scene.camera.cam_to_world;
    cameraPos = glm::vec3(cam_to_world(0,3), cam_to_world(1,3),cam_to_world(2,3));
    cameraFront = glm::vec3(-cam_to_world(0,2), -cam_to_world(1,2),-cam_to_world(2,2));
    cameraUp = glm::vec3(cam_to_world(0,1), cam_to_world(1,1),cam_to_world(2,1));

    yaw = glm::degrees(atan2(-cam_to_world(2,2),-cam_to_world(0,2)));
    pitch = glm::degrees(glm::asin((-cam_to_world(1,2))));

    // setup VAOs, VBOs, and EBOs
    std::vector<unsigned int> VAOs(scene.meshes.size());
    std::vector<unsigned int> VBOs_vertices(scene.meshes.size());
    std::vector<unsigned int> VBOs_colors(scene.meshes.size());
    std::vector<unsigned int> EBOs(scene.meshes.size());

    glGenVertexArrays(scene.meshes.size(), VAOs.data());
    glGenBuffers(scene.meshes.size(), VBOs_vertices.data());
    glGenBuffers(scene.meshes.size(), VBOs_colors.data());
    glGenBuffers(scene.meshes.size(), EBOs.data());

    for (size_t i = 0; i < scene.meshes.size(); ++i) {
        const auto& mesh = scene.meshes[i];

        glBindVertexArray(VAOs[i]);

        glBindBuffer(GL_ARRAY_BUFFER, VBOs_vertices[i]);
        glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Vector3f), mesh.vertices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, VBOs_colors[i]);
        glBufferData(GL_ARRAY_BUFFER, mesh.vertex_colors.size() * sizeof(Vector3f), mesh.vertex_colors.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[i]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.faces.size() * sizeof(Vector3i), mesh.faces.data(), GL_STATIC_DRAW);
    }

    // render loop
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(scene.background.x, scene.background.y, scene.background.z, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        glm::mat4 view_matrix = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        for (size_t i = 0; i < scene.meshes.size(); ++i) {
            const auto& mesh = scene.meshes[i];

            // trasnform model matrix to glm matrix
            glm::mat4 glm_model_matrix;
            for (int row = 0; row < 4; ++row) {
                for (int col = 0; col < 4; ++col) {
                    glm_model_matrix[col][row] = mesh.model_matrix(row, col);
                }
            }

            // trasnform projection matrix to glm matrix
            glm::mat4 glm_projection_matrix;
            for (int row = 0; row < 4; ++row) {
                for (int col = 0; col < 4; ++col) {
                    glm_projection_matrix[col][row] = projection_matrix(row, col);
                }
            }

            // compute overall transform matrix
            glm::mat4 transform_matrix = glm_projection_matrix * view_matrix * glm_model_matrix;
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "transform"), 1, GL_FALSE, glm::value_ptr(transform_matrix));

            glBindVertexArray(VAOs[i]);
            glDrawElements(GL_TRIANGLES, 3 * mesh.faces.size(), GL_UNSIGNED_INT, 0);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    for (size_t i = 0; i < VAOs.size(); ++i) {
        glDeleteVertexArrays(1, &VAOs[i]);
        glDeleteBuffers(1, &VBOs_vertices[i]);
        glDeleteBuffers(1, &VBOs_colors[i]);
        glDeleteBuffers(1, &EBOs[i]);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
}

void hw_3_4(const std::vector<std::string> &params) {
    // HW 3.4: Render a scene with lighting
    if (params.size() == 0) {
        return;
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;

    // Initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Create GLFW Window
    GLFWwindow* window = glfwCreateWindow(800, 600, "HW4 - Lighting", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    //glfwSetCursorPosCallback(window, mouse_callback);
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Load OpenGL Function Pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return;
    }

    // Enable Depth Testing and Gamma Correction
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_FRAMEBUFFER_SRGB);

    // Vertex Shader
    const char* vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aColor;
        layout (location = 2) in vec3 aNormal;

        out vec3 fragPos;
        out vec3 vertexColor;
        out vec3 normal;

        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;

        void main() {
            fragPos = vec3(model * vec4(aPos, 1.0));
            normal = mat3(transpose(inverse(model))) * aNormal; // Transform normals properly
            vertexColor = aColor;
            gl_Position = projection * view * vec4(fragPos, 1.0);
        }
    )";

    // Fragment Shader
    const char* fragmentShaderSource = R"(
        #version 330 core
        in vec3 fragPos;
        in vec3 vertexColor;
        in vec3 normal;

        out vec4 FragColor;

        uniform vec3 lightDir;
        uniform vec3 viewPos;
        uniform float ambientStrength;
        uniform float specularStrength;

        void main() {
            // Ambient
            vec3 ambient = ambientStrength * vertexColor;

            // Diffuse
            vec3 norm = normalize(normal);
            vec3 light = normalize(-lightDir); // Ensure light direction is normalized
            float diff = max(dot(norm, light), 0.0);
            vec3 diffuse = diff * vertexColor;

            // Specular
            vec3 viewDir = normalize(viewPos - fragPos);
            vec3 reflectDir = reflect(-light, norm);
            float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
            vec3 specular = specularStrength * spec * vec3(1.0); // Specular light is white

            // Combine results
            vec3 result = ambient + diffuse + specular;
            FragColor = vec4(result, 1.0);
        }
    )";

    unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // projection matrix
    Real width = scene.camera.resolution.x;
    Real height = scene.camera.resolution.y;
    Real alpha = width / height;
    Real s = scene.camera.s;
    Real z_near = scene.camera.z_near;
    Real z_far = scene.camera.z_far;

    Matrix4x4 projection_matrix = Matrix4x4::identity();
    projection_matrix(0, 0) = 1.0 / (alpha * s);
    projection_matrix(1, 1) = 1.0 / s;
    projection_matrix(2, 2) = -z_far / (z_far - z_near);
    projection_matrix(2, 3) = -(z_far * z_near) / (z_far - z_near);
    projection_matrix(3, 2) = -1.0;
    projection_matrix(3, 3) = 0.0;

    Matrix4x4 cam_to_world = scene.camera.cam_to_world;
    cameraPos = glm::vec3(cam_to_world(0, 3), cam_to_world(1, 3), cam_to_world(2, 3));
    cameraFront = glm::vec3(-cam_to_world(0, 2), -cam_to_world(1, 2), -cam_to_world(2, 2));
    cameraUp = glm::vec3(cam_to_world(0, 1), cam_to_world(1, 1), cam_to_world(2, 1));

    yaw = glm::degrees(atan2(-cam_to_world(2, 2), -cam_to_world(0, 2)));
    pitch = glm::degrees(glm::asin(-cam_to_world(1, 2)));

    // setup VAOs, VBOs, and EBOs
    std::vector<unsigned int> VAOs(scene.meshes.size());
    std::vector<unsigned int> VBOs_vertices(scene.meshes.size());
    std::vector<unsigned int> VBOs_colors(scene.meshes.size());
    std::vector<unsigned int> VBOs_normals(scene.meshes.size());
    std::vector<unsigned int> EBOs(scene.meshes.size());

    glGenVertexArrays(scene.meshes.size(), VAOs.data());
    glGenBuffers(scene.meshes.size(), VBOs_vertices.data());
    glGenBuffers(scene.meshes.size(), VBOs_colors.data());
    glGenBuffers(scene.meshes.size(), VBOs_normals.data());
    glGenBuffers(scene.meshes.size(), EBOs.data());

    for (size_t i = 0; i < scene.meshes.size(); ++i) {
        const auto& mesh = scene.meshes[i];

        glBindVertexArray(VAOs[i]);

        glBindBuffer(GL_ARRAY_BUFFER, VBOs_vertices[i]);
        glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Vector3f), mesh.vertices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, VBOs_colors[i]);
        glBufferData(GL_ARRAY_BUFFER, mesh.vertex_colors.size() * sizeof(Vector3f), mesh.vertex_colors.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, VBOs_normals[i]);
        glBufferData(GL_ARRAY_BUFFER, mesh.vertex_normals.size() * sizeof(Vector3f), mesh.vertex_normals.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(2);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[i]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.faces.size() * sizeof(Vector3i), mesh.faces.data(), GL_STATIC_DRAW);
    }

    // render loop
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(scene.background.x, scene.background.y, scene.background.z, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        glm::mat4 view_matrix = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        for (size_t i = 0; i < scene.meshes.size(); ++i) {
            const auto& mesh = scene.meshes[i];

            // trasnform model matrix to glm matrix
            glm::mat4 glm_model_matrix;
            for (int row = 0; row < 4; ++row) {
                for (int col = 0; col < 4; ++col) {
                    glm_model_matrix[col][row] = mesh.model_matrix(row, col);
                }
            }

            // trasnform projection matrix to glm matrix
            glm::mat4 glm_projection_matrix;
            for (int row = 0; row < 4; ++row) {
                for (int col = 0; col < 4; ++col) {
                    glm_projection_matrix[col][row] = projection_matrix(row, col);
                }
            }

            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(glm_model_matrix));
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view_matrix));
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(glm_projection_matrix));
            glUniform3fv(glGetUniformLocation(shaderProgram, "lightDir"), 1, glm::value_ptr(glm::normalize(glm::vec3(1.0f, 1.0f, 1.0f))));
            glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, glm::value_ptr(cameraPos));
            glUniform1f(glGetUniformLocation(shaderProgram, "ambientStrength"), 0.1f);
            glUniform1f(glGetUniformLocation(shaderProgram, "specularStrength"), 0.5f);

            glBindVertexArray(VAOs[i]);
            glDrawElements(GL_TRIANGLES, 3 * mesh.faces.size(), GL_UNSIGNED_INT, 0);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    for (size_t i = 0; i < VAOs.size(); ++i) {
        glDeleteVertexArrays(1, &VAOs[i]);
        glDeleteBuffers(1, &VBOs_vertices[i]);
        glDeleteBuffers(1, &VBOs_colors[i]);
        glDeleteBuffers(1, &VBOs_normals[i]);
        glDeleteBuffers(1, &EBOs[i]);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
}

void hw_3_4_animation(const std::vector<std::string> &params) {
    // HW 3.4 bonus: Lighting Animation
    if (params.size() == 0) {
        return;
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;

    // Initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Create GLFW Window
    GLFWwindow* window = glfwCreateWindow(800, 600, "HW4 - Lighting Animation", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    //glfwSetCursorPosCallback(window, mouse_callback);
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Load OpenGL Function Pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return;
    }

    // Enable Depth Testing and Gamma Correction
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_FRAMEBUFFER_SRGB);

    // Vertex Shader
    const char* vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aColor;
        layout (location = 2) in vec3 aNormal;

        out vec3 fragPos;
        out vec3 vertexColor;
        out vec3 normal;

        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;

        void main() {
            fragPos = vec3(model * vec4(aPos, 1.0));
            normal = mat3(transpose(inverse(model))) * aNormal; // Transform normals properly
            vertexColor = aColor;
            gl_Position = projection * view * vec4(fragPos, 1.0);
        }
    )";

    // Fragment Shader
    const char* fragmentShaderSource = R"(
        #version 330 core
        in vec3 fragPos;
        in vec3 vertexColor;
        in vec3 normal;

        out vec4 FragColor;

        uniform vec3 lightDir;
        uniform vec3 viewPos;
        uniform float ambientStrength;
        uniform float specularStrength;

        void main() {
            // Ambient
            vec3 ambient = ambientStrength * vertexColor;

            // Diffuse
            vec3 norm = normalize(normal);
            vec3 light = normalize(-lightDir); // Ensure light direction is normalized
            float diff = max(dot(norm, light), 0.0);
            vec3 diffuse = diff * vertexColor;

            // Specular
            vec3 viewDir = normalize(viewPos - fragPos);
            vec3 reflectDir = reflect(-light, norm);
            float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
            vec3 specular = specularStrength * spec * vec3(1.0); // Specular light is white

            // Combine results
            vec3 result = ambient + diffuse + specular;
            FragColor = vec4(result, 1.0);
        }
    )";

    unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // projection matrix
    Real width = scene.camera.resolution.x;
    Real height = scene.camera.resolution.y;
    Real alpha = width / height;
    Real s = scene.camera.s;
    Real z_near = scene.camera.z_near;
    Real z_far = scene.camera.z_far;

    Matrix4x4 projection_matrix = Matrix4x4::identity();
    projection_matrix(0, 0) = 1.0 / (alpha * s);
    projection_matrix(1, 1) = 1.0 / s;
    projection_matrix(2, 2) = -z_far / (z_far - z_near);
    projection_matrix(2, 3) = -(z_far * z_near) / (z_far - z_near);
    projection_matrix(3, 2) = -1.0;
    projection_matrix(3, 3) = 0.0;

    Matrix4x4 cam_to_world = scene.camera.cam_to_world;
    cameraPos = glm::vec3(cam_to_world(0, 3), cam_to_world(1, 3), cam_to_world(2, 3));
    cameraFront = glm::vec3(-cam_to_world(0, 2), -cam_to_world(1, 2), -cam_to_world(2, 2));
    cameraUp = glm::vec3(cam_to_world(0, 1), cam_to_world(1, 1), cam_to_world(2, 1));

    // setup VAOs, VBOs, and EBOs
    std::vector<unsigned int> VAOs(scene.meshes.size());
    std::vector<unsigned int> VBOs_vertices(scene.meshes.size());
    std::vector<unsigned int> VBOs_colors(scene.meshes.size());
    std::vector<unsigned int> VBOs_normals(scene.meshes.size());
    std::vector<unsigned int> EBOs(scene.meshes.size());

    glGenVertexArrays(scene.meshes.size(), VAOs.data());
    glGenBuffers(scene.meshes.size(), VBOs_vertices.data());
    glGenBuffers(scene.meshes.size(), VBOs_colors.data());
    glGenBuffers(scene.meshes.size(), VBOs_normals.data());
    glGenBuffers(scene.meshes.size(), EBOs.data());

    for (size_t i = 0; i < scene.meshes.size(); ++i) {
        const auto& mesh = scene.meshes[i];

        glBindVertexArray(VAOs[i]);

        glBindBuffer(GL_ARRAY_BUFFER, VBOs_vertices[i]);
        glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Vector3f), mesh.vertices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, VBOs_colors[i]);
        glBufferData(GL_ARRAY_BUFFER, mesh.vertex_colors.size() * sizeof(Vector3f), mesh.vertex_colors.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, VBOs_normals[i]);
        glBufferData(GL_ARRAY_BUFFER, mesh.vertex_normals.size() * sizeof(Vector3f), mesh.vertex_normals.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(2);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[i]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.faces.size() * sizeof(Vector3i), mesh.faces.data(), GL_STATIC_DRAW);
    }

    // render loop
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glClearColor(scene.background.x, scene.background.y, scene.background.z, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        glm::mat4 view_matrix = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        // light position
        float lightX = cos(currentFrame) * 1.0f;
        float lightZ = sin(currentFrame) * 1.0f;
        glm::vec3 lightDir = glm::normalize(glm::vec3(lightX, 1.0f, lightZ));

        for (size_t i = 0; i < scene.meshes.size(); ++i) {
            const auto& mesh = scene.meshes[i];

            glm::mat4 glm_model_matrix;
            for (int row = 0; row < 4; ++row) {
                for (int col = 0; col < 4; ++col) {
                    glm_model_matrix[col][row] = mesh.model_matrix(row, col);
                }
            }

            glm::mat4 glm_projection_matrix;
            for (int row = 0; row < 4; ++row) {
                for (int col = 0; col < 4; ++col) {
                    glm_projection_matrix[col][row] = projection_matrix(row, col);
                }
            }

            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(glm_model_matrix));
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view_matrix));
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(glm_projection_matrix));
            glUniform3fv(glGetUniformLocation(shaderProgram, "lightDir"), 1, glm::value_ptr(lightDir));
            glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, glm::value_ptr(cameraPos));
            glUniform1f(glGetUniformLocation(shaderProgram, "ambientStrength"), 0.1f);
            glUniform1f(glGetUniformLocation(shaderProgram, "specularStrength"), 0.5f);

            glBindVertexArray(VAOs[i]);
            glDrawElements(GL_TRIANGLES, 3 * mesh.faces.size(), GL_UNSIGNED_INT, 0);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    for (size_t i = 0; i < VAOs.size(); ++i) {
        glDeleteVertexArrays(1, &VAOs[i]);
        glDeleteBuffers(1, &VBOs_vertices[i]);
        glDeleteBuffers(1, &VBOs_colors[i]);
        glDeleteBuffers(1, &VBOs_normals[i]);
        glDeleteBuffers(1, &EBOs[i]);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
}

void hw_3_4_moreLights(const std::vector<std::string> &params) {
    // HW 3.4: Render a scene with multiple lights (directional, point, and spotlights)
    if (params.size() == 0) {
        return;
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;

    // Initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Create GLFW Window
    GLFWwindow* window = glfwCreateWindow(800, 600, "HW4 - Multiple Lights", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    // glfwSetCursorPosCallback(window, mouse_callback);
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Load OpenGL Function Pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return;
    }

    // Enable Depth Testing and Gamma Correction
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_FRAMEBUFFER_SRGB);

    // Vertex Shader
    const char* vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aColor;
        layout (location = 2) in vec3 aNormal;

        out vec3 FragPos;
        out vec3 VertexColor;
        out vec3 Normal;

        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;

        void main() {
            FragPos = vec3(model * vec4(aPos, 1.0));
            Normal = mat3(transpose(inverse(model))) * aNormal; // Transform normals properly
            VertexColor = aColor;
            gl_Position = projection * view * vec4(FragPos, 1.0);
        }
    )";

    // Fragment Shader
    const char* fragmentShaderSource = R"(
        #version 330 core
        struct DirLight {
            vec3 direction;
            vec3 ambient;
            vec3 diffuse;
            vec3 specular;
        };

        struct PointLight {
            vec3 position;
            vec3 ambient;
            vec3 diffuse;
            vec3 specular;

            float constant;
            float linear;
            float quadratic;
        };

        struct SpotLight {
            vec3 position;
            vec3 direction;
            float cutOff;
            float outerCutOff;

            vec3 ambient;
            vec3 diffuse;
            vec3 specular;

            float constant;
            float linear;
            float quadratic;
        };

        #define NR_POINT_LIGHTS 4

        in vec3 FragPos;
        in vec3 VertexColor;
        in vec3 Normal;

        out vec4 FragColor;

        uniform vec3 viewPos;
        uniform DirLight dirLight;
        uniform PointLight pointLights[NR_POINT_LIGHTS];
        uniform SpotLight spotLight;

        // Function prototypes
        vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 objectColor);
        vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 objectColor);
        vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 objectColor);

        void main() {
            vec3 norm = normalize(Normal);
            vec3 viewDir = normalize(viewPos - FragPos);
            vec3 result = vec3(0.0);

            // Directional light
            result += CalcDirLight(dirLight, norm, viewDir, VertexColor);

            // Point lights
            for(int i = 0; i < NR_POINT_LIGHTS; i++)
                result += CalcPointLight(pointLights[i], norm, FragPos, viewDir, VertexColor);

            // Spot light
            result += CalcSpotLight(spotLight, norm, FragPos, viewDir, VertexColor);

            FragColor = vec4(result, 1.0);
        }

        // Calculate directional light
        vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 objectColor) {
            vec3 lightDir = normalize(-light.direction);
            // Diffuse shading
            float diff = max(dot(normal, lightDir), 0.0);
            // Specular shading
            vec3 reflectDir = reflect(-lightDir, normal);
            float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
            // Combine results
            vec3 ambient = light.ambient * objectColor;
            vec3 diffuse = light.diffuse * diff * objectColor;
            vec3 specular = light.specular * spec;
            return (ambient + diffuse + specular);
        }

        // Calculate point light
        vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 objectColor) {
            vec3 lightDir = normalize(light.position - fragPos);
            // Diffuse shading
            float diff = max(dot(normal, lightDir), 0.0);
            // Specular shading
            vec3 reflectDir = reflect(-lightDir, normal);
            float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
            // Attenuation
            float distance = length(light.position - fragPos);
            float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
            // Combine results
            vec3 ambient = light.ambient * objectColor;
            vec3 diffuse = light.diffuse * diff * objectColor;
            vec3 specular = light.specular * spec;
            ambient *= attenuation;
            diffuse *= attenuation;
            specular *= attenuation;
            return (ambient + diffuse + specular);
        }

        // Calculate spot light
        vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 objectColor) {
            vec3 lightDir = normalize(light.position - fragPos);
            // Diffuse shading
            float diff = max(dot(normal, lightDir), 0.0);
            // Specular shading
            vec3 reflectDir = reflect(-lightDir, normal);
            float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
            // Attenuation
            float distance = length(light.position - fragPos);
            float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
            // Spotlight intensity
            float theta = dot(lightDir, normalize(-light.direction));
            float epsilon = light.cutOff - light.outerCutOff;
            float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
            // Combine results
            vec3 ambient = light.ambient * objectColor;
            vec3 diffuse = light.diffuse * diff * objectColor;
            vec3 specular = light.specular * spec;
            ambient *= attenuation * intensity;
            diffuse *= attenuation * intensity;
            specular *= attenuation * intensity;
            return (ambient + diffuse + specular);
        }
    )";

    unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    int success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // projection matrix
    Real width = scene.camera.resolution.x;
    Real height = scene.camera.resolution.y;
    Real alpha = width / height;
    Real s = scene.camera.s;
    Real z_near = scene.camera.z_near;
    Real z_far = scene.camera.z_far;

    Matrix4x4 projection_matrix = Matrix4x4::identity();
    projection_matrix(0, 0) = 1.0 / (alpha * s);
    projection_matrix(1, 1) = 1.0 / s;
    projection_matrix(2, 2) = -z_far / (z_far - z_near);
    projection_matrix(2, 3) = -(z_far * z_near) / (z_far - z_near);
    projection_matrix(3, 2) = -1.0;
    projection_matrix(3, 3) = 0.0;

    Matrix4x4 cam_to_world = scene.camera.cam_to_world;
    cameraPos = glm::vec3(cam_to_world(0, 3), cam_to_world(1, 3), cam_to_world(2, 3));
    cameraFront = glm::vec3(-cam_to_world(0, 2), -cam_to_world(1, 2), -cam_to_world(2, 2));
    cameraUp = glm::vec3(cam_to_world(0, 1), cam_to_world(1, 1), cam_to_world(2, 1));

    yaw = glm::degrees(atan2(-cam_to_world(2, 2), -cam_to_world(0, 2)));
    pitch = glm::degrees(glm::asin(-cam_to_world(1, 2)));

    for (auto& mesh : scene.meshes) {
        if (mesh.vertex_normals.empty()) {
            mesh.vertex_normals.resize(mesh.vertices.size(), Vector3f{0, 0, 0});
            for (const auto& face : mesh.faces) {
                Vector3f v0 = mesh.vertices[face[0]];
                Vector3f v1 = mesh.vertices[face[1]];
                Vector3f v2 = mesh.vertices[face[2]];
                Vector3f normal = cross(v1 - v0, v2 - v0);
                normal = normalize(normal);
                mesh.vertex_normals[face[0]] += normal;
                mesh.vertex_normals[face[1]] += normal;
                mesh.vertex_normals[face[2]] += normal;
            }
            for (auto& normal : mesh.vertex_normals) {
                normal = normalize(normal);
            }
        }

        if (mesh.vertex_colors.empty()) {
            mesh.vertex_colors.resize(mesh.vertices.size(), Vector3f{1.0f, 0.5f, 0.2f}); // Default color
        }
    }

    // setup VAOs, VBOs, and EBOs
    std::vector<unsigned int> VAOs(scene.meshes.size());
    std::vector<unsigned int> VBOs_vertices(scene.meshes.size());
    std::vector<unsigned int> VBOs_colors(scene.meshes.size());
    std::vector<unsigned int> VBOs_normals(scene.meshes.size());
    std::vector<unsigned int> EBOs(scene.meshes.size());

    glGenVertexArrays(scene.meshes.size(), VAOs.data());
    glGenBuffers(scene.meshes.size(), VBOs_vertices.data());
    glGenBuffers(scene.meshes.size(), VBOs_colors.data());
    glGenBuffers(scene.meshes.size(), VBOs_normals.data());
    glGenBuffers(scene.meshes.size(), EBOs.data());

    for (size_t i = 0; i < scene.meshes.size(); ++i) {
        const auto& mesh = scene.meshes[i];

        glBindVertexArray(VAOs[i]);

        glBindBuffer(GL_ARRAY_BUFFER, VBOs_vertices[i]);
        glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Vector3f), mesh.vertices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, VBOs_colors[i]);
        glBufferData(GL_ARRAY_BUFFER, mesh.vertex_colors.size() * sizeof(Vector3f), mesh.vertex_colors.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, VBOs_normals[i]);
        glBufferData(GL_ARRAY_BUFFER, mesh.vertex_normals.size() * sizeof(Vector3f), mesh.vertex_normals.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(2);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[i]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.faces.size() * sizeof(Vector3i), mesh.faces.data(), GL_STATIC_DRAW);
    }

    // define lights
    glm::vec3 dirLightDirection = glm::vec3(-0.2f, -1.0f, -0.3f);
    glm::vec3 dirLightAmbient = glm::vec3(0.05f, 0.05f, 0.05f);
    glm::vec3 dirLightDiffuse = glm::vec3(0.4f, 0.4f, 0.4f);
    glm::vec3 dirLightSpecular = glm::vec3(0.5f, 0.5f, 0.5f);

    const int NR_POINT_LIGHTS = 4;
    glm::vec3 pointLightPositions[NR_POINT_LIGHTS] = {
        glm::vec3(0.7f,  0.2f,  2.0f),
        glm::vec3(2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f, 2.0f, -12.0f),
        glm::vec3(0.0f,  0.0f, -3.0f)
    };
    glm::vec3 pointLightAmbient = glm::vec3(0.05f, 0.05f, 0.05f);
    glm::vec3 pointLightDiffuse = glm::vec3(0.8f, 0.8f, 0.8f);
    glm::vec3 pointLightSpecular = glm::vec3(1.0f, 1.0f, 1.0f);

    float pointLightConstant = 1.0f;
    float pointLightLinear = 0.09f;
    float pointLightQuadratic = 0.032f;

    glm::vec3 spotLightAmbient = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 spotLightDiffuse = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 spotLightSpecular = glm::vec3(1.0f, 1.0f, 1.0f);
    float spotLightConstant = 1.0f;
    float spotLightLinear = 0.09f;
    float spotLightQuadratic = 0.032f;
    float spotLightCutOff = glm::cos(glm::radians(12.5f));
    float spotLightOuterCutOff = glm::cos(glm::radians(17.5f));

    // render loop
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(scene.background.x, scene.background.y, scene.background.z, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        glm::mat4 view_matrix = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        glm::mat4 glm_projection_matrix;
        for (int row = 0; row < 4; ++row) {
            for (int col = 0; col < 4; ++col) {
                glm_projection_matrix[col][row] = projection_matrix(row, col);
            }
        }
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view_matrix));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(glm_projection_matrix));

        glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, glm::value_ptr(cameraPos));

        glUniform3fv(glGetUniformLocation(shaderProgram, "dirLight.direction"), 1, glm::value_ptr(dirLightDirection));
        glUniform3fv(glGetUniformLocation(shaderProgram, "dirLight.ambient"), 1, glm::value_ptr(dirLightAmbient));
        glUniform3fv(glGetUniformLocation(shaderProgram, "dirLight.diffuse"), 1, glm::value_ptr(dirLightDiffuse));
        glUniform3fv(glGetUniformLocation(shaderProgram, "dirLight.specular"), 1, glm::value_ptr(dirLightSpecular));

        for (int i = 0; i < NR_POINT_LIGHTS; i++) {
            std::string number = std::to_string(i);
            glUniform3fv(glGetUniformLocation(shaderProgram, ("pointLights[" + number + "].position").c_str()), 1, glm::value_ptr(pointLightPositions[i]));
            glUniform3fv(glGetUniformLocation(shaderProgram, ("pointLights[" + number + "].ambient").c_str()), 1, glm::value_ptr(pointLightAmbient));
            glUniform3fv(glGetUniformLocation(shaderProgram, ("pointLights[" + number + "].diffuse").c_str()), 1, glm::value_ptr(pointLightDiffuse));
            glUniform3fv(glGetUniformLocation(shaderProgram, ("pointLights[" + number + "].specular").c_str()), 1, glm::value_ptr(pointLightSpecular));
            glUniform1f(glGetUniformLocation(shaderProgram, ("pointLights[" + number + "].constant").c_str()), pointLightConstant);
            glUniform1f(glGetUniformLocation(shaderProgram, ("pointLights[" + number + "].linear").c_str()), pointLightLinear);
            glUniform1f(glGetUniformLocation(shaderProgram, ("pointLights[" + number + "].quadratic").c_str()), pointLightQuadratic);
        }

        glUniform3fv(glGetUniformLocation(shaderProgram, "spotLight.position"), 1, glm::value_ptr(cameraPos));
        glUniform3fv(glGetUniformLocation(shaderProgram, "spotLight.direction"), 1, glm::value_ptr(cameraFront));
        glUniform1f(glGetUniformLocation(shaderProgram, "spotLight.cutOff"), spotLightCutOff);
        glUniform1f(glGetUniformLocation(shaderProgram, "spotLight.outerCutOff"), spotLightOuterCutOff);
        glUniform3fv(glGetUniformLocation(shaderProgram, "spotLight.ambient"), 1, glm::value_ptr(spotLightAmbient));
        glUniform3fv(glGetUniformLocation(shaderProgram, "spotLight.diffuse"), 1, glm::value_ptr(spotLightDiffuse));
        glUniform3fv(glGetUniformLocation(shaderProgram, "spotLight.specular"), 1, glm::value_ptr(spotLightSpecular));
        glUniform1f(glGetUniformLocation(shaderProgram, "spotLight.constant"), spotLightConstant);
        glUniform1f(glGetUniformLocation(shaderProgram, "spotLight.linear"), spotLightLinear);
        glUniform1f(glGetUniformLocation(shaderProgram, "spotLight.quadratic"), spotLightQuadratic);

        for (size_t i = 0; i < scene.meshes.size(); ++i) {
            const auto& mesh = scene.meshes[i];

            // Transform model matrix to glm matrix
            glm::mat4 glm_model_matrix;
            for (int row = 0; row < 4; ++row) {
                for (int col = 0; col < 4; ++col) {
                    glm_model_matrix[col][row] = mesh.model_matrix(row, col);
                }
            }
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(glm_model_matrix));

            glBindVertexArray(VAOs[i]);
            glDrawElements(GL_TRIANGLES, 3 * mesh.faces.size(), GL_UNSIGNED_INT, 0);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    for (size_t i = 0; i < VAOs.size(); ++i) {
        glDeleteVertexArrays(1, &VAOs[i]);
        glDeleteBuffers(1, &VBOs_vertices[i]);
        glDeleteBuffers(1, &VBOs_colors[i]);
        glDeleteBuffers(1, &VBOs_normals[i]);
        glDeleteBuffers(1, &EBOs[i]);
    }

    glDeleteProgram(shaderProgram);

    glfwDestroyWindow(window);
    glfwTerminate();
}

void hw_3_4_shadow(const std::vector<std::string> &params) {
    // HW 3.4: Shadow Mapping
    if (params.size() == 0) {
        return;
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;

    // Initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Create GLFW Window
    int windowWidth = 800;
    int windowHeight = 600;
    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "HW4 - Shadow Mapping", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    // glfwSetCursorPosCallback(window, mouse_callback);
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Load OpenGL Function Pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return;
    }

    // Enable Depth Testing and Gamma Correction
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_FRAMEBUFFER_SRGB);

    // Build and compile shaders
    const char* depthVertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        uniform mat4 model;
        uniform mat4 lightSpaceMatrix;
        void main()
        {
            gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);
        }
    )";

    const char* depthFragmentShaderSource = R"(
        #version 330 core
        void main()
        {
            // Empty fragment shader
        }
    )";

    unsigned int depthVertexShader = compileShader(GL_VERTEX_SHADER, depthVertexShaderSource);
    unsigned int depthFragmentShader = compileShader(GL_FRAGMENT_SHADER, depthFragmentShaderSource);
    unsigned int depthShaderProgram = glCreateProgram();
    glAttachShader(depthShaderProgram, depthVertexShader);
    glAttachShader(depthShaderProgram, depthFragmentShader);
    glLinkProgram(depthShaderProgram);

    int success;
    glGetProgramiv(depthShaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(depthShaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::DEPTH_SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(depthVertexShader);
    glDeleteShader(depthFragmentShader);

    const char* vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aColor;
        layout (location = 2) in vec3 aNormal;

        out vec3 fragPos;
        out vec3 vertexColor;
        out vec3 normal;
        out vec4 FragPosLightSpace; // Position in light space

        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;
        uniform mat4 lightSpaceMatrix;

        void main() {
            fragPos = vec3(model * vec4(aPos, 1.0));
            normal = mat3(transpose(inverse(model))) * aNormal;
            vertexColor = aColor;
            gl_Position = projection * view * vec4(fragPos, 1.0);
            FragPosLightSpace = lightSpaceMatrix * vec4(fragPos, 1.0);
        }
    )";

    const char* fragmentShaderSource = R"(
        #version 330 core
        in vec3 fragPos;
        in vec3 vertexColor;
        in vec3 normal;
        in vec4 FragPosLightSpace;

        out vec4 FragColor;

        uniform vec3 lightDir;
        uniform vec3 viewPos;
        uniform float ambientStrength;
        uniform float specularStrength;
        uniform sampler2D shadowMap;

        float ShadowCalculation(vec4 fragPosLightSpace)
        {
            vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
            projCoords = projCoords * 0.5 + 0.5;
            float closestDepth = texture(shadowMap, projCoords.xy).r;
            float currentDepth = projCoords.z;
            float bias = 0.005;
            float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
            if (projCoords.z > 1.0)
                shadow = 0.0;
            return shadow;
        }

        void main() {
            vec3 ambient = ambientStrength * vertexColor;
            vec3 norm = normalize(normal);
            vec3 light = normalize(-lightDir);
            float diff = max(dot(norm, light), 0.0);
            vec3 diffuse = diff * vertexColor;
            vec3 viewDir = normalize(viewPos - fragPos);
            vec3 reflectDir = reflect(-light, norm);
            float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
            vec3 specular = specularStrength * spec * vec3(1.0);
            float shadow = ShadowCalculation(FragPosLightSpace);
            vec3 lighting = ambient + (1.0 - shadow) * (diffuse + specular);
            FragColor = vec4(lighting, 1.0);
        }
    )";

    unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    Real width = scene.camera.resolution.x;
    Real height = scene.camera.resolution.y;
    Real alpha = width / height;
    Real s = scene.camera.s;
    Real z_near = scene.camera.z_near;
    Real z_far = scene.camera.z_far;

    Matrix4x4 projection_matrix = Matrix4x4::identity();
    projection_matrix(0, 0) = 1.0 / (alpha * s);
    projection_matrix(1, 1) = 1.0 / s;
    projection_matrix(2, 2) = -z_far / (z_far - z_near);
    projection_matrix(2, 3) = -(z_far * z_near) / (z_far - z_near);
    projection_matrix(3, 2) = -1.0;
    projection_matrix(3, 3) = 0.0;

    Matrix4x4 cam_to_world = scene.camera.cam_to_world;
    cameraPos = glm::vec3(cam_to_world(0, 3), cam_to_world(1, 3), cam_to_world(2, 3));
    cameraFront = glm::vec3(-cam_to_world(0, 2), -cam_to_world(1, 2), -cam_to_world(2, 2));
    cameraUp = glm::vec3(cam_to_world(0, 1), cam_to_world(1, 1), cam_to_world(2, 1));

    yaw = glm::degrees(atan2(-cam_to_world(2, 2), -cam_to_world(0, 2)));
    pitch = glm::degrees(glm::asin(-cam_to_world(1, 2)));

    for (auto& mesh : scene.meshes) {
        if (mesh.vertex_normals.empty()) {
            mesh.vertex_normals.resize(mesh.vertices.size(), Vector3f{0, 0, 0});
            for (const auto& face : mesh.faces) {
                Vector3f v0 = mesh.vertices[face[0]];
                Vector3f v1 = mesh.vertices[face[1]];
                Vector3f v2 = mesh.vertices[face[2]];
                Vector3f normal = cross(v1 - v0, v2 - v0);
                normal = normalize(normal);
                mesh.vertex_normals[face[0]] += normal;
                mesh.vertex_normals[face[1]] += normal;
                mesh.vertex_normals[face[2]] += normal;
            }
            for (auto& normal : mesh.vertex_normals) {
                normal = normalize(normal);
            }
        }

        if (mesh.vertex_colors.empty()) {
            mesh.vertex_colors.resize(mesh.vertices.size(), Vector3f{1.0f, 0.5f, 0.2f}); // Default color
        }
    }

    std::vector<unsigned int> VAOs(scene.meshes.size());
    std::vector<unsigned int> VBOs_vertices(scene.meshes.size());
    std::vector<unsigned int> VBOs_colors(scene.meshes.size());
    std::vector<unsigned int> VBOs_normals(scene.meshes.size());
    std::vector<unsigned int> EBOs(scene.meshes.size());

    glGenVertexArrays(scene.meshes.size(), VAOs.data());
    glGenBuffers(scene.meshes.size(), VBOs_vertices.data());
    glGenBuffers(scene.meshes.size(), VBOs_colors.data());
    glGenBuffers(scene.meshes.size(), VBOs_normals.data());
    glGenBuffers(scene.meshes.size(), EBOs.data());

    for (size_t i = 0; i < scene.meshes.size(); ++i) {
        const auto& mesh = scene.meshes[i];

        glBindVertexArray(VAOs[i]);

        glBindBuffer(GL_ARRAY_BUFFER, VBOs_vertices[i]);
        glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Vector3f), mesh.vertices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, VBOs_colors[i]);
        glBufferData(GL_ARRAY_BUFFER, mesh.vertex_colors.size() * sizeof(Vector3f), mesh.vertex_colors.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, VBOs_normals[i]);
        glBufferData(GL_ARRAY_BUFFER, mesh.vertex_normals.size() * sizeof(Vector3f), mesh.vertex_normals.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(2);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[i]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.faces.size() * sizeof(Vector3i), mesh.faces.data(), GL_STATIC_DRAW);
    }

    unsigned int depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);

    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    unsigned int depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
                 SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // or GL_LINEAR
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // or GL_LINEAR
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER); 
    float borderColor[] = {1.0, 1.0, 1.0, 1.0};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glm::vec3 lightDir = glm::normalize(glm::vec3(-1.0f, 1.0f, -1.0f));
    glm::vec3 lightPos = -lightDir * 200.0f;

    // render loop
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glm::mat4 lightProjection, lightView;
        glm::mat4 lightSpaceMatrix;
        float near_plane = 1.0f, far_plane = 500.0f;
        float ortho_size = 200.0f;
        lightProjection = glm::ortho(-ortho_size, ortho_size, -ortho_size, ortho_size, near_plane, far_plane);
        lightView = glm::lookAt(lightPos, glm::vec3(0.0f, 0.0f, 32.0f), glm::vec3(0.0, 0.0, 1.0));
        lightSpaceMatrix = lightProjection * lightView;

        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);

        glUseProgram(depthShaderProgram);
        glUniformMatrix4fv(glGetUniformLocation(depthShaderProgram, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

        for (size_t i = 0; i < scene.meshes.size(); ++i) {
            const auto& mesh = scene.meshes[i];

            glm::mat4 glm_model_matrix;
            for (int row = 0; row < 4; ++row) {
                for (int col = 0; col < 4; ++col) {
                    glm_model_matrix[col][row] = mesh.model_matrix(row, col);
                }
            }
            glUniformMatrix4fv(glGetUniformLocation(depthShaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(glm_model_matrix));

            glBindVertexArray(VAOs[i]);
            glDrawElements(GL_TRIANGLES, 3 * mesh.faces.size(), GL_UNSIGNED_INT, 0);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glViewport(0, 0, windowWidth, windowHeight);
        glClearColor(scene.background.x, scene.background.y, scene.background.z, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, glm::value_ptr(cameraPos));
        glUniform1f(glGetUniformLocation(shaderProgram, "ambientStrength"), 0.1f);
        glUniform1f(glGetUniformLocation(shaderProgram, "specularStrength"), 0.5f);
        glUniform3fv(glGetUniformLocation(shaderProgram, "lightDir"), 1, glm::value_ptr(lightDir));

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glUniform1i(glGetUniformLocation(shaderProgram, "shadowMap"), 0);

        glm::mat4 view_matrix = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        for (size_t i = 0; i < scene.meshes.size(); ++i) {
            const auto& mesh = scene.meshes[i];

            glm::mat4 glm_model_matrix;
            for (int row = 0; row < 4; ++row) {
                for (int col = 0; col < 4; ++col) {
                    glm_model_matrix[col][row] = mesh.model_matrix(row, col);
                }
            }

            glm::mat4 glm_projection_matrix;
            for (int row = 0; row < 4; ++row) {
                for (int col = 0; col < 4; ++col) {
                    glm_projection_matrix[col][row] = projection_matrix(row, col);
                }
            }

            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(glm_model_matrix));
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view_matrix));
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(glm_projection_matrix));

            glBindVertexArray(VAOs[i]);
            glDrawElements(GL_TRIANGLES, 3 * mesh.faces.size(), GL_UNSIGNED_INT, 0);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    for (size_t i = 0; i < VAOs.size(); ++i) {
        glDeleteVertexArrays(1, &VAOs[i]);
        glDeleteBuffers(1, &VBOs_vertices[i]);
        glDeleteBuffers(1, &VBOs_colors[i]);
        glDeleteBuffers(1, &VBOs_normals[i]);
        glDeleteBuffers(1, &EBOs[i]);
    }

    glDeleteProgram(shaderProgram);
    glDeleteProgram(depthShaderProgram);
    glDeleteTextures(1, &depthMap);
    glDeleteFramebuffers(1, &depthMapFBO);

    glfwDestroyWindow(window);
    glfwTerminate();
}
