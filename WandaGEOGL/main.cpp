#include "NKWindow/NkWindow.h"
#include "NKWindow/Core/NkMain.h"
#include "NKLogger/NkLog.h"

#include "NKGlad/include/glad/wgl.h"
#include "NKGlad/include/glad/gl.h"

#if defined(Bool)
    #undef Bool
#endif

#include "NKContext/Factory/NkContextFactory.h"
#include "NKContext/Core/NkContextDesc.h"
#include "NKContext/Core/NkNativeContextAccess.h"
#include "NKContext/Core/NkOpenGLDesc.h"

#include "NKEvent/NkKeyboardEvent.h"
#include "NKEvent/NkWindowEvent.h"

#include "nkMath/NkMat4x4.h"

#include "engine/ShaderProgram.h"
#include "engine/Mesh.h"
#include "engine/Camera.h"
#include "engine/UniformBuffer.h"
#include "engine/ImGuiLayer.h"
#include "imgui.h"
#include "engine/Primitive.h"
#include "engine/Texture2D.h"

#include <vector>
#include <iostream>

using namespace nkentseu;

// Contrat d'alignement std140 rigoureux
//
// Layout réel attendu par le bloc GLSL "CommonData" (std140) :
//   vec2  u_Resolution        -> offset 0  (8 octets)
//   float u_Time              -> offset 8
//   float u_DeltaTime         -> offset 12
//   int   u_EffectIndex       -> offset 16
//   float u_SpeedMultiplier   -> offset 20
//   mat4  u_View              -> offset 32 (aligné sur 16, padding auto de 8 octets ici)
//   mat4  u_Proj              -> offset 96
//
// IMPORTANT : un vec2 ne force pas d'alignement 16 pour les champs suivants
// (seuls vec3/vec4/mat sont alignés sur 16). Ajouter un padding manuel ici
// décale TOUS les offsets de 8 octets et désynchronise effectIndex / la
// matrice view-proj entre le C++ et le shader.
struct EngineUBOData {
    float resX, resY;          // offset 0  -> u_Resolution
    float time;                // offset 8  -> u_Time
    float deltaTime;           // offset 12 -> u_DeltaTime
    int effectIndex;           // offset 16 -> u_EffectIndex
    float speedMultiplier;     // offset 20 -> u_SpeedMultiplier
    float _pad0[2];            // offset 24 -> padding explicite jusqu'à 32 (alignement du mat4)
    float viewMatrix[16];      // offset 32 -> u_View
    float projMatrix[16];      // offset 96 -> u_Proj
};


static bool InitOpenGL(NkIGraphicsContext* ctx) {
    auto loader = NkNativeContext::GetOpenGLProcAddressLoader(ctx);
    if (!loader || !gladLoadGL((GLADloadfunc)loader)) return false;
    return true;
}

int nkmain(const nkentseu::NkEntryState& /*state*/) {
    NkWindowConfig cfg;
    cfg.title = "Wanda Graphics Engine OpenGL";
    cfg.width = 1280;
    cfg.height = 720;
    cfg.centered = true;
    cfg.resizable = true;

    NkWindow window;
    if (!window.Create(cfg)) return -1;

    NkContextDesc desc;
    desc.api = NkGraphicsApi::NK_API_OPENGL;
    desc.opengl.majorVersion = 4;
    desc.opengl.minorVersion = 6;
    desc.opengl.profile = NkGLProfile::Core;

    auto ctx = NkContextFactory::Create(window, desc);
    if (!ctx || !InitOpenGL(ctx)) { window.Close(); return -2; }

    // Init de la couche ImGui isolée
    ImGuiLayer::Init(window);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    Shader pipelineShader("shaders/engine.vert", "shaders/engine.frag");
    UniformBuffer globalUBO;
    globalUBO.Init(sizeof(EngineUBOData), 0);

    // // Initialisation des Meshes à l'écran
    // std::vector<Vertex> triVertices = {
    //     { NkVec3f(-0.6f, -0.5f, 0.0f), NkVec3f(1.0f, 0.0f, 0.0f), 0.0f, 0.0f },
    //     { NkVec3f( 0.0f, -0.5f, 0.0f), NkVec3f(0.0f, 1.0f, 0.0f), 1.0f, 0.0f },
    //     { NkVec3f(-0.3f,  0.5f, 0.0f), NkVec3f(0.0f, 0.0f, 1.0f), 0.5f, 1.0f }
    // };
    // Mesh triangle; triangle.Init(triVertices);

    // std::vector<Vertex> quadVertices = {
    //     { NkVec3f(0.2f,  0.5f, 0.0f), NkVec3f(1.0f, 1.0f, 0.0f), 0.0f, 1.0f },
    //     { NkVec3f(0.8f,  0.5f, 0.0f), NkVec3f(0.0f, 1.0f, 1.0f), 1.0f, 1.0f },
    //     { NkVec3f(0.8f, -0.5f, 0.0f), NkVec3f(1.0f, 0.0f, 1.0f), 1.0f, 0.0f },
    //     { NkVec3f(0.2f, -0.5f, 0.0f), NkVec3f(1.0f, 1.0f, 1.0f), 0.0f, 0.0f }
    // };
    // // std::vector<unsigned int> quadIndices = { 0, 1, 2, 2, 3, 0 };
    // std::vector<unsigned int> quadIndices = { 0, 3, 2, 2, 1, 0 };
    // Mesh quad; quad.Init(quadVertices, quadIndices);

    Texture2D blockTexture;
    if (!blockTexture.load("assets/sun.jpg")) {
        // NkLog::Error("Impossible de charger la texture !");
    }

    size_t vertexCount = 0, triangleCount = 0;
    std::vector<Object> sceneObjects;

    // Objet 1 : Un cube au centre
    Object cube = CreateCube3D(blockTexture.id);
    cube.position[2] = -2.0f; // Reculé en Z pour que la caméra le voie
    sceneObjects.push_back(cube);

    // Objet 2 : Un second objet décalé sur le côté
    Object secondCube = CreateCube3D(blockTexture.id);
    secondCube.position[0] = 1.5f;
    secondCube.scale[0] = 0.5f; // Écrasé sur l'axe X
    sceneObjects.push_back(secondCube);

    Camera camera;
    EngineUBOData uboData;
    uboData.effectIndex = 0;
    uboData.speedMultiplier = 1.0f;

    float clearColor[3] = { 0.1f, 0.1f, 0.15f };
    bool wireframe = false;
    bool depthTest = true;
    bool culling = true;
    int spawnCount = 0; // Compteur utilisé pour décaler la position des objets ajoutés via UI
    bool animationPlaying = true; // Play/Pause global (rotation des objets + effets shader animés)
    int selectedObjectIndex = 0;  // Objet actuellement sélectionné dans le panneau d'inspection

    bool running = true;
    auto& events = NkEvents();

    float runTime = 0.0f;
    const float dt = 0.016f; // Simulation stable à 60 Hz

    while (running) {
        while (NkEvent* ev = events.PollEvent()) {
            if (ev->Is<NkWindowCloseEvent>()) { running = false; break; }
            
            // On route vers ImGui en premier. S'il consomme l'input, on bloque le traitement de la scène
            if (ImGuiLayer::HandleNKEvent(ev)) continue;

            // Système d'entrées de secours caméra si ImGui est inactif
            if (auto* k = ev->As<NkKeyPressEvent>()){
                switch (k->GetKey()) {
                    case NkKey::NK_W:
                        camera.ProcessKeyboard(0, dt);
                        break;
                    case NkKey::NK_S:
                        camera.ProcessKeyboard(1, dt);
                        break;
                    case NkKey::NK_A:
                        camera.ProcessKeyboard(2, dt);
                        break;
                    case NkKey::NK_D:
                        camera.ProcessKeyboard(3, dt);
                        break;
                    default:
                        break;
                }
            }
            if (auto* k = ev->As<NkKeyRepeatEvent>()){
                switch (k->GetKey()) {
                    case NkKey::NK_W:
                        camera.ProcessKeyboard(0, dt);
                        break;
                    case NkKey::NK_S:
                        camera.ProcessKeyboard(1, dt);
                        break;
                    case NkKey::NK_A:
                        camera.ProcessKeyboard(2, dt);
                        break;
                    case NkKey::NK_D:
                        camera.ProcessKeyboard(3, dt);
                        break;
                    default:
                        break;
                }
            }
        }

        if (!running) break;

        if (animationPlaying) runTime += dt;

        if (ctx->BeginFrame()) {
            // Configuration dynamique des états OpenGL via l'interface
            glViewport(0, 0, window.GetSize().width, window.GetSize().height);
            glClearColor(clearColor[0], clearColor[1], clearColor[2], 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            if (depthTest) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
            if (culling) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
            glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);

            // Remplissage structure UBO alignée
            uboData.resX = (float)window.GetSize().width;
            uboData.resY = (float)window.GetSize().height;
            uboData.time = runTime;
            uboData.deltaTime = dt;
            camera.GetViewMatrix(uboData.viewMatrix);
            camera.GetProjectionMatrix(uboData.resX / uboData.resY, uboData.projMatrix);
            globalUBO.Update(0, sizeof(EngineUBOData), &uboData);

            // Phase de Dessin
            pipelineShader.use();

            size_t vCount = 0 , tCount = 0;
            for (auto& obj : sceneObjects) {
                // Chaque objet tourne sur lui-même en plus de sa transformation propre
                // (figé si l'animation est en pause, puisque uboData.time n'avance plus)
                float spin = uboData.time * 50.0f; // 50 degrés par seconde
                float rotation[3] = {
                    obj.rotation[0] + spin,
                    obj.rotation[1] + spin * 0.5f,
                    obj.rotation[2]
                };


                // 1. Calcul de la matrice TRS via votre NkMat4x4 complété
                NkMat4f modelMatrix = NkMat4f::Transformation3D(obj.position, rotation, obj.scale);

                // 2. Export au format plat OpenGL
                float glMatrix[16];
                modelMatrix.ToPtr(glMatrix);

                // 3. Envoi au shader juste avant le Draw
                pipelineShader.use();
                pipelineShader.setMat4("u_Model", glMatrix);
                pipelineShader.setVec3("u_Tint", obj.tint);
                pipelineShader.setBool("u_UseTexture", obj.useTexture);

                if (obj.useTexture) {
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, obj.textureID);
                    pipelineShader.setInt("u_TextureSampler", 0);
                }

                // 4. Rendu de l'objet
                obj.mesh.Draw();

                

                vCount += obj.mesh.GetVertexCount();
                tCount += obj.mesh.GetTriangleCount();
            }
            triangleCount = tCount;
            // tCount = 0;
            vertexCount = vCount;
            // vCount =0;

            // triangle.Draw();
            // quad.Draw();

            // Rendu de l'interface ImGui
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Forcer le mode plein pour ImGui
            ImGuiLayer::BeginFrame();

            // Récupération des dimensions courantes de la fenêtre pour le layout dynamique
            float winWidth  = (float)window.GetSize().width;
            float winHeight = (float)window.GetSize().height;
            float margin = 10.0f;

            // Largeurs et hauteurs par défaut souhaitées pour les calculs de placement
            float panelWidthLeft   = 300.0f;
            float panelWidthRight  = 350.0f;
            float panelHeightLower = 150.0f;

            // 1. Panneau Performance (Ancré En haut à gauche)
            ImGui::SetNextWindowPos(ImVec2(margin, margin), ImGuiCond_Always);
            ImGui::SetNextWindowSize(ImVec2(panelWidthLeft, 140), ImGuiCond_FirstUseEver);
            ImGui::Begin("Statistiques de Performance");
                ImGui::Text("FPS : %.1f", ImGui::GetIO().Framerate);
                ImGui::Text("Frame Time : %.3f ms", 1000.0f / ImGui::GetIO().Framerate);
                ImGui::Separator();
                ImGui::Text("Sommets Actifs : %zu", vertexCount);
                ImGui::Text("Triangles Totaux : %zu", triangleCount);
            ImGui::End();

            // 2. Panneau États OpenGL (Ancré En bas à gauche)
            // Position Y calculée : Hauteur totale - Hauteur du panneau - Marge
            ImGui::SetNextWindowPos(ImVec2(margin, winHeight - panelHeightLower - margin), ImGuiCond_Always);
            ImGui::SetNextWindowSize(ImVec2(panelWidthLeft, panelHeightLower), ImGuiCond_FirstUseEver);
            ImGui::Begin("Configurations OpenGL Core");
                ImGui::ColorEdit3("Fond d'écran", clearColor);
                ImGui::Checkbox("Test de Profondeur (Depth Test)", &depthTest);
                ImGui::Checkbox("Face Culling", &culling);
                ImGui::Checkbox("Mode Filaire (Wireframe)", &wireframe);
            ImGui::End();

            // 3. Panneau Shaders (Ancré En haut à droite)
            // Position X calculée : Largeur totale - Largeur du panneau - Marge
            ImGui::SetNextWindowPos(ImVec2(winWidth - panelWidthRight - margin, margin), ImGuiCond_Always);
            ImGui::SetNextWindowSize(ImVec2(panelWidthRight, 120), ImGuiCond_FirstUseEver);
            ImGui::Begin("Effets Procéduraux & Shaders");
                const char* effets[] = { "Aucun (Texture Brute)", "SDF Disque Pulsé", "SDF Rectangle Fixe", "Fractale Mandelbrot", "Value Noise Dynamique", "Scrolling Matrix UV", "Morphing Interpolé" };
                ImGui::Combo("Effet Courant", &uboData.effectIndex, effets, IM_ARRAYSIZE(effets));
                ImGui::SliderFloat("Vitesse de l'Effet", &uboData.speedMultiplier, 0.0f, 5.0f);
                ImGui::Separator();
                if (ImGui::Button(animationPlaying ? "Pause" : "Lecture", ImVec2(100, 0))) {
                    animationPlaying = !animationPlaying;
                }
                ImGui::SameLine();
                ImGui::Text(animationPlaying ? "(en cours)" : "(en pause)");
            ImGui::End();

            // 3bis. Panneau Inspecteur d'Objet (Ancré sous le panneau Effets, à droite)
            float panelHeightInspector = 160.0f;
            ImGui::SetNextWindowPos(ImVec2(winWidth - panelWidthRight - margin, margin + 130.0f), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowSize(ImVec2(panelWidthRight, panelHeightInspector), ImGuiCond_FirstUseEver);
            ImGui::Begin("Inspecteur d'Objet");
                if (sceneObjects.empty()) {
                    ImGui::TextDisabled("Aucun objet dans la scène.");
                } else {
                    // Garde l'index sélectionné dans une plage valide si des objets ont été supprimés
                    if (selectedObjectIndex >= (int)sceneObjects.size()) {
                        selectedObjectIndex = (int)sceneObjects.size() - 1;
                    }

                    ImGui::SliderInt("Objet sélectionné", &selectedObjectIndex, 0, (int)sceneObjects.size() - 1);

                    Object& selected = sceneObjects[selectedObjectIndex];
                    ImGui::Separator();
                    ImGui::ColorEdit3("Teinte", selected.tint);
                    ImGui::Checkbox("Utiliser la texture", &selected.useTexture);
                    if (!selected.useTexture) {
                        ImGui::TextDisabled("Couleur unie (teinte seule)");
                    }
                }
            ImGui::End();

            // 4. Panneau Caméra (Ancré En bas à droite)
            // Position X et Y dynamiques pour coller au coin inférieur droit
            float panelHeightCam = 130.0f;
            ImGui::SetNextWindowPos(ImVec2(winWidth - panelWidthRight - margin, winHeight - panelHeightCam - margin), ImGuiCond_Always);
            ImGui::SetNextWindowSize(ImVec2(panelWidthRight, panelHeightCam), ImGuiCond_FirstUseEver);
            ImGui::Begin("Inspecteur Caméra 3D");
                ImGui::Text("Position : (X: %.2f, Y: %.2f, Z: %.2f)", camera.position[0], camera.position[1], camera.position[2]);
                ImGui::SliderFloat("Champ de Vision (FOV)", &camera.fov, 10.0f, 120.0f);
                ImGui::SliderFloat("Vitesse Caméra", &camera.speed, 0.5f, 10.0f);
            ImGui::End();

            // 5. Panneau Ajout d'Objets (Ancré En bas au centre)
            float panelWidthAdd  = 320.0f;
            float panelHeightAdd = 170.0f;
            ImGui::SetNextWindowPos(ImVec2((winWidth - panelWidthAdd) * 0.5f, winHeight - panelHeightAdd - margin), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowSize(ImVec2(panelWidthAdd, panelHeightAdd), ImGuiCond_FirstUseEver);
            ImGui::Begin("Ajouter un Objet");
                ImGui::Text("Objets dans la scène : %zu", sceneObjects.size());
                ImGui::Separator();

                // Décalage en X pour que chaque nouvel objet n'apparaisse pas
                // empilé sur les précédents. Repart à gauche après 5 objets sur la rangée.
                auto spawnPosition = [&]() -> float {
                    float x = -2.0f + 1.5f * (float)(spawnCount % 5);
                    spawnCount++;
                    return x;
                };

                if (ImGui::Button("Ajouter Cube", ImVec2(95, 0))) {
                    Object obj = CreateCube3D(blockTexture.id);
                    obj.position[0] = spawnPosition();
                    obj.position[2] = -2.0f;
                    sceneObjects.push_back(obj);
                }
                ImGui::SameLine();
                if (ImGui::Button("Ajouter Sphère", ImVec2(95, 0))) {
                    Object obj = CreateSphere3D(blockTexture.id);
                    obj.position[0] = spawnPosition();
                    obj.position[2] = -2.0f;
                    sceneObjects.push_back(obj);
                }
                ImGui::SameLine();
                if (ImGui::Button("Ajouter Pyramide", ImVec2(105, 0))) {
                    Object obj = CreatePyramid3D(blockTexture.id);
                    obj.position[0] = spawnPosition();
                    obj.position[2] = -2.0f;
                    sceneObjects.push_back(obj);
                }

                if (ImGui::Button("Ajouter Quad 2D", ImVec2(145, 0))) {
                    Object obj = CreateQuad2D(blockTexture.id);
                    obj.position[0] = spawnPosition();
                    obj.position[2] = -2.0f;
                    sceneObjects.push_back(obj);
                }
                ImGui::SameLine();
                if (ImGui::Button("Ajouter Triangle 2D", ImVec2(155, 0))) {
                    Object obj = CreateTriangle2D(blockTexture.id);
                    obj.position[0] = spawnPosition();
                    obj.position[2] = -2.0f;
                    sceneObjects.push_back(obj);
                }

                ImGui::Separator();
                if (ImGui::Button("Vider la scène", ImVec2(140, 0))) {
                    for (auto& obj : sceneObjects) obj.mesh.Destroy();
                    sceneObjects.clear();
                    spawnCount = 0;
                    selectedObjectIndex = 0;
                }
            ImGui::End();

            ImGuiLayer::EndFrame();

            ctx->EndFrame();
            ctx->Present();
        }
    }

    // Libération RAII
    // triangle.Destroy();
    // quad.Destroy();
    for (auto& obj : sceneObjects) {
        obj.mesh.Destroy();
    }
    glDeleteTextures(1, &blockTexture.id);
    globalUBO.Destroy();
    pipelineShader.Destroy();
    ImGuiLayer::Shutdown();

    ctx->Shutdown();
    window.Close();
    return 0;
}