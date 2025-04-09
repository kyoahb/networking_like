#pragma once
#include "Utils/Imports/raylib_wrapper.h"
#include <sstream>


class RenderUtils {
public:
	/**
	* Wraps text into a vector of lines based on the specified wrap length
	* @param text The text to wrap
	* @param wrap_length The maximum length of each line
	* @param font_size The size of the font
	* @param text_spacing The spacing between lines
	* @param font The font to use for rendering
	* 
	* @return A vector of strings, each representing a line of wrapped text
	*/
	static std::vector<std::string> wrap_to_lines(const std::string& text, float wrap_length, float font_size, float text_spacing, Font font)
	{
        std::vector<std::string> lines;
        std::string current_line;

        // Process the text word by word
        std::istringstream stream(text);
        std::string word;
        bool first_word = true;

        while (stream >> word) {
            // Add a space before words (except the first word in a line)
            if (!first_word) {
                word = " " + word;
            }

            // Measure the width of this word
            float word_width = MeasureTextEx(font, word.c_str(), font_size, text_spacing).x;

            // Measure the current line with this word added
            float new_line_width = MeasureTextEx(font, (current_line + word).c_str(), font_size, text_spacing).x;

            // If adding this word would exceed the wrap length, start a new line
            if (!current_line.empty() && new_line_width > wrap_length) {
                lines.push_back(current_line);
                current_line = word;
                first_word = true;  // This is now the first word in the new line
            }
            else {
                current_line += word;
                first_word = false;
            }
        }

        // Add the last line if it's not empty
        if (!current_line.empty()) {
            lines.push_back(current_line);
        }

        return lines;
    };
};