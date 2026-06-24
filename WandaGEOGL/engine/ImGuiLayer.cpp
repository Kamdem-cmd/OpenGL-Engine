#include "ImGuiLayer.h"

// Implémentation minimaliste autonome des bindings de fenêtrage pour s'isoler des dépendances GLFW/SDL
#include <windows.h>

using namespace nkentseu;

bool ImGuiLayer::s_WantCaptureMouse = false;
bool ImGuiLayer::s_WantCaptureKeyboard = false;

void ImGuiLayer::Init(nkentseu::NkWindow& window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    // Binding OpenGL natif
    ImGui_ImplOpenGL3_Init("#version 460 core");

    io.DisplaySize = ImVec2((float)window.GetSize().width, (float)window.GetSize().height);
}

void ImGuiLayer::BeginFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();

    s_WantCaptureMouse = ImGui::GetIO().WantCaptureMouse;
    s_WantCaptureKeyboard = ImGui::GetIO().WantCaptureKeyboard;
}

void ImGuiLayer::EndFrame() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiLayer::Shutdown() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();
}

// =============================================================================
// Mapping NkMouseButton -> index bouton ImGui
// ImGui attend : 0 = gauche, 1 = droit, 2 = milieu, 3/4 = extra
// =============================================================================
static int ToImGuiMouseButton(NkMouseButton b) {
    switch (b) {
        case NkMouseButton::NK_MB_LEFT:    return 0;
        case NkMouseButton::NK_MB_RIGHT:   return 1;
        case NkMouseButton::NK_MB_MIDDLE:  return 2;
        case NkMouseButton::NK_MB_BACK:    return 3;
        case NkMouseButton::NK_MB_FORWARD: return 4;
        default:                           return -1;
    }
}

// =============================================================================
// Mapping NkKey -> ImGuiKey
// =============================================================================
ImGuiKey ImGuiLayer::ToImGuiKey(NkKey key) {
    switch (key) {
        case NkKey::NK_TAB:           return ImGuiKey_Tab;
        case NkKey::NK_LEFT:          return ImGuiKey_LeftArrow;
        case NkKey::NK_RIGHT:         return ImGuiKey_RightArrow;
        case NkKey::NK_UP:            return ImGuiKey_UpArrow;
        case NkKey::NK_DOWN:          return ImGuiKey_DownArrow;
        case NkKey::NK_PAGE_UP:       return ImGuiKey_PageUp;
        case NkKey::NK_PAGE_DOWN:     return ImGuiKey_PageDown;
        case NkKey::NK_HOME:          return ImGuiKey_Home;
        case NkKey::NK_END:           return ImGuiKey_End;
        case NkKey::NK_INSERT:        return ImGuiKey_Insert;
        case NkKey::NK_DELETE:        return ImGuiKey_Delete;
        case NkKey::NK_BACK:          return ImGuiKey_Backspace;
        case NkKey::NK_SPACE:         return ImGuiKey_Space;
        case NkKey::NK_ENTER:         return ImGuiKey_Enter;
        case NkKey::NK_ESCAPE:        return ImGuiKey_Escape;
        case NkKey::NK_APOSTROPHE:    return ImGuiKey_Apostrophe;
        case NkKey::NK_COMMA:         return ImGuiKey_Comma;
        case NkKey::NK_MINUS:         return ImGuiKey_Minus;
        case NkKey::NK_PERIOD:        return ImGuiKey_Period;
        case NkKey::NK_SLASH:         return ImGuiKey_Slash;
        case NkKey::NK_SEMICOLON:     return ImGuiKey_Semicolon;
        case NkKey::NK_EQUALS:        return ImGuiKey_Equal;
        case NkKey::NK_LBRACKET:      return ImGuiKey_LeftBracket;
        case NkKey::NK_BACKSLASH:     return ImGuiKey_Backslash;
        case NkKey::NK_RBRACKET:      return ImGuiKey_RightBracket;
        case NkKey::NK_GRAVE:         return ImGuiKey_GraveAccent;
        case NkKey::NK_CAPSLOCK:      return ImGuiKey_CapsLock;
        case NkKey::NK_SCROLL_LOCK:   return ImGuiKey_ScrollLock;
        case NkKey::NK_NUM_LOCK:      return ImGuiKey_NumLock;
        case NkKey::NK_PRINT_SCREEN:  return ImGuiKey_PrintScreen;
        case NkKey::NK_PAUSE_BREAK:   return ImGuiKey_Pause;
        case NkKey::NK_LCTRL:         return ImGuiKey_LeftCtrl;
        case NkKey::NK_LSHIFT:        return ImGuiKey_LeftShift;
        case NkKey::NK_LALT:          return ImGuiKey_LeftAlt;
        case NkKey::NK_LSUPER:        return ImGuiKey_LeftSuper;
        case NkKey::NK_RCTRL:         return ImGuiKey_RightCtrl;
        case NkKey::NK_RSHIFT:        return ImGuiKey_RightShift;
        case NkKey::NK_RALT:          return ImGuiKey_RightAlt;
        case NkKey::NK_RSUPER:        return ImGuiKey_RightSuper;
        case NkKey::NK_MENU:          return ImGuiKey_Menu;

        case NkKey::NK_NUM0: return ImGuiKey_0;
        case NkKey::NK_NUM1: return ImGuiKey_1;
        case NkKey::NK_NUM2: return ImGuiKey_2;
        case NkKey::NK_NUM3: return ImGuiKey_3;
        case NkKey::NK_NUM4: return ImGuiKey_4;
        case NkKey::NK_NUM5: return ImGuiKey_5;
        case NkKey::NK_NUM6: return ImGuiKey_6;
        case NkKey::NK_NUM7: return ImGuiKey_7;
        case NkKey::NK_NUM8: return ImGuiKey_8;
        case NkKey::NK_NUM9: return ImGuiKey_9;

        case NkKey::NK_A: return ImGuiKey_A;
        case NkKey::NK_B: return ImGuiKey_B;
        case NkKey::NK_C: return ImGuiKey_C;
        case NkKey::NK_D: return ImGuiKey_D;
        case NkKey::NK_E: return ImGuiKey_E;
        case NkKey::NK_F: return ImGuiKey_F;
        case NkKey::NK_G: return ImGuiKey_G;
        case NkKey::NK_H: return ImGuiKey_H;
        case NkKey::NK_I: return ImGuiKey_I;
        case NkKey::NK_J: return ImGuiKey_J;
        case NkKey::NK_K: return ImGuiKey_K;
        case NkKey::NK_L: return ImGuiKey_L;
        case NkKey::NK_M: return ImGuiKey_M;
        case NkKey::NK_N: return ImGuiKey_N;
        case NkKey::NK_O: return ImGuiKey_O;
        case NkKey::NK_P: return ImGuiKey_P;
        case NkKey::NK_Q: return ImGuiKey_Q;
        case NkKey::NK_R: return ImGuiKey_R;
        case NkKey::NK_S: return ImGuiKey_S;
        case NkKey::NK_T: return ImGuiKey_T;
        case NkKey::NK_U: return ImGuiKey_U;
        case NkKey::NK_V: return ImGuiKey_V;
        case NkKey::NK_W: return ImGuiKey_W;
        case NkKey::NK_X: return ImGuiKey_X;
        case NkKey::NK_Y: return ImGuiKey_Y;
        case NkKey::NK_Z: return ImGuiKey_Z;

        case NkKey::NK_F1:  return ImGuiKey_F1;
        case NkKey::NK_F2:  return ImGuiKey_F2;
        case NkKey::NK_F3:  return ImGuiKey_F3;
        case NkKey::NK_F4:  return ImGuiKey_F4;
        case NkKey::NK_F5:  return ImGuiKey_F5;
        case NkKey::NK_F6:  return ImGuiKey_F6;
        case NkKey::NK_F7:  return ImGuiKey_F7;
        case NkKey::NK_F8:  return ImGuiKey_F8;
        case NkKey::NK_F9:  return ImGuiKey_F9;
        case NkKey::NK_F10: return ImGuiKey_F10;
        case NkKey::NK_F11: return ImGuiKey_F11;
        case NkKey::NK_F12: return ImGuiKey_F12;

        case NkKey::NK_NUMPAD_0: return ImGuiKey_Keypad0;
        case NkKey::NK_NUMPAD_1: return ImGuiKey_Keypad1;
        case NkKey::NK_NUMPAD_2: return ImGuiKey_Keypad2;
        case NkKey::NK_NUMPAD_3: return ImGuiKey_Keypad3;
        case NkKey::NK_NUMPAD_4: return ImGuiKey_Keypad4;
        case NkKey::NK_NUMPAD_5: return ImGuiKey_Keypad5;
        case NkKey::NK_NUMPAD_6: return ImGuiKey_Keypad6;
        case NkKey::NK_NUMPAD_7: return ImGuiKey_Keypad7;
        case NkKey::NK_NUMPAD_8: return ImGuiKey_Keypad8;
        case NkKey::NK_NUMPAD_9: return ImGuiKey_Keypad9;
        case NkKey::NK_NUMPAD_DOT:    return ImGuiKey_KeypadDecimal;
        case NkKey::NK_NUMPAD_DIV:    return ImGuiKey_KeypadDivide;
        case NkKey::NK_NUMPAD_MUL:    return ImGuiKey_KeypadMultiply;
        case NkKey::NK_NUMPAD_SUB:    return ImGuiKey_KeypadSubtract;
        case NkKey::NK_NUMPAD_ADD:    return ImGuiKey_KeypadAdd;
        case NkKey::NK_NUMPAD_ENTER:  return ImGuiKey_KeypadEnter;
        case NkKey::NK_NUMPAD_EQUALS: return ImGuiKey_KeypadEqual;

        default: return ImGuiKey_None;
    }
}

// Applique l'état courant des modificateurs (Ctrl/Shift/Alt/Super) à ImGui.
// À appeler à chaque événement clavier pour rester synchronisé, y compris
// quand le système n'envoie pas explicitement d'événement pour LCtrl/RCtrl etc.
static void UpdateModifiers(ImGuiIO& io, const NkModifierState& mods) {
    io.AddKeyEvent(ImGuiMod_Ctrl,  mods.ctrl);
    io.AddKeyEvent(ImGuiMod_Shift, mods.shift);
    io.AddKeyEvent(ImGuiMod_Alt,   mods.alt || mods.altGr);
    io.AddKeyEvent(ImGuiMod_Super, mods.super);
}

bool ImGuiLayer::HandleNKEvent(nkentseu::NkEvent* ev) {
    ImGuiIO& io = ImGui::GetIO();

    // ---- Souris : déplacement ----
    if (ev->Is<NkMouseMoveEvent>()) {
        auto m = ev->As<NkMouseMoveEvent>();
        io.AddMousePosEvent((float)m->GetX(), (float)m->GetY());

        // --- DEBUG TEMPORAIRE ---
        static int counter = 0;
        if (++counter % 30 == 0) { // pour ne pas spammer la console
            printf("[DEBUG] NkMouseMove: x=%d y=%d | io.DisplaySize=%.0fx%.0f\n",
                m->GetX(), m->GetY(), io.DisplaySize.x, io.DisplaySize.y);
    }

    return s_WantCaptureMouse;
}

    // ---- Souris : entrée/sortie de la zone client ----
    if (ev->Is<NkMouseLeaveEvent>()) {
        io.AddMousePosEvent(-FLT_MAX, -FLT_MAX);
        return false; // pas de capture à faire ici, juste informer ImGui
    }

    // ---- Souris : boutons ----
    if (ev->Is<NkMouseButtonPressEvent>()) {
        auto b = ev->As<NkMouseButtonPressEvent>();
        int button = ToImGuiMouseButton(b->GetButton());
        if (button >= 0) io.AddMouseButtonEvent(button, true);
        return s_WantCaptureMouse;
    }

    if (ev->Is<NkMouseButtonReleaseEvent>()) {
        auto b = ev->As<NkMouseButtonReleaseEvent>();
        int button = ToImGuiMouseButton(b->GetButton());
        if (button >= 0) io.AddMouseButtonEvent(button, false);
        return s_WantCaptureMouse;
    }

    if (ev->Is<NkMouseDoubleClickEvent>()) {
        auto b = ev->As<NkMouseDoubleClickEvent>();
        int button = ToImGuiMouseButton(b->GetButton());
        // ImGui détecte lui-même le double-clic à partir de deux press/release
        // rapprochés ; on relaie simplement un press supplémentaire.
        if (button >= 0) io.AddMouseButtonEvent(button, true);
        return s_WantCaptureMouse;
    }

    // ---- Souris : molette verticale / horizontale ----
    if (ev->Is<NkMouseWheelVerticalEvent>()) {
        auto w = ev->As<NkMouseWheelVerticalEvent>();
        io.AddMouseWheelEvent(0.0f, (float)w->GetDeltaY());
        return s_WantCaptureMouse;
    }

    if (ev->Is<NkMouseWheelHorizontalEvent>()) {
        auto w = ev->As<NkMouseWheelHorizontalEvent>();
        io.AddMouseWheelEvent((float)w->GetDeltaX(), 0.0f);
        return s_WantCaptureMouse;
    }

    // ---- Souris : scroll unifié (trackpad) ----
    if (ev->Is<NkMouseScrollEvent>()) {
        auto s = ev->As<NkMouseScrollEvent>();
        io.AddMouseWheelEvent((float)s->GetDeltaX(), (float)s->GetDeltaY());
        return s_WantCaptureMouse;
    }

    // ---- Clavier : touche pressée / répétée ----
    if (ev->Is<NkKeyPressEvent>()) {
        auto k = ev->As<NkKeyPressEvent>();
        UpdateModifiers(io, k->GetModifiers());
        ImGuiKey key = ToImGuiKey(k->GetKey());
        if (key != ImGuiKey_None) io.AddKeyEvent(key, true);
        return s_WantCaptureKeyboard;
    }

    if (ev->Is<NkKeyRepeatEvent>()) {
        auto k = ev->As<NkKeyRepeatEvent>();
        UpdateModifiers(io, k->GetModifiers());
        ImGuiKey key = ToImGuiKey(k->GetKey());
        if (key != ImGuiKey_None) io.AddKeyEvent(key, true);
        return s_WantCaptureKeyboard;
    }

    if (ev->Is<NkKeyReleaseEvent>()) {
        auto k = ev->As<NkKeyReleaseEvent>();
        UpdateModifiers(io, k->GetModifiers());
        ImGuiKey key = ToImGuiKey(k->GetKey());
        if (key != ImGuiKey_None) io.AddKeyEvent(key, false);
        return s_WantCaptureKeyboard;
    }

    // ---- Texte : caractère Unicode produit (saisie dans les champs ImGui) ----
    if (ev->Is<NkTextInputEvent>()) {
        auto t = ev->As<NkTextInputEvent>();
        if (t->IsPrintable()) io.AddInputCharacter(t->GetCodepoint());
        return s_WantCaptureKeyboard;
    }

    // ---- Fenêtre : redimensionnement -> ImGui doit connaître la nouvelle taille ----
    if (ev->Is<NkWindowResizeEvent>()) {
        auto r = ev->As<NkWindowResizeEvent>();
        io.DisplaySize = ImVec2((float)r->GetWidth(), (float)r->GetHeight());
        return false; // un resize ne doit jamais être "consommé", la scène doit aussi réagir
    }

    // ---- Fenêtre : perte de focus -> on relâche tout pour éviter les touches "collées" ----
    if (ev->Is<NkWindowFocusLostEvent>()) {
        io.AddFocusEvent(false);
        return false;
    }

    if (ev->Is<NkWindowFocusGainedEvent>()) {
        io.AddFocusEvent(true);
        return false;
    }

    return false;
}
