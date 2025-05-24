#include <cstdlib>
#include <imgui.h>
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"
#include <SDL3/SDL.h>
#include <cstdio>
#include <fstream>
#include <deque>
#include <glob.h>

#define MAX_LINE_SIZE 15
#define MAX_VECTOR_SIZE 300
#define SERIAL_PATTERN "/dev/ttyACM*"

int main(int argc, char *argv[]) {
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) {
        printf("Error: SDL_Init(): %s\n", SDL_GetError());
        return -1;
    }

    glob_t globs;
    globs.gl_pathc = 0;
  
    // Get the serial port
    while (globs.gl_pathc == 0) {
        glob(SERIAL_PATTERN, 0, NULL, &globs);
    }
    const char *SERIAL = globs.gl_pathv[0];
    printf("Found %s\n", SERIAL);
    // Create window with SDL_Renderer graphics context
    Uint32 window_flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN;
    SDL_Window *window = SDL_CreateWindow("ImGuiOsc", 1280, 720, window_flags);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, nullptr);
    if (renderer == nullptr) {
        SDL_Log("Error: SDL_CreateRenderer(): %s\n", SDL_GetError());
        return -1;
    }

    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED,
                          SDL_WINDOWPOS_CENTERED);
    SDL_ShowWindow(window);

    std::ifstream serial = std::ifstream(SERIAL);
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |=
        ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |=
        ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls
    // io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    bool done = false;
    ImGuiViewport *view = ImGui::GetMainViewport();
    ImGui::SetNextWindowSize(view->WorkSize);
    ImGui::SetNextWindowPos(view->WorkPos);
    ImGui::SetNextWindowViewport(view->ID);

    std::deque<float> readings(MAX_VECTOR_SIZE);
    char dataline[MAX_LINE_SIZE];
    int data;
    float readData[MAX_VECTOR_SIZE];
    while (!done) {
        // SDL Event stuff
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL3_ProcessEvent(&event);
            if (event.type == SDL_EVENT_QUIT)
                done = true;
            if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED &&
                event.window.windowID == SDL_GetWindowID(window))
                done = true;
        }

        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        // Read data from the serial port
        if (serial.is_open()) {
            serial.getline(dataline, MAX_LINE_SIZE);
            sscanf(dataline, "%d\n", &data);
            if (readings.size() == MAX_VECTOR_SIZE) {
                readings.pop_front();
            }
            readings.push_back((float)data);
        } else
            serial.open(SERIAL);
        for (size_t i = 0; i < readings.size(); i++) {
            readData[i] = readings[i];
        }
        ImGuiWindowFlags window_flags =
            ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize;
        ImGui::SetNextWindowSize(view->WorkSize);
        ImGui::SetNextWindowPos(view->WorkPos);
        ImGui::SetNextWindowViewport(view->ID);
        ImGui::Begin("Oscope", NULL, window_flags);
        ImVec2 region = ImGui::GetContentRegionAvail();
        ImGui::PushItemWidth(region.x);
        ImGui::PlotLines("##", readData, readings.size(), 0, NULL, 0, 4500,
                         ImVec2(0, region.y / 2));
        ImGui::PopItemWidth();
        // ImGui::PlotLines();
        ImGui::End();
        ImGui::Render();
        SDL_SetRenderDrawColorFloat(renderer, clear_color.x, clear_color.y,
                                    clear_color.z, clear_color.w);
        SDL_RenderClear(renderer);
        ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
        SDL_RenderPresent(renderer);
    }
    serial.close();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
