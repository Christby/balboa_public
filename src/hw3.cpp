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