#include <iostream>
#include<raylib.h>
#include<imgui.h> 
#include<rlImGui.h>

#include<gameMain.h>

int main()
{
	if (PRODUCTION_BUILD == 1)
	{
		SetTraceLogLevel(LOG_NONE); // 设置 raylib 最低需要显示的日志级别。
	}

	SetConfigFlags(FLAG_WINDOW_RESIZABLE);  //允许用户拖动窗口边缘改变窗口大小
	InitWindow(1600, 800, "C++ Game");
	SetExitKey(KEY_NULL);
	SetTargetFPS(240);

#pragma region ImGUI
	rlImGuiSetup(true);
	ImGuiIO& io = ImGui::GetIO();  //ImGuiIO是一个结构体
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; //停靠功能
	io.FontGlobalScale = 1;
#pragma endregion

	if (!initGame())
	{
		return 0;
	}

	while (!WindowShouldClose())
	{
		BeginDrawing();
		ClearBackground(BLACK);

#pragma region ImGUI
		rlImGuiBegin();
		ImGui::PushStyleColor(ImGuiCol_WindowBg, {});
        ImGui::PushStyleColor(ImGuiCol_DockingEmptyBg, {});
		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport()); // imgui可以停靠在游戏窗口
		ImGui::PopStyleColor(2);
#pragma endregion

		if (!updateGame())
		{
			CloseWindow();
		}

#pragma region ImGUI
		rlImGuiEnd();
#pragma endregion

		EndDrawing();
	}

	CloseWindow();
	closeGame();

#pragma region ImGUI
	rlImGuiShutdown();
#pragma endregion
	return 0;
}