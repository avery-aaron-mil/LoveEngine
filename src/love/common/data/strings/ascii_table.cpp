#include "ascii_table.hpp"

#include <cstring>
#include <stdexcept>

namespace love_engine {
    typedef struct ASCII_Table_Cursor_ {
        uint16_t pos = 0;
        uint16_t len; // Length of entire string
        uint16_t lineLen; // Length of each line
        uint16_t rowLen; // Length of the entire row of cells
        uint8_t lines = 1;
    } ASCII_Table_Cursor;
    typedef std::vector<std::string> ASCII_Table_Rows;
    typedef std::vector<std::vector<ASCII_Table_Cursor>> ASCII_Table_Cursors;

    void resize_Column(const uint16_t width, const uint16_t column) {
        ASCII_Table_Rows rows;
        ASCII_Table_Cursors cursors;
        for (auto row : rows) {
            
        }
    }

    void update_Cell(const uint16_t maxCellDataWidth, const uint16_t row, const uint16_t column, const std::string& src) {
        ASCII_Table_Rows rows;
        ASCII_Table_Cursors cursors;
        // Input validation
        if (rows.size() != cursors.size()) throw std::out_of_range("Data row count is not equal to cursor row count.");
        if (cursors.empty()) {
            // TODO add_Row();
            return;
        }
        if (row > cursors.size()) {
            if (row == rows.size() + 1) {
                // TODO add_Row();
                return;
            }
            else throw std::out_of_range("Row to update in ASCII table is larger than the current amount of rows + 1.");
        }
        if (column > cursors[row].size()) {
            if (column == cursors[row].size() + 1) {
                // TODO add_Column();
                return;
            }
            else throw std::out_of_range("Column to update in ASCII table is larger than the current amount of columns + 1.");
        }

        ASCII_Table_Cursor& cursor = cursors[row][column];
        uint16_t remainingLen = cursor.len;

        // Check if column needs to be resized
        if (src.length() > cursor.len) {
            const uint16_t newLineLen = static_cast<uint16_t>(
                std::min(src.length(), static_cast<size_t>(maxCellDataWidth))
            );
            cursor.rowLen += newLineLen - cursor.lineLen;
            cursor.lineLen = newLineLen;
            resize_Column(cursor.len, column);
        }

        // Update table
        size_t pos = cursor.pos;
        uint16_t writeLen;
        uint8_t lines;
        for (lines = 0; remainingLen > 0; lines++) {
            writeLen = std::min(remainingLen, cursor.lineLen);
            rows[row].replace(pos, writeLen, src);
            pos += cursor.rowLen;
            remainingLen -= writeLen;
        }
        cursor.lines = lines;
    }

}
