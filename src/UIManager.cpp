// UIManager.cpp
#include "UIManager.h"
#include <imgui.h>
#include <glm/gtx/string_cast.hpp>

UIManager::UIManager(Robot *r, Scene *s, Shader *sh)
    : robot(r), scene(s), shader(sh)
{
    // Initialize object positions matching Scene::initModels()
    objectPositions = {
        {2.0f, 0.5f, 3.0f},  // Changed y to 0.5
        {-3.0f, 0.5f, 1.0f}, // Changed y to 0.5
        {0.0f, 0.5f, -2.5f}, // Changed y to 0.5
        {4.0f, 0.5f, -1.0f}, // Changed y to 0.5
        {-1.5f, 0.5f, -3.5f} // Changed y to 0.5
    }; // Descriptions for each object
    infoStrings = {
        "Heykel: Antik döneme ait taş heykel.",
        "Manstatue: Orta Çağ'dan kalma insan heykeli.",
        "Modelleme: Modern sanat eseri.",
        "Roma Mezar Modeli: Roma dönemine ait mezar maketi.",
        "Roma Yeni: Yenilenmiş Roma dönemi lahdi."};
}

void UIManager::render()
{
    // Main control window
    ImGui::Begin("Controls");
    if (autoTour)
    {
        if (ImGui::Button("Stop Auto Tour"))
        {
            autoTour = false;
            robot->setPath({});
        }
    }
    else
    {
        if (ImGui::Button("Start Auto Tour"))
        {
            autoTour = true;
            robot->setPath(objectPositions);
        }
    }
    ImGui::Separator();
    ImGui::Checkbox("Auto Tour", &autoTour);
    ImGui::SliderFloat3("Robot Position", &robot->position.x, -10.0f, 10.0f);
    ImGui::Text("Robot Direction: %s", glm::to_string(robot->direction).c_str());
    ImGui::End();

    // Proximity detection for pop-up
    if (!showInfo && autoTour)
    {
        for (int i = 0; i < (int)objectPositions.size(); ++i)
        {
            float dist = glm::distance(robot->position, objectPositions[i]);
            if (dist < scanThreshold)
            {
                showInfo = true;
                currentObject = i;
                break;
            }
        }
    }

    // Info pop-up window
    if (showInfo)
    {
        ImGui::Begin("Object Info");
        ImGui::TextWrapped("%s", infoStrings[currentObject].c_str());
        if (ImGui::Button("Continue Tour"))
        {
            showInfo = false;
        }
        ImGui::End();
    }
}
