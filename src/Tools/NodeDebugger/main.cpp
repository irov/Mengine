#include "NodeDebuggerApp.h"

#if 0
#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "imgui_impl_glfw_gl3_glad.h"
#endif

int main()
{
#if 1
    NodeDebuggerApp app;
    if( app.Initialize() )
    {
        app.Loop();
        app.Shutdown();
    }
#else

    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    int wndWidth = 1280, wndHeight = 720;
    GLFWwindow* wnd = glfwCreateWindow(wndWidth, wndHeight, "Node Debugger", nullptr, nullptr);

    //glfwSetWindowUserPointer(wnd, this);

    glfwMakeContextCurrent(wnd);
    gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));
    glfwSwapInterval(1); // enable v-sync

    glViewport(0, 0, wndWidth, wndHeight);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glfwSetScrollCallback(wnd, ImGui_ImplGlfw_ScrollCallback);
    glfwSetCharCallback(wnd, ImGui_ImplGlfw_CharCallback);
    glfwSetKeyCallback(wnd, ImGui_ImplGlfw_KeyCallback);
    glfwSetMouseButtonCallback(wnd, ImGui_ImplGlfw_MouseButtonCallback);

    // Setup Dear ImGui binding
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::GetIO().IniFilename = nullptr; // disable "imgui.ini"
    //ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange; // tell ImGui to not interfere with our cursors
    ImGui_ImplGlfwGL3_Init(wnd, false);
    ImGui::StyleColorsClassic();


    while (GL_FALSE == glfwWindowShouldClose(wnd)) {
        glfwPollEvents();

        glClearColor(0.412f, 0.796f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplGlfwGL3_NewFrame();

        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(static_cast<float>(wndWidth), static_cast<float>(wndHeight)), ImGuiCond_FirstUseEver);

        if (ImGui::Begin("Node Debugger", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse)) {
            ImGui::Columns(2, nullptr, true);
            ImGui::SetColumnWidth(0, 300.0f);

            if (ImGui::BeginChild("SceneTree", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar)) {
                const int numSubNodes = 20;

                for (int i = 0; i < numSubNodes; ++i) {
                    ImGui::TreeNodeEx("Some long title to force horizontal scroll", ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_Selected | ImGuiTreeNodeFlags_DefaultOpen);
                }

                for (int i = 0; i < numSubNodes; ++i) {
                    ImGui::TreePop();
                }
            }
            ImGui::EndChild();
        }
        ImGui::End();

        ImGui::Render();
        ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(wnd);
    }

    glfwTerminate();
#endif

    return 0;
}
