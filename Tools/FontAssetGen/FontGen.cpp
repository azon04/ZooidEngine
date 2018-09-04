#include "FontGen.h"

#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <map>

#include "lodepng.h"
#include "msdfgen.h"
#include "msdfgen-ext.h"

#include "../Common/Dir.h"

#include "ft2build.h"
#include FT_FREETYPE_H

#define MIN(a, b) (a) > (b) ? (b) : (a)

#define LARGE 1e240

#define PX_RANGE 4.0

// Hack to get FT_Face from msdfgen::FontHandle
class HackFontHandle
{
	FT_Face face;

public:
	FT_Face& getFace() { return face; }
};

struct FontGlyphDesc
{
	unsigned long charCode;
	unsigned int posX;
	unsigned int posY;
	unsigned int height;
	unsigned int width;
	unsigned int advance;
	int bearingX;
	int bearingY;
};

struct FontDesc
{
	std::string textureName;
	std::string methodName;
	unsigned int pixelHeight;
};

void copyTexture(unsigned char* dest, int width, int height, int targetX, int targetY, unsigned char* src, int srcWidth, int srcHeight)
{
	size_t itemSize = sizeof(unsigned char);
	void* pDest = (void*)(reinterpret_cast<uintptr_t>(dest) + (itemSize * width * targetY) + (targetX * itemSize));
	void* pSource = src;
	for (int i = 0; i < srcHeight; i++)
	{
		std::memcpy(pDest, pSource, itemSize * srcWidth);
		pDest = (void*)(reinterpret_cast<uintptr_t>(pDest) + (itemSize * width));
		pSource = (void*)(reinterpret_cast<uintptr_t>(pSource) + (itemSize * srcWidth));
	}
}

void copyTexture(msdfgen::Bitmap<float>& dest, msdfgen::Bitmap<float>& source, int xOffset, int yOffset, bool yFlipped = true)
{
	int yStartPos = yFlipped ? dest.height() - source.height() - 1 - yOffset : yOffset;
	for (int x = 0; x < source.width(); x++)
	{
		for (int y = 0; y < source.height(); y++)
		{
			dest(xOffset + x, yStartPos + y) = source(x, y);
		}
	}
}

void copyTexture(msdfgen::Bitmap<msdfgen::FloatRGB>& dest, msdfgen::Bitmap<msdfgen::FloatRGB>& source, int xOffset, int yOffset, bool yFlipped = true)
{
	int yStartPos = yFlipped ? dest.height() - source.height() - 1 - yOffset : yOffset;
	for (int x = 0; x < source.width(); x++)
	{
		for (int y = 0; y < source.height(); y++)
		{
			dest(xOffset + x, yStartPos + y) = source(x, y);
		}
	}
}

void saveGreyPng(unsigned char* greyBitmap, unsigned int height, unsigned int width, const char* fileName)
{
	std::vector<unsigned char> pixels(height*width);
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			pixels[y * width + x] = greyBitmap[y * width + x];
		}
	}

	lodepng::encode(fileName, pixels, width, height, LCT_GREY);
}

void saveGreyPng(float* greyBitmap, unsigned int height, unsigned int width, const char* fileName)
{
	std::vector<unsigned char> pixels(height*width);
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			pixels[y * width + x] = MIN((int)greyBitmap[y * width + x], 0xff);
		}
	}
	
	lodepng::encode(fileName, pixels, width, height, LCT_GREY);
}

void saveRGB(msdfgen::FloatRGB* rgbBitmap, unsigned int height, unsigned int width, const char* fileName)
{
	std::vector<unsigned char> pixels(3 * height * width);
	int idx = 0;
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			pixels[idx++] = MIN(int(rgbBitmap[y * width + y].r), 0xff);
			pixels[idx++] = MIN(int(rgbBitmap[y * width + y].g), 0xff);
			pixels[idx++] = MIN(int(rgbBitmap[y * width + y].b), 0xff);
		}
	}

	lodepng::encode(fileName, pixels, width, height, LCT_RGB);
}

int getAllCharCode(std::vector<FontGlyphDesc>& fontMap, FT_Face& face)
{
	std::cout << "Populating All Character Code..." << std::endl;

	int actualCharCount = 1;
	unsigned int glyphIndex = 0;
	fontMap[0].charCode = 0;
	unsigned long charCode = FT_Get_First_Char(face, &glyphIndex);
	while (glyphIndex != 0)
	{
		actualCharCount++;
		fontMap[glyphIndex].charCode = charCode;
		charCode = FT_Get_Next_Char(face, charCode, &glyphIndex);
	}

	std::cout << "Found " << actualCharCount << " characters available"<< std::endl;
	return actualCharCount;
}

int calculateTextureDimAndPopulateFontMapData(std::vector<FontGlyphDesc>& fontMap, FT_Face& face, int actualCharCount, int pixelHeight, int padding)
{
	std::cout << "Calculating Possible Texture Dimension..." << std::endl;

	int textureDimension = -1;
	int charCount = face->num_glyphs;
	int numberPerRow = (int)(sqrt((pixelHeight + padding) / (0.5f * pixelHeight + padding) * actualCharCount));

	int curPos[2] = { padding, padding };
	for (int c = 0; c < charCount; c++)
	{
		if ((fontMap[c].charCode == 0 && c != 0) || FT_Load_Glyph(face, c, FT_LOAD_RENDER))
		{
			continue;
		}

		fontMap[c].posX = curPos[0];
		fontMap[c].posY = curPos[1];
		fontMap[c].width = face->glyph->bitmap.width;
		fontMap[c].height = face->glyph->bitmap.rows;
		fontMap[c].advance = face->glyph->advance.x >> 6;
		fontMap[c].bearingX = face->glyph->bitmap_left;
		fontMap[c].bearingY = face->glyph->bitmap_top;

		curPos[0] += padding + face->glyph->bitmap.width;

		if (textureDimension < curPos[0])
		{
			textureDimension = curPos[0];
		}

		if ((c + 1) % numberPerRow == 0)
		{
			curPos[0] = padding;
			curPos[1] = curPos[1] + padding + pixelHeight;
		}

		if (textureDimension < curPos[1])
		{
			textureDimension = curPos[1];
		}
	}

	textureDimension += padding;

	textureDimension = (textureDimension / 4 + 1) * 4;

	std::cout << "Texture Dimension " << textureDimension << "x" << textureDimension << std::endl;


	return textureDimension;
}

void cleanUpFontMap(std::vector<FontGlyphDesc>& fontMap)
{
	for (int i = fontMap.size() - 1; i > 0; i--)
	{
		if (fontMap[i].charCode == 0)
		{
			fontMap.erase(fontMap.begin() + i);
		}
	}
}

void generateDescFile(std::vector<FontGlyphDesc>& fontMap, const char* resultPath, const char* fileName, FontDesc& fontDesc)
{
	std::string fullPath = std::string(resultPath) + "\\" + fileName + ".zFont";
	std::ofstream outStream;
	outStream.open(fullPath, std::ofstream::out);

	outStream << "Image " << fontDesc.textureName << std::endl;
	outStream << "Method " << fontDesc.methodName << std::endl;
	outStream << "GlyphHeight " << fontDesc.pixelHeight << std::endl;
	outStream << "Glyph "<< fontMap.size() << std::endl;
	for (int i = 0; i < fontMap.size(); i++)
	{
		FontGlyphDesc& desc = fontMap[i];
		outStream << desc.charCode << " ";
		outStream << desc.posX << " " << desc.posY << " ";
		outStream << desc.width << " " << desc.height << " ";
		outStream << desc.bearingX << " " << desc.bearingY << " ";
		outStream << desc.advance ;
		outStream << std::endl;
	}
}

void generateFontMap(const char* fontFilePath, const char* resultPath, int pixelHeight, bool bDebugOutput)
{
	std::string outFileName = ZETools::Dir::GetFileName(fontFilePath);
	
	std::cout << "Load " << fontFilePath << std::endl;

	FT_Library ft;
	if (FT_Init_FreeType(&ft))
	{
		std::cerr << "Couldn't init Freetype Library" << std::endl;
		return;
	}

	FT_Face face;
	if (FT_New_Face(ft, fontFilePath, 0, &face))
	{
		std::cerr << "Couldn't load font from: " << fontFilePath << std::endl;
		return;
	}

	FT_Set_Pixel_Sizes(face, 0, pixelHeight);

	int padding = 5;
	int charCount = face->num_glyphs;
	std::vector<FontGlyphDesc> fontMap(charCount);
	
	// Get all char code
	int actualCharCount = getAllCharCode(fontMap, face);

	// Calculate possible character per row and texture dimension
	int textureDimension = calculateTextureDimAndPopulateFontMapData(fontMap, face, actualCharCount, pixelHeight, padding);

	unsigned char* grayMap = new unsigned char[textureDimension * textureDimension];
	std::memset(grayMap, 0, sizeof(unsigned char) * textureDimension * textureDimension);

	std::cout << "Populating Char Glyph into output texture ..." << std::endl;
	int percentage = 0;
	for (int c = 0; c < charCount; c++)
	{
		if ((fontMap[c].charCode == 0 && c != 0) || FT_Load_Glyph(face, c, FT_LOAD_RENDER))
		{
			continue;
		}

		if (percentage != int(100 * c / (float)charCount))
		{
			percentage = 100 * c / (float)charCount;
			std::cout << percentage << "%" << std::endl;
		}

		FontGlyphDesc& desc = fontMap[c];

		if(bDebugOutput)
			saveGreyPng(face->glyph->bitmap.buffer, desc.height, desc.width, (std::to_string(c) + ".png").c_str());

		copyTexture(grayMap, textureDimension, textureDimension, desc.posX, desc.posY, face->glyph->bitmap.buffer, desc.width, desc.height);
	}

	std::string fullPath = (std::string(resultPath) + "\\" + outFileName + ".png");
	std::cout << "Saving the generated texture to: " << fullPath << std::endl;
	saveGreyPng(grayMap, textureDimension, textureDimension, fullPath.c_str());
	
	cleanUpFontMap(fontMap);
	std::cout << "Saving the font desc file to: " << resultPath << outFileName << ".zFont" << std::endl;

	FontDesc fontDesc;
	fontDesc.methodName = "TEXTURE";
	fontDesc.textureName = outFileName + ".png";
	fontDesc.pixelHeight = pixelHeight;
	generateDescFile(fontMap, resultPath, outFileName.c_str(), fontDesc);
	
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
}

void generateFontSDF(const char* fontFilePath, const char* resultPath, int pixelHeight, bool bDebugOutput)
{
	std::string outFileName = ZETools::Dir::GetFileName(fontFilePath)+"_SDF";

	std::cout << "Load " << fontFilePath << std::endl;

	msdfgen::FreetypeHandle *ft = msdfgen::initializeFreetype();
	if (!ft)
	{
		std::cerr << "Couldn't init Freetype Library" << std::endl;
		return;
	}

	msdfgen::FontHandle *font = msdfgen::loadFont(ft, fontFilePath);
	if (!font)
	{
		std::cerr << "Couldn't load font from: " << fontFilePath << std::endl;
		return;
	}

	HackFontHandle* hackHandle = reinterpret_cast<HackFontHandle*>(font);
	FT_Face& face = hackHandle->getFace();

	FT_Set_Pixel_Sizes(face, 0, pixelHeight);

	int padding = 5;
	int charCount = face->num_glyphs;
	std::vector<FontGlyphDesc> fontMap(charCount);

	// Get all char code
	int actualCharCount = getAllCharCode(fontMap, face);

	// Calculate possible character per row and texture dimension
	int textureDimension = calculateTextureDimAndPopulateFontMapData(fontMap, face, actualCharCount, pixelHeight, 2 * padding);

	std::cout << "Generating SDF Char Glyph into output texture ..." << std::endl;
	int percentage = 0;

	msdfgen::Bitmap<float> floatMap(textureDimension, textureDimension);
	for (int c = 0; c < charCount; c++)
	{
		msdfgen::Shape shape;
		double advance;
		FontGlyphDesc& desc = fontMap[c];

		if ((desc.charCode == 0 && c != 0) || !msdfgen::loadGlyph(shape, font, desc.charCode, &advance))
		{
			continue;
		}

		if (percentage != int(100 * c / (float)charCount))
		{
			percentage = 100 * c / (float)charCount;
			std::cout << percentage << "%" << std::endl;
		}

		shape.normalize();

		msdfgen::Bitmap<float> charMap(desc.width + 2 * padding, desc.height + 2 * padding);

		double l = LARGE, r = -LARGE, t = -LARGE , b = LARGE;
		shape.bounds(l, b, r, t);
		msdfgen::Vector2 frame(charMap.width(), charMap.height());
		frame -= 2 * padding;
		msdfgen::Vector2 scale(1.0);
		if (l >= r || b >= t)
			l = 0, b = 0, r = 1, t = 1;
		msdfgen::Vector2 dims(r - l, t - b);
		msdfgen::Vector2 trans(0.0);

		if (dims.x*frame.y < dims.y*frame.x) {
			trans.set(.5*(frame.x / frame.y*dims.y - dims.x) - l, -b);
			scale = frame.y / dims.y;
		}
		else {
			trans.set(-l, .5*(frame.y / frame.x*dims.x - dims.y) - b);
			scale = frame.x / dims.x;
		}

		double range = padding / scale.x;
		trans += padding / scale;

		msdfgen::generateSDF(charMap, shape, range, scale, trans);

		copyTexture(floatMap, charMap, desc.posX - padding, desc.posY - padding);
		
		// TODO Fit desc to generated frame of SDF
		desc.posY -= padding;
		desc.posX -= padding;
		if (desc.width > 0) { desc.width += 2 * padding; }
		if (desc.height > 0) { desc.height += 2 * padding; }
		desc.bearingX -= padding;
		desc.bearingY += padding;

		if (bDebugOutput)
			msdfgen::savePng(charMap, (std::to_string( desc.charCode ) + ".png").c_str());
	}
	
	std::string fullPath = (std::string(resultPath) + "\\" + outFileName + ".png");
	std::cout << "Saving the generated texture to: " << fullPath << std::endl;
	msdfgen::savePng(floatMap, fullPath.c_str());
	
	cleanUpFontMap(fontMap);
	std::cout << "Saving the font desc file to: " << resultPath << outFileName << ".zFont" << std::endl;
	
	FontDesc fontDesc;
	fontDesc.methodName = "SDF";
	fontDesc.textureName = outFileName + ".png";
	fontDesc.pixelHeight = pixelHeight;
	generateDescFile(fontMap, resultPath, outFileName.c_str(), fontDesc);

	msdfgen::destroyFont(font);
	msdfgen::deinitializeFreetype(ft);
}

void generateFontPSDF(const char* fontFilePath, const char* resultPath, int pixelHeight, bool bDebugOutput)
{
	std::string outFileName = ZETools::Dir::GetFileName(fontFilePath) + "_PSDF";

	std::cout << "Load " << fontFilePath << std::endl;

	msdfgen::FreetypeHandle *ft = msdfgen::initializeFreetype();
	if (!ft)
	{
		std::cerr << "Couldn't init Freetype Library" << std::endl;
		return;
	}

	msdfgen::FontHandle *font = msdfgen::loadFont(ft, fontFilePath);
	if (!font)
	{
		std::cerr << "Couldn't load font from: " << fontFilePath << std::endl;
		return;
	}

	HackFontHandle* hackHandle = reinterpret_cast<HackFontHandle*>(font);
	FT_Face& face = hackHandle->getFace();

	FT_Set_Pixel_Sizes(face, 0, pixelHeight);

	int padding = 5;
	int charCount = face->num_glyphs;
	std::vector<FontGlyphDesc> fontMap(charCount);

	// Get all char code
	int actualCharCount = getAllCharCode(fontMap, face);

	// Calculate possible character per row and texture dimension
	int textureDimension = calculateTextureDimAndPopulateFontMapData(fontMap, face, actualCharCount, pixelHeight, 2 * padding);

	std::cout << "Generating Pseudo-SDF Char Glyph into output texture ..." << std::endl;
	int percentage = 0;

	msdfgen::Bitmap<float> floatMap(textureDimension, textureDimension);
	for (int c = 0; c < charCount; c++)
	{
		msdfgen::Shape shape;
		double advance;
		FontGlyphDesc& desc = fontMap[c];

		if ((desc.charCode == 0 && c != 0) || !msdfgen::loadGlyph(shape, font, desc.charCode, &advance))
		{
			continue;
		}

		if (percentage != int(100 * c / (float)charCount))
		{
			percentage = 100 * c / (float)charCount;
			std::cout << percentage << "%" << std::endl;
		}

		shape.normalize();

		msdfgen::Bitmap<float> charMap(desc.width + 2 * padding, desc.height + 2 * padding);

		double l = LARGE, r = -LARGE, t = -LARGE, b = LARGE;
		shape.bounds(l, b, r, t);
		msdfgen::Vector2 frame(charMap.width(), charMap.height());
		frame -= 2 * padding;
		msdfgen::Vector2 scale(1.0);
		if (l >= r || b >= t)
			l = 0, b = 0, r = 1, t = 1;
		msdfgen::Vector2 dims(r - l, t - b);
		msdfgen::Vector2 trans(0.0);

		if (dims.x*frame.y < dims.y*frame.x) {
			trans.set(.5*(frame.x / frame.y*dims.y - dims.x) - l, -b);
			scale = frame.y / dims.y;
		}
		else {
			trans.set(-l, .5*(frame.y / frame.x*dims.x - dims.y) - b);
			scale = frame.x / dims.x;
		}

		double range = padding / scale.x;
		trans += padding / scale;

		msdfgen::generatePseudoSDF(charMap, shape, range, scale, trans);

		copyTexture(floatMap, charMap, desc.posX - padding, desc.posY - padding);

		// TODO Fit desc to generated frame of SDF
		desc.posY -= padding;
		desc.posX -= padding;
		if (desc.width > 0) { desc.width += 2 * padding; }
		if (desc.height > 0) { desc.height += 2 * padding; }
		desc.bearingX -= padding;
		desc.bearingY += padding;

		if (bDebugOutput)
			msdfgen::savePng(charMap, (std::to_string(desc.charCode) + ".png").c_str());
	}

	std::string fullPath = (std::string(resultPath) + "\\" + outFileName + ".png");
	std::cout << "Saving the generated texture to: " << fullPath << std::endl;
	msdfgen::savePng(floatMap, fullPath.c_str());

	cleanUpFontMap(fontMap);
	std::cout << "Saving the font desc file to: " << resultPath << outFileName << ".zFont" << std::endl;
	
	FontDesc fontDesc;
	fontDesc.methodName = "PSDF";
	fontDesc.textureName = outFileName + ".png";
	fontDesc.pixelHeight = pixelHeight;
	generateDescFile(fontMap, resultPath, outFileName.c_str(), fontDesc);

	msdfgen::destroyFont(font);
	msdfgen::deinitializeFreetype(ft);
}

void generateFontMSDF(const char* fontFilePath, const char* resultPath, int pixelHeight, bool bDebugOutput)
{
	std::string outFileName = ZETools::Dir::GetFileName(fontFilePath) + "_MSDF";

	std::cout << "Load " << fontFilePath << std::endl;

	msdfgen::FreetypeHandle *ft = msdfgen::initializeFreetype();
	if (!ft)
	{
		std::cerr << "Couldn't init Freetype Library" << std::endl;
		return;
	}

	msdfgen::FontHandle *font = msdfgen::loadFont(ft, fontFilePath);
	if (!font)
	{
		std::cerr << "Couldn't load font from: " << fontFilePath << std::endl;
		return;
	}

	HackFontHandle* hackHandle = reinterpret_cast<HackFontHandle*>(font);
	FT_Face& face = hackHandle->getFace();

	FT_Set_Pixel_Sizes(face, 0, pixelHeight);

	int padding = 5;
	int charCount = face->num_glyphs;
	std::vector<FontGlyphDesc> fontMap(charCount);

	// Get all char code
	int actualCharCount = getAllCharCode(fontMap, face);

	// Calculate possible character per row and texture dimension
	int textureDimension = calculateTextureDimAndPopulateFontMapData(fontMap, face, actualCharCount, pixelHeight, 2 * padding);

	std::cout << "Generating MultiChannel-SDF Char Glyph into output texture ..." << std::endl;
	int percentage = 0;

	msdfgen::Bitmap<msdfgen::FloatRGB> floatMap(textureDimension, textureDimension);
	for (int c = 0; c < charCount; c++)
	{
		msdfgen::Shape shape;
		double advance;
		FontGlyphDesc& desc = fontMap[c];

		if ((desc.charCode == 0 && c != 0) || !msdfgen::loadGlyph(shape, font, desc.charCode, &advance))
		{
			continue;
		}

		if (percentage != int(100 * c / (float)charCount))
		{
			percentage = 100 * c / (float)charCount;
			std::cout << percentage << "%" << std::endl;
		}

		shape.normalize();
		
		msdfgen::edgeColoringSimple(shape, 3.0);

		msdfgen::Bitmap<msdfgen::FloatRGB> charMap(desc.width + 2 * padding, desc.height + 2 * padding);

		double l = LARGE, r = -LARGE, t = -LARGE, b = LARGE;
		shape.bounds(l, b, r, t);
		msdfgen::Vector2 frame(charMap.width(), charMap.height());
		frame -= 2 * padding;
		msdfgen::Vector2 scale(1.0);
		if (l >= r || b >= t)
			l = 0, b = 0, r = 1, t = 1;
		msdfgen::Vector2 dims(r - l, t - b);
		msdfgen::Vector2 trans(0.0);

		if (dims.x*frame.y < dims.y*frame.x) {
			trans.set(.5*(frame.x / frame.y*dims.y - dims.x) - l, -b);
			scale = frame.y / dims.y;
		}
		else {
			trans.set(-l, .5*(frame.y / frame.x*dims.x - dims.y) - b);
			scale = frame.x / dims.x;
		}

		double range = padding / scale.x;
		trans += padding / scale;

		msdfgen::generateMSDF(charMap, shape, range, scale, trans);

		copyTexture(floatMap, charMap, desc.posX - padding, desc.posY - padding);

		// TODO Fit desc to generated frame of SDF
		desc.posY -= padding;
		desc.posX -= padding;
		if (desc.width > 0) { desc.width += 2 * padding; }
		if (desc.height > 0) { desc.height += 2 * padding; }
		desc.bearingX -= padding;
		desc.bearingY += padding;

		if (bDebugOutput)
			msdfgen::savePng(charMap, (std::to_string(desc.charCode) + ".png").c_str());
	}

	std::string fullPath = (std::string(resultPath) + "\\" + outFileName + ".png");
	std::cout << "Saving the generated texture to: " << fullPath << std::endl;
	msdfgen::savePng(floatMap, fullPath.c_str());

	cleanUpFontMap(fontMap);
	std::cout << "Saving the font desc file to: " << resultPath << outFileName << ".zFont" << std::endl;
	
	FontDesc fontDesc;
	fontDesc.methodName = "MSDF";
	fontDesc.textureName = outFileName + ".png";
	fontDesc.pixelHeight = pixelHeight;
	generateDescFile(fontMap, resultPath, outFileName.c_str(), fontDesc);

	msdfgen::destroyFont(font);
	msdfgen::deinitializeFreetype(ft);
}
