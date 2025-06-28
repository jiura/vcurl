#include "helpers.h"
#include "clay/clay.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int FONT_ID_BODY_16 = 0;

typedef struct {
	char* key;
	char* val;
} Header;

typedef struct {
	DinArray* headers;
} Layout_Data;

Layout_Data Layout_NewData() {
	Layout_Data data;

	data.headers = dinNew(2, sizeof(Header));
	if (!data.headers) {
		fprintf(stderr, "Fatal error: Failed to allocate memory for layout data. Shutting down.");
		exit(EXIT_FAILURE);
	}

	return data;
}

bool Layout_DataAddHeader(Layout_Data *data, Header *new_header) {
	return dinAppend(data->headers, new_header, 1);
}

void RenderClayHeaderButton(Clay_String text) {
	CLAY({.layout = {.padding = {16, 16, 8, 8}},
		  .backgroundColor = {140, 140, 140, 255},
		  .cornerRadius = CLAY_CORNER_RADIUS(5)}) {
		CLAY_TEXT(text, CLAY_TEXT_CONFIG({.fontId = FONT_ID_BODY_16,
										  .fontSize = 16,
										  .textColor = {255, 255, 255, 255}}));
	}
}

void RenderClayDropdownMenuItem(Clay_String text) {
	CLAY({.layout = {.padding = CLAY_PADDING_ALL(16)}}) {
		CLAY_TEXT(text, CLAY_TEXT_CONFIG({.fontId = FONT_ID_BODY_16,
										  .fontSize = 16,
										  .textColor = {255, 255, 255, 255}}));
	}
}

Clay_RenderCommandArray CreateLayout(Layout_Data *data) {
	Clay_BeginLayout();

	Clay_Sizing layout_expand = {.width = CLAY_SIZING_GROW(0),
								 .height = CLAY_SIZING_GROW(0)};

	Clay_Color outer_container_bg_color = {43, 41, 51, 255};
	Clay_Color content_background_color = {90, 90, 90, 255};

	// Build UI here
	CLAY({
		.id = CLAY_ID("OuterContainer"),
		.backgroundColor = outer_container_bg_color,
		.layout = {
			.layoutDirection = CLAY_TOP_TO_BOTTOM,
			.sizing = layout_expand,
			.padding = CLAY_PADDING_ALL(16),
			.childGap = 16
		}
	}) { // Child elements go inside braces
		CLAY({.id = CLAY_ID("HeaderBar"),
			  .layout = {.sizing = {.height = CLAY_SIZING_FIXED(60),
									.width = CLAY_SIZING_GROW(0)},
						 .padding = {16, 16, 0, 0},
						 .childGap = 16,
						 .childAlignment = {.y = CLAY_ALIGN_Y_CENTER}},
			  .backgroundColor = content_background_color,
			  .cornerRadius = CLAY_CORNER_RADIUS(8)}) {
			// Header buttons go here
			CLAY({.id = CLAY_ID("FileButton"),
				  .layout = {.padding = {16, 16, 8, 8}},
				  .backgroundColor = {140, 140, 140, 255},
				  .cornerRadius = CLAY_CORNER_RADIUS(5)}) {
				CLAY_TEXT(
					CLAY_STRING("File"),
					CLAY_TEXT_CONFIG({.fontId = FONT_ID_BODY_16,
									  .fontSize = 16,
									  .textColor = {255, 255, 255, 255}}));

				bool file_menu_visible =
					Clay_PointerOver(
						Clay_GetElementId(CLAY_STRING("FileButton"))) ||
					Clay_PointerOver(
						Clay_GetElementId(CLAY_STRING("FileMenu")));

				if (file_menu_visible) {
					CLAY({.id = CLAY_ID("FileMenu"),
						  .floating =
							  {
								  .attachTo = CLAY_ATTACH_TO_PARENT,
								  .attachPoints =
									  {.parent = CLAY_ATTACH_POINT_LEFT_BOTTOM},
							  },
						  .layout = {.padding = {0, 0, 8, 8}}}) {
						CLAY({.layout =
								  {
									  .layoutDirection = CLAY_TOP_TO_BOTTOM,
									  .sizing = {.width =
													 CLAY_SIZING_FIXED(200)},
								  },
							  .backgroundColor = {40, 40, 40, 255},
							  .cornerRadius = CLAY_CORNER_RADIUS(8)}) {
							// Render dropdown items here
							RenderClayDropdownMenuItem(CLAY_STRING("New"));
							RenderClayDropdownMenuItem(CLAY_STRING("Open"));
							RenderClayDropdownMenuItem(CLAY_STRING("Close"));
						}
					}
				}
			}

			RenderClayHeaderButton(CLAY_STRING("Edit"));
			CLAY({.layout = {.sizing = {CLAY_SIZING_GROW(0)}}}) {}
			RenderClayHeaderButton(CLAY_STRING("Upload"));
			RenderClayHeaderButton(CLAY_STRING("Media"));
			RenderClayHeaderButton(CLAY_STRING("Support"));
		}

		CLAY({
			.id = CLAY_ID("MainContainer"),
			.layout = {
				.layoutDirection = CLAY_LEFT_TO_RIGHT,
				.sizing = layout_expand,
				.childGap = 16
			}
		}) {

			CLAY({
				.id = CLAY_ID("RequestForm"),
				.backgroundColor = content_background_color,
				.cornerRadius = CLAY_CORNER_RADIUS(5),
				.layout = {
					.layoutDirection = CLAY_TOP_TO_BOTTOM,
					.padding = CLAY_PADDING_ALL(10),
					.sizing = layout_expand,
					.childGap = 16
				}
			}) {

				CLAY({
					.id = CLAY_ID("MethodAndUrl"),
					.layout = {
						.layoutDirection = CLAY_LEFT_TO_RIGHT,
						.childGap = 16,
						.sizing = {
							.width = CLAY_SIZING_GROW(0)
//							.height = CLAY_SIZING_FIT(0)
						}
					}
				}) {

					CLAY({
						.id = CLAY_ID("MethodDropdown"),
						.backgroundColor = outer_container_bg_color,
						.cornerRadius = CLAY_CORNER_RADIUS(5),
						.layout = {
							.sizing = {
								.width = CLAY_SIZING_FIXED(100),
								.height = CLAY_SIZING_FIXED(50)
							}
						}
					}) {}

					CLAY({
						.id = CLAY_ID("UrlInput"),
						.backgroundColor = outer_container_bg_color,
						.cornerRadius = CLAY_CORNER_RADIUS(5),
						.layout = {
							.sizing = {
								.width = CLAY_SIZING_GROW(0),
								.height = CLAY_SIZING_FIXED(50)
							}
						}
					}) {}
				}

				CLAY({
					.id = CLAY_ID("HeadersContainer"),
					.layout = {
						.layoutDirection = CLAY_TOP_TO_BOTTOM,
						.childGap = 16,
						.sizing = {
							.width = CLAY_SIZING_GROW(0)
						}
					}
				}) {
					for (size_t i = 0; i < data->headers->len; ++i) {

						CLAY({
							.layout = {
								.layoutDirection = CLAY_LEFT_TO_RIGHT,
								.childGap = 16,
								.sizing = layout_expand
							}
						}) {

							CLAY({
								.id = CLAY_IDI("HeaderKey", i+1),
								.backgroundColor = outer_container_bg_color,
								.cornerRadius = CLAY_CORNER_RADIUS(5),
								.layout = {
									.padding = {
										.left = 10
									},
									.sizing = {
										.width = CLAY_SIZING_GROW(0),
										.height = CLAY_SIZING_FIXED(50)
									}
								}
							}) {

								Clay_String header_key = (Clay_String){
									.isStaticallyAllocated = 0,
									.length = strlen(((Header*)data->headers->vals)[i].key),
									.chars = ((Header*)data->headers->vals)[i].key
								};

								CLAY_TEXT(
									header_key,
									CLAY_TEXT_CONFIG({
										.fontId = FONT_ID_BODY_16,
										.fontSize = 26,
										.textColor = {255, 255, 255, 255},
										.textAlignment = CLAY_TEXT_ALIGN_LEFT,
										.lineHeight = 50
									})
								);
							}

							CLAY({
								.id = CLAY_IDI("HeaderVal", i+1),
								.backgroundColor = outer_container_bg_color,
								.cornerRadius = CLAY_CORNER_RADIUS(5),
								.layout = {
		  							.padding = {
										.left = 10
									},
									.sizing = {
										.width = CLAY_SIZING_GROW(0),
										.height = CLAY_SIZING_FIXED(50)
									}
								}
							}) {

								Clay_String header_value = (Clay_String){
									.isStaticallyAllocated = 0,
									.length = strlen(((Header*)data->headers->vals)[i].val),
									.chars = ((Header*)data->headers->vals)[i].val
								};

								CLAY_TEXT(
									header_value,
									CLAY_TEXT_CONFIG({
										.fontId = FONT_ID_BODY_16,
										.fontSize = 26,
										.textColor = {255, 255, 255, 255},
										.textAlignment = CLAY_TEXT_ALIGN_LEFT,
										.lineHeight = 50
									})
								);
							}

							CLAY({
								.backgroundColor = outer_container_bg_color,
								.cornerRadius = CLAY_CORNER_RADIUS(5),
								.layout = {
									.sizing = {
										.width = CLAY_SIZING_FIXED(50),
										.height = CLAY_SIZING_FIXED(50)
									}
								}
							}) {

								CLAY_TEXT(
									CLAY_STRING(" X"),
									CLAY_TEXT_CONFIG({
										.fontId = FONT_ID_BODY_16,
										.fontSize = 30,
										.textColor = {255, 255, 255, 255},
										.textAlignment = CLAY_TEXT_ALIGN_CENTER,
										.letterSpacing = 10,
										.lineHeight = 50
									})
								);
							}
						}
					}

					CLAY({
						.layout = {
							.layoutDirection = CLAY_LEFT_TO_RIGHT,
							.childAlignment = {
								.x = CLAY_ALIGN_X_RIGHT
							},
							.sizing = layout_expand
						}
					}) {

						CLAY({
							.id = CLAY_ID("HeaderAddNewBtn"),
							.backgroundColor = outer_container_bg_color,
							.cornerRadius = CLAY_CORNER_RADIUS(5),
							.layout = {
								.sizing = {
									.width = CLAY_SIZING_FIXED(50),
									.height = CLAY_SIZING_FIXED(50)
								}
							}
						}) {}
					}

					CLAY({
						.id = CLAY_ID("BodyInput"),
						.backgroundColor = outer_container_bg_color,
						.cornerRadius = CLAY_CORNER_RADIUS(5),
						.layout = {
							.sizing = {
								.width = CLAY_SIZING_PERCENT(0.45f),
								.height = CLAY_SIZING_FIXED(300)
							}
						}
					}) {}
				}
			}

			CLAY({
				.id = CLAY_ID("ResponseContent"),
				.backgroundColor = content_background_color,
				.cornerRadius = CLAY_CORNER_RADIUS(5),
				.layout = {
					.layoutDirection = CLAY_TOP_TO_BOTTOM,
					.sizing = layout_expand
				}
			}) {}
		}

		/*CLAY({
			.id = CLAY_ID("LowerContent"),
			.layout = {
				.sizing = layout_expand,
				.childGap = 16
			}
		}) {
			CLAY({
				.id = CLAY_ID("Sidebar"),
				.backgroundColor = content_background_color,
				.layout = {
					.layoutDirection = CLAY_TOP_TO_BOTTOM,
					.padding = CLAY_PADDING_ALL(16),
					.childGap = 8,
					.sizing = {
						.width = CLAY_SIZING_FIXED(250),
						.height = CLAY_SIZING_GROW(0)
					}
				}
			}) {}

			CLAY({
				.id = CLAY_ID("MainContent"),
				.backgroundColor = content_background_color,
				.clip = {.vertical = true, .childOffset = Clay_GetScrollOffset()},
				.layout = {
					.layoutDirection = CLAY_TOP_TO_BOTTOM,
					.childGap = 16,
					.padding = CLAY_PADDING_ALL(16),
					.sizing = layout_expand
				}
			}) {}
		}*/
	}

	Clay_RenderCommandArray render_commands = Clay_EndLayout();
	//			for (int32_t i = 0, i < render_commands.length; ++i) {
	//				Clay_RenderCommandArray_Get(&render_commands,
	//i)->boundingBox.y += data->yOffset;
	//			}

	return render_commands;
}
