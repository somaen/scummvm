/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include "glk/comprehend/comprehend.h"
#include "glk/comprehend/file_buf.h"
#include "glk/comprehend/game.h"
#include "glk/comprehend/game_data.h"
#include "glk/comprehend/image_data.h"
#include "glk/comprehend/graphics.h"
#include "glk/comprehend/util.h"

namespace Glk {
namespace Comprehend {

#define IMAGES_PER_FILE	16

struct ImageContext {
	unsigned	x;
	unsigned	y;
	unsigned	pen_color;
	unsigned	fill_color;
	unsigned	shape;

	unsigned	text_x;
	unsigned	text_y;
};

static unsigned draw_flags;

void ImageData::clear() {
	fb = nullptr;
	image_offsets = nullptr;
	nr_images = 0;
}

void image_set_draw_flags(unsigned flags)
{
	draw_flags |= flags;
}

static uint16 image_get_operand(FileBuffer *fb)
{
	uint8 val;

	val = fb->readByte();
	return val;
}

static bool do_image_op(FileBuffer *fb, ImageContext *ctx)
{
	uint8 opcode;
	uint16 a, b;

	opcode = fb->readByte();
	debug_printf(DEBUG_IMAGE_DRAW,
		     "  %.4x [%.2x]: ", fb->pos() - 1, opcode);

	switch (opcode) {
	case IMAGE_OP_SCENE_END:
	case IMAGE_OP_EOF:
		debug_printf(DEBUG_IMAGE_DRAW, "end\n");
		return true;

	case IMAGE_OP_PEN_COLOR_A:
	case IMAGE_OP_PEN_COLOR_B:
	case IMAGE_OP_PEN_COLOR_C:
	case IMAGE_OP_PEN_COLOR_D:
	case IMAGE_OP_PEN_COLOR_E:
	case IMAGE_OP_PEN_COLOR_F:
	case IMAGE_OP_PEN_COLOR_G:
	case IMAGE_OP_PEN_COLOR_H:
		debug_printf(DEBUG_IMAGE_DRAW, "set_pen_color(%.2x)\n", opcode);
		ctx->pen_color = g_set_pen_color(opcode);
		break;

	case IMAGE_OP_DRAW_LINE:
	case IMAGE_OP_DRAW_LINE_FAR:
		a = image_get_operand(fb);
		b = image_get_operand(fb);

		if (opcode & 0x1)
			a += 255;

		debug_printf(DEBUG_IMAGE_DRAW,
			     "draw_line (%d, %d) - (%d, %d)\n", opcode,
			     ctx->x, ctx->y, a, b);
		g_draw_line(ctx->x, ctx->y, a, b, ctx->pen_color);

		ctx->x = a;
		ctx->y = b;
		break;

	case IMAGE_OP_DRAW_BOX:
	case IMAGE_OP_DRAW_BOX_FAR:
		a = image_get_operand(fb);
		b = image_get_operand(fb);

		if (opcode & 0x1)
			a += 255;

		debug_printf(DEBUG_IMAGE_DRAW,
			     "draw_box (%d, %d) - (%d, %d)\n", opcode,
			     ctx->x, ctx->y, a, b);

		g_draw_box(ctx->x, ctx->y, a, b, ctx->pen_color);
		break;

	case IMAGE_OP_MOVE_TO:
	case IMAGE_OP_MOVE_TO_FAR:
		/* Move to */
		a = image_get_operand(fb);
		b = image_get_operand(fb);

		if (opcode & 0x1)
			a += 255;

		debug_printf(DEBUG_IMAGE_DRAW, "move_to(%d, %d)\n", a, b);
		ctx->x = a;
		ctx->y = b;
		break;

	case IMAGE_OP_SHAPE_PIXEL:
	case IMAGE_OP_SHAPE_BOX:
	case IMAGE_OP_SHAPE_CIRCLE_TINY:
	case IMAGE_OP_SHAPE_CIRCLE_SMALL:
	case IMAGE_OP_SHAPE_CIRCLE_MED:
	case IMAGE_OP_SHAPE_CIRCLE_LARGE:
	case IMAGE_OP_SHAPE_A:
	case IMAGE_OP_SHAPE_SPRAY:
		debug_printf(DEBUG_IMAGE_DRAW,
			     "set_shape_type(%.2x)\n", opcode - 0x40);
		ctx->shape = opcode;
		break;

	case 0x48:
		/*
		 * FIXME - This appears to be a shape type. Only used by
		 *         OO-Topos.
		 */
		debug_printf(DEBUG_IMAGE_DRAW, "shape_unknown()\n");
		ctx->shape = IMAGE_OP_SHAPE_PIXEL;
		break;

	case IMAGE_OP_DRAW_SHAPE:
	case IMAGE_OP_DRAW_SHAPE_FAR:
		a = image_get_operand(fb);
		b = image_get_operand(fb);

		if (opcode & 0x1)
			a += 255;

		debug_printf(DEBUG_IMAGE_DRAW,
			     "draw_shape(%d, %d), style=%.2x, fill=%.2x\n",
			     a, b, ctx->shape, ctx->fill_color);

		g_draw_shape(a, b, ctx->shape, ctx->fill_color);
		break;

	case IMAGE_OP_PAINT:
	case IMAGE_OP_PAINT_FAR:
		/* Paint */
		a = image_get_operand(fb);
		b = image_get_operand(fb);

		if (opcode & 0x1)
			a += 255;

		debug_printf(DEBUG_IMAGE_DRAW, "paint(%d, %d)\n", a, b);
		if (!(draw_flags & IMAGEF_NO_FLOODFILL))
			g_floodfill(a, b, ctx->fill_color,
				    g_get_pixel_color(a, b));
		break;

	case IMAGE_OP_FILL_COLOR:
		a = image_get_operand(fb);
		debug_printf(DEBUG_IMAGE_DRAW, "set_fill_color(%.2x)\n", a);
		ctx->fill_color = g_set_fill_color(a);
		break;

	case IMAGE_OP_SET_TEXT_POS:
		a = image_get_operand(fb);
		b = image_get_operand(fb);
		debug_printf(DEBUG_IMAGE_DRAW, "set_text_pos(%d, %d)\n", a, b);

		ctx->text_x = a;
		ctx->text_y = b;
		break;

	case IMAGE_OP_DRAW_CHAR:
		a = image_get_operand(fb);
		debug_printf(DEBUG_IMAGE_DRAW, "draw_char(%c)\n",
			     a >= 0x20 && a < 0x7f ? a : '?');

		g_draw_box(ctx->text_x, ctx->text_y,
			   ctx->text_x + 6, ctx->text_y + 7, ctx->fill_color);
		ctx->text_x += 8;
		break;

	case 0xf3:
		/*
		 * FIXME - Oo-Topos uses this at the beginning of some room
		 *         images.
		 */
		debug_printf(DEBUG_IMAGE_DRAW, "unknown()\n");
		break;

	case 0xb5:
	case 0x82:
	case 0x50:
		/* FIXME - unknown, no arguments */
		debug_printf(DEBUG_IMAGE_DRAW, "unknown\n");
		break;

	case 0x73:
	case 0xb0:
	case 0xd0:
		/* FIXME - unknown, one argument */
		a = image_get_operand(fb);
		debug_printf(DEBUG_IMAGE_DRAW, "unknown %.2x: (%.2x) '%c'\n",
			     opcode, a,
			     a >= 0x20 && a < 0x7f ? a : '?');
		break;

	default:
		/* FIXME - Unknown, two arguments */
		a = image_get_operand(fb);
		b = image_get_operand(fb);

		debug_printf(DEBUG_IMAGE_DRAW,
			     "unknown(%.2x, %.2x)\n", a, b);
		g_draw_pixel(a, b, 0x00ff00ff);
		break;
	}

	return false;
}

void draw_image(ImageData *info, unsigned index)
{
	unsigned file_num;
	FileBuffer *fb;
	bool done = false;
	ImageContext ctx = {
		0, 0, G_COLOR_BLACK, G_COLOR_BLACK, IMAGE_OP_SHAPE_CIRCLE_LARGE, 0, 0
	};

	file_num = index / IMAGES_PER_FILE;
	fb = &info->fb[file_num];

	if (index >= info->nr_images) {
		warning("Bad image index %.8x (max=%.8x)\n", index,
		       (uint)info->nr_images);
		return;
	}

	fb->seek(info->image_offsets[index]);
	while (!done) {
		done = do_image_op(fb, &ctx);
		if (!done && (draw_flags & IMAGEF_OP_WAIT_KEYPRESS)) {
			getchar();
			g_flip_buffers();
		}
	}

	g_flip_buffers();
}

void draw_dark_room(void)
{
	g_clear_screen(G_COLOR_BLACK);
}

void draw_bright_room(void)
{
	g_clear_screen(G_COLOR_WHITE);
}

void draw_location_image(ImageData *info, unsigned index)
{
	g_clear_screen(G_COLOR_WHITE);
	draw_image(info, index);
}

static void load_image_file(ImageData *info, const char *filename,
			    unsigned file_num)
{
	unsigned base = file_num * IMAGES_PER_FILE;
	FileBuffer *fb;
	uint16 version;
	int i;

	info->fb[file_num] = FileBuffer(filename);
	fb = &info->fb[file_num];

	/*
	 * In earlier versions of Comprehend the first word is 0x1000 and
	 * the image offsets start four bytes in. In newer versions the
	 * image offsets start at the beginning of the image file.
	 */
	version = fb->readUint16LE();
	if (version == 0x1000)
		fb->seek(4);
	else
		fb->seek(0);

	/* Get the image offsets in the file */
	for (i = 0; i < IMAGES_PER_FILE; i++) {
		info->image_offsets[base + i] = fb->readUint16LE();
		if (version == 0x1000)
			info->image_offsets[base + i] += 4;
	}
}

static void load_image_files(ImageData *info,
	const Common::Array<const char *> filenames) {
	uint i;

	memset(info, 0, sizeof(*info));

	info->nr_images = filenames.size() * IMAGES_PER_FILE;
	info->fb = (FileBuffer *)xmalloc(info->nr_images * sizeof(*info->fb));
	info->image_offsets = (uint16 *)xmalloc(info->nr_images * sizeof(uint16));

	for (i = 0; i < filenames.size(); i++) {
		load_image_file(info, filenames[i], i);
	}
}

void comprehend_load_image_file(const char *filename, ImageData *info)
{
	Common::Array<const char *> filenames;
	filenames.push_back(filename);

	load_image_files(info, filenames);
}

void comprehend_load_images(ComprehendGame *game) {
	load_image_files(&game->_roomImages,
			 game->_locationGraphicFiles);

	load_image_files(&game->_itemImages,
			 game->_itemGraphicFiles);
}

} // namespace Comprehend
} // namespace Glk