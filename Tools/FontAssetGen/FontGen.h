#pragma once

// Save as font map png
void generateFontMap(const char* fontFilePath, const char* resultPath, int pixelHeight, bool bDebugOutput = false);

// Save as font sdf png
void generateFontSDF(const char* fontFilePath, const char* resultPath, int pixelHeight, bool bDebugOutput = false);

// Save as font psdf png
void generateFontPSDF(const char* fontFilePath, const char* resultPath, int pixelHeight, bool bDebugOutput = false);

// Save as font msdf png
void generateFontMSDF(const char* fontFilePath, const char* resultPath, int pixelHeight, bool bDebugOutput = false);
