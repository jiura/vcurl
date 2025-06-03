#define HELPERS_IMPLEMENTATION
#include "helpers.h"

#define CLAY_IMPLEMENTATION
#include "clay/clay.h"
#include "clay/renderers/raylib/raylib.h"
#include "clay/renderers/raylib/clay_renderer_raylib.c"

#include "layout.c"

void HandleClayErrors(Clay_ErrorData data) {
	printf("%s", data.errorText.chars);

	switch (data.errorType) {
		// TODO: Handle possible error types
	}
}

int main(void) {
	Clay_Raylib_Initialize(800, 600, "vcurl", FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
	
	uint64_t clay_required_memory = Clay_MinMemorySize();
	Clay_Arena clay_memory = Clay_CreateArenaWithCapacityAndMemory(clay_required_memory, malloc(clay_required_memory));

	Clay_Initialize(clay_memory,
	(Clay_Dimensions) {
		.width = GetScreenWidth(),
		.height = GetScreenHeight()
	},
	(Clay_ErrorHandler) { HandleClayErrors });

	Font fonts[1];
	fonts[FONT_ID_BODY_16] = LoadFontEx("resources/Roboto-Regular.ttf", 48, 0, 400);
	SetTextureFilter(fonts[FONT_ID_BODY_16].texture, TEXTURE_FILTER_BILINEAR);
	Clay_SetMeasureTextFunction(Raylib_MeasureText, fonts);

	Layout_Data data = Layout_NewData();

	while (!WindowShouldClose()) {
		Clay_SetLayoutDimensions((Clay_Dimensions) {
			.width = GetScreenWidth(),
			.height = GetScreenHeight()
		});

		Vector2 mouse_position = GetMousePosition();
		Vector2 scroll_delta = GetMouseWheelMoveV();

		Clay_SetPointerState(
			(Clay_Vector2) { mouse_position.x, mouse_position.y },
			IsMouseButtonDown(0)
		);

		Clay_UpdateScrollContainers(
			true,
			(Clay_Vector2) { scroll_delta.x, scroll_delta.y },
			GetFrameTime()
		);

		Clay_RenderCommandArray render_commands = CreateLayout(&data);

		BeginDrawing();
		ClearBackground(BLACK);
		Clay_Raylib_Render(render_commands, fonts);
		EndDrawing();
	}
	
	Clay_Raylib_Close();
}
