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

#include <cstdio>
#include <cmath>
#include <cstring>
#include <string>
#include <vector>
#include <iostream>

#include "ShaderProgram.h"
#include "Mesh.h"
#include "../nkMath/NkVec3.h"
    
using namespace nkentseu;
using NkVec3f = NkVec3<float>;

// ============================================================================
// SHADERS INLIÉS (INTEGRÉS DIRECTEMENT EN C++)
// ============================================================================
const char* vertexShaderSource = R"glsl(
#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
out vec3 ourColor;
void main()
{
    gl_Position = vec4(aPos, 1.0);
    ourColor = aColor;
}
)glsl";

const char* fragmentShaderSource =R"glsl(
#version 460 core
in vec3 ourColor;
out vec4 FragColor;
void main()
{
    FragColor = vec4(ourColor, 1.0);
}
)glsl";

// Fonction utilitaire pour compiler directement les chaînes de caractères ci-dessus
unsigned int CreateShaderFromSource(const char* vertexSource, const char* fragmentSource) {
    unsigned int vertex, fragment;
    int success;
    char infoLog[512];

    // 1. Vertex Shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertexSource, NULL);
    glCompileShader(vertex);
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        return 0;
    }

    // 2. Fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragmentSource, NULL);
    glCompileShader(fragment);
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        glDeleteShader(vertex);
        return 0;
    }

    // 3. Shader Program
    unsigned int programID = glCreateProgram();
    glAttachShader(programID, vertex);
    glAttachShader(programID, fragment);
    glLinkProgram(programID);
    glGetProgramiv(programID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(programID, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        return 0;
    }

    // Nettoyage immédiat des shaders intermédiaires
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    return programID;
}

static bool LoadGL(NkIGraphicsContext* ctx) {
    auto loader = NkNativeContext::GetOpenGLProcAddressLoader(ctx);
    if (!loader) { 
        logger.Info("[RHIDemo] OpenGL loader introuvable\n"); 
        return false; 
    }

    int ver = gladLoadGL((GLADloadfunc)loader);
    
    if (!ver) { 
        logger.Error("[RHIDemo] gladLoadGL echoue\n"); 
        return false; 
    }
    logger.Infof("[RHIDemo] OpenGL %s  GLSL %s\n",
    glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION));
    return true;
}

int nkmain(const nkentseu::NkEntryState& /*state*/) {
    
    NkWindowConfig cfg;
    cfg.title = "Graphic Engine - OpenGL";
    cfg.width = 1280;
    cfg.height = 720;
    cfg.centered = true;
    cfg.resizable = true;
    cfg.dropEnabled = false;

    NkWindow window;
    
    if (!window.Create(cfg)) {
        logger.Error("La fenetre n'a pas ete cree");
        return -2;
    }

    NkContextDesc desc;
    desc.api    = NkGraphicsApi::NK_API_OPENGL;
    desc.opengl.majorVersion = 4;
    desc.opengl.minorVersion = 6;
    desc.opengl.profile = NkGLProfile::Core;
    desc.opengl.contextFlags = NkGLContextFlags::ForwardCompat | NkGLContextFlags::Debug;
    desc.opengl.runtime.installDebugCallback = true;
    desc.opengl.msaaSamples        = 4;
    desc.opengl.srgbFramebuffer    = true;
    desc.opengl.swapInterval       = NkGLSwapInterval::AdaptiveVSync;
    desc.opengl.runtime.autoLoadEntryPoints  = true;
    desc.opengl.runtime.validateVersion      = true;

    auto ctx = NkContextFactory::Create(window, desc);
    if (!ctx)  { 
        logger.Error("[RHIDemo] Contexte GL echoue"); 
        window.Close(); 
        return -3; 
    }
    if (!LoadGL(ctx)) { 
        ctx->Shutdown(); 
        window.Close(); 
        return -4; 
    }

    glEnable(GL_DEPTH_TEST);

    // Initialisation de notre objet Shader (on utilise l'instance vide par défaut)
    Shader activeShader;
    
    // On compile directement à partir des chaînes de caractères brutes
    activeShader.ID = CreateShaderFromSource(vertexShaderSource, fragmentShaderSource);
    
    if (activeShader.ID != 0) {
        std::cout << "[Engine] Shaders integres compiles avec succes ! ID GPU: " << activeShader.ID << std::endl;
    } else {
        std::cerr << "[Engine] Erreur critique lors de la compilation des shaders integres." << std::endl;
    }

    // -- ENTITÉ 1 : Le Triangle RGB
    std::vector<Vertex> triangleVertices = {
        { NkVec3f(-0.6f, -0.5f, 0.0f), NkVec3f(1.0f, 0.0f, 0.0f) }, 
        { NkVec3f( 0.0f, -0.5f, 0.0f), NkVec3f(0.0f, 1.0f, 0.0f) }, 
        { NkVec3f(-0.3f,  0.5f, 0.0f), NkVec3f(0.0f, 0.0f, 1.0f) }  
    };
    Mesh triangleMesh;
    triangleMesh.Init(triangleVertices);

    // -- ENTITÉ 2 : Le Quad (Indexé avec EBO)
    std::vector<Vertex> quadVertices = {
        { NkVec3f(0.2f,  0.5f, 0.0f), NkVec3f(1.0f, 1.0f, 0.0f) }, // Jaune
        { NkVec3f(0.8f,  0.5f, 0.0f), NkVec3f(0.0f, 1.0f, 1.0f) }, // Cyan
        { NkVec3f(0.8f, -0.5f, 0.0f), NkVec3f(1.0f, 0.0f, 1.0f) }, // Magenta
        { NkVec3f(0.2f, -0.5f, 0.0f), NkVec3f(1.0f, 1.0f, 1.0f) }  // Blanc
    };
    std::vector<unsigned int> quadIndices = {
        0, 1, 2, 
        2, 3, 0  
    };
    Mesh quadMesh;
    quadMesh.Init(quadVertices, quadIndices);

    // -- ENTITÉ 3 : Un Pentagone coloré au centre-haut
    std::vector<Vertex> pentagonVertices = {
        { NkVec3f( 0.0f,  0.8f, 0.0f), NkVec3f(1.0f, 0.5f, 0.0f) }, // Orange (Haut)
        { NkVec3f(-0.3f,  0.5f, 0.0f), NkVec3f(0.0f, 1.0f, 1.0f) }, // Cyan
        { NkVec3f(-0.2f,  0.1f, 0.0f), NkVec3f(1.0f, 0.0f, 1.0f) }, // Magenta
        { NkVec3f( 0.2f,  0.1f, 0.0f), NkVec3f(1.0f, 1.0f, 0.0f) }, // Jaune
        { NkVec3f( 0.3f,  0.5f, 0.0f), NkVec3f(0.5f, 1.0f, 0.5f) }  // Vert clair
    };
    std::vector<unsigned int> pentagonIndices = {
        0, 1, 4,
        1, 2, 3,
        1, 3, 4
    };

    Mesh pentagonMesh;
    pentagonMesh.Init(pentagonVertices, pentagonIndices);

    bool running = true;
    auto& events = NkEvents();

    while (running) {
        while (NkEvent* ev = events.PollEvent()) {
            if (ev->Is<NkWindowCloseEvent>()) {
                running = false;
                break;
            }
            // Note : Le Hot-Reload est temporairement désactivé puisque le code est figé dans le C++
        }

        if (!running) {
            break;
        }

        if (ctx->BeginFrame()) {
            glViewport(0, 0, window.GetSize().width, window.GetSize().height);

            glClearColor(0.1f, 0.1f, 0.15f, 1.0f); 
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            if (activeShader.ID != 0) {
                activeShader.use();

                // Dessine le tout nouveau pentagone !
                pentagonMesh.Draw();
                
                // Dessiner le triangle à gauche (sans EBO)
                triangleMesh.Draw();

                // Dessiner le quad à droite (avec EBO)
                quadMesh.Draw();

                
            }

            ctx->EndFrame();
            ctx->Present();
        }
    }

    // Nettoyage
    triangleMesh.Destroy();
    quadMesh.Destroy();
    pentagonMesh.Destroy();
    if(activeShader.ID != 0) {
        glDeleteProgram(activeShader.ID);
    }
    
    ctx->Shutdown();
    window.Close();

    return 0;
}