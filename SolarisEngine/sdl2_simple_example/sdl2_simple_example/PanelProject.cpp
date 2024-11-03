﻿#include "PanelProject.h"
#include "imgui.h"
#include "Defs.h"
#include "App.h"
#include "stb_image.h"
#include "TextureLoader.h"


namespace fs = std::filesystem;
// Constructor
PanelProject::PanelProject() {
    projectPath = ASSETS_DIR;
    icons["folder"] = (int*)app->textureLoader->LoadTextureDevIL("./Assets/file2.png");
    icons["file"] = (int*)app->textureLoader->LoadTextureDevIL("./Assets/file2.png");
    icons["image"] = (int*)app->textureLoader->LoadTextureDevIL("./Assets/file2.png");
    currentPath = projectPath; // ruta actual
    selectedItem = ""; // item selecionado
    pathStack.push(currentPath);
}

// Destructor
PanelProject::~PanelProject() {
	// Cleanup if necessary
}

void PanelProject::Render() {
    if (!app->windowEditor->GetImGuiWindow()->showprojectPanel) {
        return;
    }

    ImGui::Begin("Project Explorer");

    // breadnavigation
    ImGui::BeginChild("BreadcrumbNavigation", ImVec2(0, 20), false, ImGuiWindowFlags_NoScrollbar);
    ShowBreadcrumbNavigation();
    ImGui::EndChild();

    
    ImGui::Separator(); 

    // ruta y arbol
    ShowFileSystemTree(currentPath);

    ImGui::End();
}

void PanelProject::ShowFileSystemTree(const std::filesystem::path& path) {
    const float iconSize = 64.0f;
    const float padding = 20.0f;
    const float textMaxWidth = iconSize;
    const float itemTotalWidth = iconSize + padding;

    float panelWidth = ImGui::GetContentRegionAvail().x;
    int maxItemsPerRow = static_cast<int>(panelWidth / itemTotalWidth);
    if (maxItemsPerRow < 1) maxItemsPerRow = 1;

    int itemsInRow = 0;

    for (const auto& entry : fs::directory_iterator(path)) {
        const auto& entryPath = entry.path();
        std::string fileName = entryPath.filename().string();
        bool isDirectory = entry.is_directory();
        bool isHovered = false;
        bool isSelected = (fileName == selectedItem);

        ImGui::BeginGroup();

        // que icono
        void* icon;
        if (isDirectory) {
            icon = icons["folder"]; // carpeta
        }
        else {
            std::string extension = entryPath.extension().string();
            if (extension == ".png" || extension == ".jpg" || extension == ".jpeg") {
                icon = icons["image"]; // foto
            }
            else if (extension == ".mp3" || extension == ".wav") {
                icon = icons["audio"]; // audio
            }
            else if (extension == ".txt" || extension == ".md") {
                icon = icons["text"]; // texto
            }
            else {
                icon = icons["file"]; // otros
            }
        }

        // fondo trasparente
        ImGui::PushStyleColor(ImGuiCol_Button, isSelected ? ImVec4(0.0f, 0.0f, 1.0f, 0.5f) : ImVec4(0, 0, 0, 0));

        
        isHovered = ImGui::ImageButton((ImTextureID)icon, ImVec2(iconSize, iconSize));

        if (isHovered && isDirectory) {
            selectedItem = fileName; 
            pathStack.push(currentPath); 
            currentPath = entryPath;
        }
        else if (isHovered && !isDirectory) {
            selectedItem = fileName; 
           
        }

        ImGui::PopStyleColor(); 

        // nombre de fitxe
        ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + textMaxWidth);
        float textWidth = ImGui::CalcTextSize(fileName.c_str()).x;
        if (textWidth > textMaxWidth) {
            fileName = fileName.substr(0, 10) + "...";
        }

        float centeredTextPos = ImGui::GetCursorPosX() + (iconSize - std::min(textWidth, textMaxWidth)) / 2;
        ImGui::SetCursorPosX(centeredTextPos);
        ImGui::Text("%s", fileName.c_str());
        ImGui::PopTextWrapPos();
        ImGui::EndGroup();

        itemsInRow++;
        if (itemsInRow >= maxItemsPerRow) {
            ImGui::NewLine();
            itemsInRow = 0;
        }
        else {
            ImGui::SameLine(0, padding);
        }
    }
}


void PanelProject::ShowBreadcrumbNavigation() {
    std::filesystem::path pathPart;
    bool showBreadcrumb = false;

  
    for (const auto& part : currentPath) {
        if (part == "Assets") {
            showBreadcrumb = true; 
        }

        if (showBreadcrumb) {
            pathPart /= part;

            
            if (ImGui::Button(part.string().c_str())) {
               
                currentPath = pathPart;

               
                while (!pathStack.empty() && pathStack.top() != currentPath) {
                    pathStack.pop();
                }
                pathStack.push(currentPath); 
                selectedItem = ""; 
                break;
            }

            
            ImGui::SameLine();
            ImGui::Text(">");
            ImGui::SameLine();
        }
    }
}








