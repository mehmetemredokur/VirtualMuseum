// -----------------------------------------------------------------------------
// main.cpp – VirtualMuseum (düzeltilmiş)
// -----------------------------------------------------------------------------
//  * Dinamik merkez‑odaklı kamera (orbit + zoom)
//  * AABB bir kez hesaplanır, her frame hesaplanmaz
//  * Fare sol tuş: orbit   |  Scroll: zoom
//  * Robot/Scene/UI kodları korunur
// -----------------------------------------------------------------------------

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "Shader.h"
#include "Scene.h"
#include "Robot.h"
#include "UIManager.h"

// ImGui ------------------------------------------------------------
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

// -----------------------------------------------------------------
// GLOBAL SABİTLER / DEĞİŞKENLER
// -----------------------------------------------------------------
constexpr unsigned int SCR_WIDTH  = 1280;
constexpr unsigned int SCR_HEIGHT =  720;

float deltaTime = 0.0f;            // frame‑ler arası süre
float lastFrame = 0.0f;

// ----------------------------------------------------------------------------
// Kamera (orbit)
// ----------------------------------------------------------------------------
namespace Cam {
    // Sahne bilgisi (Scene::getSceneBounds ile atanıyor)
    glm::vec3 center(0.0f);
    float     radius  = 1.0f;

    // Kontroller
    float fov        = 45.0f;   // derece
    float distance   = 5.0f;    // merkezden uzaklık
    float yawDeg   =  180.0f;  // arkadaki duvara bakmak yerine ön duvara bak
    float pitchDeg =  -10.0f;  // hafif aşağı

    // Fare kontrol durumu
    bool  orbiting   = false;
    double lastX = 0.0, lastY = 0.0;

    // Pos & matrisler her frame üretilecek
    inline glm::vec3 position()
    {
        glm::vec3 dir;
        dir.x = distance * std::cos(glm::radians(pitchDeg)) * std::cos(glm::radians(yawDeg));
        dir.y = distance * std::sin(glm::radians(pitchDeg));
        dir.z = distance * std::cos(glm::radians(pitchDeg)) * std::sin(glm::radians(yawDeg));
        return center + dir;
    }
}

// -----------------------------------------------------------------------------
// GLFW CALLBACK’LERI
// -----------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void cursor_position_callback(GLFWwindow* /*window*/, double xpos, double ypos)
{
    if (!Cam::orbiting) return;

    float dx = static_cast<float>(xpos - Cam::lastX);
    float dy = static_cast<float>(ypos - Cam::lastY);
    const float sens = 0.1f;

    Cam::yawDeg   += dx * sens;
    Cam::pitchDeg += dy * sens;
    Cam::pitchDeg = std::clamp(Cam::pitchDeg, -89.0f, 89.0f);

    Cam::lastX = xpos;
    Cam::lastY = ypos;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int /*mods*/)
{
    if (button != GLFW_MOUSE_BUTTON_LEFT) return;

    if (action == GLFW_PRESS) {
        Cam::orbiting = true;
        glfwGetCursorPos(window, &Cam::lastX, &Cam::lastY);
    }
    else if (action == GLFW_RELEASE) {
        Cam::orbiting = false;
    }
}

void scroll_callback(GLFWwindow* /*window*/, double /*xoff*/, double yoff)
{
    Cam::distance -= static_cast<float>(yoff) * Cam::radius * 0.1f;
    // yakınlık sınırları
    Cam::distance = std::max(Cam::distance, Cam::radius * 0.5f);
    Cam::distance = std::min(Cam::distance, Cam::radius * 20.0f);
}

// -----------------------------------------------------------------------------
void processInput(GLFWwindow *window, Robot *robot)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float moveSpeed = robot->speed * deltaTime;

    // İleri / geri
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        robot->position += robot->direction * moveSpeed;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        robot->position -= robot->direction * moveSpeed;

    // Yanlara straf
    glm::vec3 right = glm::normalize(glm::cross(robot->direction, glm::vec3(0.0f, 1.0f, 0.0f)));
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        robot->position += right * moveSpeed;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        robot->position -= right * moveSpeed;

    // Yön dönüşü (sol / sağ ok)
    float rotSpeed = 90.0f * deltaTime; // derece / sn
    if (glfwGetKey(window, GLFW_KEY_LEFT)  == GLFW_PRESS)
        robot->direction = glm::rotateY(robot->direction,  glm::radians( rotSpeed));
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        robot->direction = glm::rotateY(robot->direction,  glm::radians(-rotSpeed));
}

// -----------------------------------------------------------------------------
int main()
{
    // 1) GLFW ------------------------------------------------------
    if (!glfwInit()) {
        std::cerr << "GLFW init failed\n";
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 2) Pencere ----------------------------------------------------
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Virtual Museum", nullptr, nullptr);
    if (!window) {
        std::cerr << "Window creation failed\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Callbacks
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback     (window, cursor_position_callback);
    glfwSetMouseButtonCallback   (window, mouse_button_callback);
    glfwSetScrollCallback        (window, scroll_callback);

    // 3) GLAD -------------------------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }
    glEnable(GL_DEPTH_TEST);

    // 4) ImGui ------------------------------------------------------
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    // 5) Uygulama nesneleri ---------------------------------------
    Shader shader("shaders/vertex.glsl", "shaders/fragment.glsl");

    Scene scene;
    scene.init();

    // --- Sahne sınır kutusu yalnızca 1 kez -----------------------
    scene.getSceneBounds(Cam::center, Cam::radius);
    Cam::distance = Cam::radius / std::tan(glm::radians(Cam::fov * 0.5f)) + Cam::radius * 0.5f; // güvenli mesafe

    Robot     robot;
    UIManager ui(&robot, &scene, &shader);

    // -----------------------------------------------------------------
    // ANA DÖNGÜ
    // -----------------------------------------------------------------
    while (!glfwWindowShouldClose(window))
    {
        // Zaman --------------------------------------------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window, &robot);

        // ---------- ImGui -------------------------------------------
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Uygulama güncelleme
        robot.update(deltaTime);

        // ---------- Temizle ----------------------------------------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();

        // ---------- Kamera & Projeksiyon ---------------------------
        glm::vec3 camPos = Cam::position();
        shader.setVec3("viewPos", camPos);

        int w, h;
        glfwGetFramebufferSize(window, &w, &h);
        float aspect = static_cast<float>(w) / static_cast<float>(h);

        glm::mat4 view = glm::lookAt(camPos, Cam::center, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 proj = glm::perspective(glm::radians(Cam::fov), aspect, Cam::radius * 0.01f, Cam::distance + Cam::radius * 2.0f);
        shader.setMat4("view",       view);
        shader.setMat4("projection", proj);

        // ---------- Aydınlatma ------------------------------------
        shader.setInt  ("numSpotLights", 1);
        shader.setVec3 ("spotLights[0].position",  glm::vec3(0.0f, 5.0f, 0.0f));
        shader.setVec3 ("spotLights[0].direction", glm::vec3(0.0f, -1.0f, 0.0f));
        shader.setFloat("spotLights[0].cutOff",      glm::cos(glm::radians(12.5f)));
        shader.setFloat("spotLights[0].outerCutOff", glm::cos(glm::radians(17.5f)));
        shader.setVec3 ("spotLights[0].ambient",  glm::vec3(0.2f));
        shader.setVec3 ("spotLights[0].diffuse",  glm::vec3(0.6f));
        shader.setVec3 ("spotLights[0].specular", glm::vec3(1.0f));
        shader.setFloat("spotLights[0].constant",  1.0f);
        shader.setFloat("spotLights[0].linear",    0.09f);
        shader.setFloat("spotLights[0].quadratic", 0.032f);

        // ---------- Çizim ----------------------------------------
        scene.draw(shader);
        robot.draw(shader);
        ui.render();

        // ---------- ImGui Render ---------------------------------
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // ---------- GLFW -----------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // -----------------------------------------------------------------
    // Kapat / temizlik
    // -----------------------------------------------------------------
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}
