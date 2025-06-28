#include <string.h>
#define HELPERS_IMPLEMENTATION
#include "helpers.h"

#define CLAY_IMPLEMENTATION
#include "clay/clay.h"
#include "clay/renderers/raylib/raylib.h"
#include "clay/renderers/raylib/clay_renderer_raylib.c"

#include "layout.c"

#define INPUT_MIN_CAP 10
#define INPUT_GROWTH_STEP 10

void HandleClayErrors(Clay_ErrorData data) {
	printf("%s", data.errorText.chars);

	switch (data.errorType) {
		// TODO: Handle possible error types
	}
}

int main(void) {
	/* CLAY INIT */
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


	/* OTHER INIT */
	SetTargetFPS(60);

	Layout_Data data = Layout_NewData();

	// FIXME: Delete test lines
	Header headers_test[2] = {
		{
			.key = malloc(INPUT_MIN_CAP),
			.val = malloc(INPUT_MIN_CAP)
		},
		{
			.key = malloc(INPUT_MIN_CAP),
			.val = malloc(INPUT_MIN_CAP)
		}
	};
	strcpy(headers_test[0].key, "Header1");
	strcpy(headers_test[0].val, "Value1");
	strcpy(headers_test[1].key, "Header2");
	strcpy(headers_test[1].val, "Value2");
	dinAppend(data.headers, headers_test, 2);

	/* MAIN LOOP */
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

		/* HANDLE KEY PRESSES */
		if (data.selected_input_box.id != 0) {
			int key = GetCharPressed();
			int header_index = data.selected_input_box.offset - 1;

			if (key != 0) {
				char** target = strstr(data.selected_input_box.stringId.chars, "Key")
					? &((Header*)data.headers->vals)[header_index].key
					: &((Header*)data.headers->vals)[header_index].val;

				if ((key >= 32) && (key <= 125)) {
					size_t target_len = strlen(*target);

					if (target_len % 10 == 9)
						*target = realloc(*target, target_len + INPUT_GROWTH_STEP);

					strncat(*target, (char[]){(char)key}, 1);
				}
			} else if (IsKeyDown(KEY_BACKSPACE)) {
				char** target = strstr(data.selected_input_box.stringId.chars, "Key")
					? &((Header*)data.headers->vals)[header_index].key
					: &((Header*)data.headers->vals)[header_index].val;

				size_t target_len = strlen(*target);
				if (target_len > 0) {
					(*target)[target_len - 1] = '\0';

					if (target_len % 10 == 9) {
						*target = realloc(*target, target_len + 1);
					}
				}

				// NOTE: Wait before deleting more so humans can keep up
				WaitTime(0.05);
			}
		}

		/* RENDER */
		BeginDrawing();
		ClearBackground(BLACK);
		Clay_Raylib_Render(render_commands, fonts);
		EndDrawing();
	}
	
	Clay_Raylib_Close();

	return 0;
}
