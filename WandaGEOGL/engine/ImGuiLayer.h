#pragma once

#include "NKWindow/NkWindow.h"
#include "NKEvent/NkKeyboardEvent.h"
#include "NKEvent/NkMouseEvent.h"
#include "NKEvent/NkWindowEvent.h"
#include "imgui.h"
#include "imgui_impl_opengl3.h"

class ImGuiLayer {
private:
    static bool s_WantCaptureMouse;
    static bool s_WantCaptureKeyboard;

    // Traduit une touche logique Nkentseu en touche logique ImGui.
    // Renvoie ImGuiKey_None si la touche n'a pas d'équivalent utile pour ImGui.
    static ImGuiKey ToImGuiKey(nkentseu::NkKey key);

public:
    static void Init(nkentseu::NkWindow& window);
    static void BeginFrame();
    static void EndFrame();
    static void Shutdown();

    // Intercepteur d'événements exclusif Nkentseu.
    // Retourne true si ImGui a "consommé" l'événement (la scène/caméra ne doit pas le traiter).
    static bool HandleNKEvent(nkentseu::NkEvent* ev);

    static bool WantCaptureMouse() { return s_WantCaptureMouse; }
    static bool WantCaptureKeyboard() { return s_WantCaptureKeyboard; }
};
