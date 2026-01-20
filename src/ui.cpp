// #include "vendor/imgui/imgui_internal.h"
// #include "glm/glm.hpp"
#include "ui.hpp"
#include <cstdio>
#include <glm/gtc/matrix_transform.hpp>
#include "common.hpp"
#include "render.hpp"
#include "utils.hpp"
#include "vendor/imgui/imgui.h"

bool fileWinOpen = 0,terminalOpen = 0;
unsigned int fileIndexOpen;
char *filePath;
ImVec2 delta;

void Render::ui() {
    ImGuiIO& io = ImGui::GetIO();
    ImGuiViewport *view = ImGui::GetMainViewport();

    // Dimensões totais da janela principal
    int totalLeng = view->Size.x;
    int totalHeight = view->Size.y;

    int k = 30;  // Altura da barra de menus
    int endx = 40;
    ImGuiID dockspace_id = ImGuiID(), dockspace_id2;
    dockspace_id2 = dockspace_id;
    ImGui::NewFrame();

    if( ImGui::BeginMainMenuBar() ) {

        if( ImGui::BeginMenu("File") ) {
            ImGui::MenuItem("Pinto");
            ImGui::EndMenu();
        }

        if( ImGui::BeginMenu("Preferences") ) {
            // ImGui::MenuItem("Scale");
            // io.FontGlobalScale = 1.3;
            ImGui::SliderFloat("Scale", &io.FontGlobalScale, .8, 2);
            ImGui::EndMenu();
        }

        if( ImGui::BeginMenu("Models") ) {
            if( ImGui::BeginMenu("Standarts") ) {
                ImGui::MenuItem("Cube");
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    int x = 0, actualLenght;

    ImGui::SetNextWindowPos(ImVec2(x, k));

    // Largura: 300px em 1920x1080
    actualLenght = (int)(300.0/1920.0 * totalLeng);

    ImGui::SetNextWindowSize(ImVec2(actualLenght, totalHeight-endx-k));
    x += actualLenght;

    // Cria janela com docking para Debug e World
    if( ImGui::Begin("Debug & World", nullptr, ImGuiWindowFlags_NoTitleBar) ) {
        dockspace_id2 = ImGui::GetID("MyDockSpacePintassimo");
        ImGui::DockSpace(dockspace_id2, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
    }
    ImGui::End();

    ImGui::SetNextWindowDockID(dockspace_id2, ImGuiCond_Once);
    if( ImGui::Begin("Debug", nullptr, ImGuiWindowFlags_None) ) {
        ImGui::Text("Double Click: %d",io.MouseDoubleClicked[0]);
        // Conteúdo da janela Debug
    }
    ImGui::End();

    ImGui::SetNextWindowDockID(dockspace_id2, ImGuiCond_Once);
    if(ImGui::Begin("World", nullptr, ImGuiWindowFlags_None)) {
        // if( ImGui::Button("Add") ) {
        //     if(selectedModelIndex < renderData.layers[selectedModelLayer].models.size()) {
        //         // const char *meshPath = renderData.LayersData[selectedModelLayer].models[selectedModelIndex].mesh.meshPath;
        //     }
        // }

        // ImGui::SameLine();

        // if( ImGui::Button("Remove") ) {
        //     if(renderData.layers[selectedModelLayer].models.size() > selectedModelIndex) {
        //         renderData.popModels(selectedModelIndex, selectedModelLayer);
        //         if(selectedModelIndex > 0) selectedModelIndex--;
        //     }
        // }

        ImGui::Text("GameObjects");

        if( ImGui::BeginTable("GameObjects", 1) ) {
            for( unsigned int i = 0; i < objects_count; i++ ) {

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                if(i == selected_object) {
                    ImGui::TableSetBgColor(
                        ImGuiTableBgTarget_RowBg0,
                        IM_COL32(138, 38, 38, 255)
                    );
                }
                if(ImGui::Selectable(objects[i].name)) {
                    selected_object = i;
                }

            }
            ImGui::EndTable();
        }
    }
    ImGui::End();


    ImGui::SetNextWindowPos(ImVec2(x, k));

    actualLenght = (int)(1220.0/1920.0 * totalLeng);

    int b = (int)(670.0/1920.0 * totalLeng);

    ImGui::SetNextWindowSize(ImVec2(actualLenght, b));

    if( ImGui::Begin("Central Space", nullptr, dockspace_flags) ) {
        dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
    }

    ImGui::End();


    ImGui::SetNextWindowDockID(dockspace_id, ImGuiCond_Once);
    if( ImGui::Begin("Game", nullptr, ImGuiWindowFlags_None) ) {
        input();
        ImGui::GetWindowDrawList()->AddImage((ImTextureRef)texToShowFrom,
                                            ImGui::GetWindowPos(),
                                            ImGui::GetWindowSize() + ImGui::GetWindowPos(),
                                            ImVec2(0, 1),
                                            ImVec2(1, 0));
    }

    ImGui::End();

    ImGui::SetNextWindowDockID(dockspace_id, ImGuiCond_Once);
    if( ImGui::Begin("Final") ) {};
    ImGui::End();

    ImGui::SetNextWindowPos(ImVec2(x, b));
    ImGui::SetNextWindowSize(ImVec2(actualLenght, totalHeight -b - endx));
    x += actualLenght;

    if( ImGui::Begin("Assets", nullptr, ImGuiWindowFlags_None) ) {
        if( ImGui::BeginTable("Files",6) ) {
            for(unsigned int i = 0; i < assets.files.size(); i ++ ){
                if(! (i % 6) ) ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(i % 6);

                ImGui::Text("%s",assets.files[i].completeName);
                if( ImGui::IsItemHovered() && io.MouseDoubleClicked[0] ) {
                    fileIndexOpen = i;

                    switch(assets.files[i].type) {
                        case FOLDER_FILE:{
                            unsigned int lenght = strlen(assets.directory) + strlen(assets.files[i].completeName) + 1;
                            char *p = (char*)malloc(lenght);
                            memcpy(p, assets.directory, strlen(assets.directory));
                            memcpy(p + strlen(assets.directory), assets.files[i].completeName, strlen(assets.files[i].completeName));
                            p[lenght - 1] = 0;
                            free(assets.directory);
                            assets.directory = p;
                            assets.update();
                        } break;

                        case CODE_FILE:
                        case MESH_FILE:
                        case IMAGE_FILE:
                        case COMMON_FILE: {
                            filePath = (char*)malloc(strlen(assets.directory) + strlen(assets.files[i].completeName) + 1);
                            memcpy(filePath, assets.directory, strlen(assets.directory));
                            memcpy(filePath + strlen(assets.directory), assets.files[i].completeName, strlen(assets.files[i].completeName));
                            filePath[strlen(assets.directory) + strlen(assets.files[i].completeName)] = 0;
                            fileWinOpen = 1;
                        } break;
                    }
                }
            }

            ImGui::EndTable();
        }
    }
    ImGui::End();

    if( fileWinOpen ){

        ImGui::SetNextWindowPos(io.MousePos,ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(300,400),ImGuiCond_Once);
        if( ImGui::Begin("File Properties", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar) ) {
            switch(assets.files[fileIndexOpen].type) {
                case MESH_FILE:{
                    // if(ImGui::Button("Add Model")){
                    //     renderData.pushModels(MeshGenData{
                    //         .path = filePath,
                    //     },LAYER::COMMON_LAYER);
                    // }
                } break;
                default: break;
            }

            ImGui::Button("vaid");

            if(!ImGui::IsWindowFocused())
                fileWinOpen = 0;
        }

        ImGui::End();
    }

    ImGui::SetNextWindowPos(ImVec2(x, k));
    ImGui::SetNextWindowPos(ImVec2(x, k));

    actualLenght = (int)(400.0/1920.0 * totalLeng);

    ImGui::SetNextWindowPos(ImVec2(x, k));
    ImGui::SetNextWindowSize(ImVec2(actualLenght, totalHeight-endx-k));

    if( ImGui::Begin("Properties") ) {

        // if( renderData.layers[selectedModelLayer].models.size() > selectedModelIndex ) {

        //     Model &m = renderData.layers[selectedModelLayer].models[selectedModelIndex];
        //     glm::vec3 transf[3] = {m.getPosition(),m.getAngle(),m.getScale()};

        //     if(ImGui::Button("virar para camera")){
        //         camera.updateRotation(utils.aim_matrix(-camera.getPosition()));
        //         setAVector(camera.getPosition(), -camera.getPosition());
        //     }

        //     ImGui::Text("Transform:");
        //     ImGui::Text("   Position:( ");
        //     ImGui::SameLine();

        //     if( ImGui::InputFloat3("newPos ", &transf[0].x) ) {
        //         renderData.positionateModel(transf[0], selectedModelIndex, selectedModelLayer);
        //     }

        //     ImGui::Text("   Rotation:( ");
        //     ImGui::SameLine();

        //     if( ImGui::InputFloat3("newRot ", &transf[1].x) ) {
        //         renderData.setAngleModel(transf[1], selectedModelIndex, selectedModelLayer);
        //     }

        //     ImGui::Text("   Scale:( ");
        //     ImGui::SameLine();

        //     if( ImGui::InputFloat3("newSca ", &transf[2].x) ) {
        //         renderData.scaleModel(transf[2], selectedModelIndex, selectedModelLayer);
        //     }

        //     glm::vec3 pi = camera.getPosition();
        //     ImGui::Text("Camera:");
        //     ImGui::Text("   Position:(%f, %f , %f)",pi.x,pi.y,pi.z);
        //     glm::vec n = camera.getFoward();
        //     ImGui::Text("   Normal:(%f, %f , %f)",n.x,n.y,n.z);
        //     ImGui::Text("   Delta %f %f",delta.x,delta.y);
        // }

        // ImGui::Checkbox("normalA", &normalATIVO);
        // ImGui::SliderFloat("normalV", &normalV,.1, 2);
    }

    ImGui::End();

    if( terminalOpen ) {

        ImGui::SetNextWindowSize(ImVec2(600, 600));
        if( ImGui::Begin("Terminal"),terminalOpen) {

            ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(.40,.20,.70,1));
            if( ImGui::BeginChild("scroll", ImVec2(-40, -40)) )
            /* -> */ for( char *p: logger.terminal.lines ) ImGui::Text("%s",p);

            ImGui::EndChild();
            ImGui::PopStyleColor();

            char buff[256];
            if( ImGui::InputText("Command", buff, 256) )
            /* -> */ if( ImGui::IsKeyPressed(ImGuiKey_Enter) ) {
                    int lenght = strlen(buff);
                    char *p = (char*)malloc(lenght + 1);
                    memcpy(p, buff, lenght);
                    p[lenght] = 0;

                    logger.terminal.lines.push_back(p);
                    logger.terminal.update();
                }
        }

        ImGui::End();
    }

    ImGui::SetNextWindowPos(ImVec2(0, totalHeight-endx));
    ImGui::SetNextWindowSize(ImVec2(totalLeng, endx));

    if( ImGui::Begin("Terminal Bar", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar) ) {
        ImGui::Text("%s", logger.terminal.lines[logger.terminal.lines.size() - 1]);

        if( ImGui::IsWindowFocused() )
        /* -> */ if( io.MouseDoubleClicked[0] ) terminalOpen = 1;
    }

    ImGui::End();

    bool a = 0;
    ImGui::ShowDemoWindow(&a);
}

void Render::input() {
    glm::vec3 position = camera.getPosition();
    // if( selectedModelIndex < renderData.layers[selectedModelLayer].models.size() ) {
    //     renderData.positionateModel(renderData.layers[selectedModelLayer].models[selectedModelIndex].getPosition(), 0, LAYER::SPECIAL_LAYER);
    //     renderData.setAngleModel(renderData.layers[selectedModelLayer].models[selectedModelIndex].getAngle(), 0, LAYER::SPECIAL_LAYER);
    // }

    ImGuiIO& io = ImGui::GetIO();
    if( ImGui::IsWindowFocused() && ImGui::GetMousePos().y >= ImGui::GetCursorScreenPos().y ) {

        if( io.MouseDown[1] ) {
            ImVec2 mousePos = io.MousePos;
            ImVec2 windowCenter = ImGui::GetWindowSize() * .5f + ImGui::GetWindowPos();

            ImGui::SetMouseCursor(ImGuiMouseCursor_None);

            ImVec2 mouseDelta = ImVec2(0, 0);
            if( io.MouseDelta.x && io.MouseDelta.y ) mouseDelta = ImVec2(mousePos.x - windowCenter.x,mousePos.y - windowCenter.y);

            glfwSetCursorPos(glfwWin,windowCenter.x,windowCenter.y);
            ImGui::Dummy(ImVec2(0, 0));
            delta = mouseDelta;
            camera.angle.x += MOUSE_SENSI * mouseDelta.x;
            camera.angle.y += .5f * MOUSE_SENSI * mouseDelta.y;
            camera.rotation = glm::rotate(glm::mat4(1), glm::radians(camera.angle.x), glm::vec3(0, 1, 0));
            camera.rotation = glm::rotate(camera.rotation, glm::radians(camera.angle.y), glm::vec3(camera.getRight()));
            float b = .05;

            // renderData.scaleModel(glm::max(0.015f * glm::length(camera.getPosition() - renderData.getPositionOfModel(0, 0)), 0.00001f), 0, 0);

            // if( ImGui::IsKeyPressed(ImGuiKey_LeftArrow)  ) renderData.translateModel(glm::vec3(1,0,0),  selectedModelIndex, selectedModelLayer);
            // if( ImGui::IsKeyPressed(ImGuiKey_RightArrow) ) renderData.translateModel(glm::vec3(-1,0,0), selectedModelIndex, selectedModelLayer);
            // if( ImGui::IsKeyPressed(ImGuiKey_UpArrow)    ) renderData.translateModel(glm::vec3(0,0,1),  selectedModelIndex, selectedModelLayer);
            // if( ImGui::IsKeyPressed(ImGuiKey_DownArrow)  ) renderData.translateModel(glm::vec3(0,0,-1), selectedModelIndex, selectedModelLayer);

            if( ImGui::IsKeyPressed(ImGuiKey_A) ) position += b * glm::vec3(-camera.getRight());
            if( ImGui::IsKeyPressed(ImGuiKey_D) ) position += b * glm::vec3(camera.getRight());
            if( ImGui::IsKeyPressed(ImGuiKey_W) ) position += b * glm::vec3(camera.getFoward());
            if( ImGui::IsKeyPressed(ImGuiKey_S) ) position += b * glm::vec3(-camera.getFoward());


            // camera.updatePosition(position);
        }

        if( io.MouseReleased[1] ) ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
    }

    if( ImGui::IsKeyPressed(ImGuiKey_Escape) ) glfwSetWindowShouldClose(glfwWin, true);
}
