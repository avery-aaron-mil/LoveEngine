#ifndef LOVE_ASCII_TABLE_HPP
#define LOVE_ASCII_TABLE_HPP

#include <algorithm>
#include <cstdint>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace love_engine {
    class ASCII_Table {
        public:
            enum class ASCII_Table_Flags {
                NONE = 0,
                CENTERED_HEADER = 1 << 0,
                BOLD_BORDER = 1 << 1,
                DRAW_EMPTY_CELLS = 1 << 2,
            };
            enum class ASCII_Cell_Flags {
                NONE = 0,
                CENTERED = 1 << 0,
                BOLD_BORDER = 1 << 1,
            };
            typedef struct ASCII_Table_Options_ {
                std::string tableHeader;
                uint32_t maxWidth = DEFAULT_MAX_WIDTH;
                uint32_t padding = DEFAULT_PADDING;
                ASCII_Table_Flags flags = ASCII_Table_Flags::NONE;
            } ASCII_Table_Options;
            typedef struct ASCII_Table_Cell_Position_ {
                size_t x = 0;
                size_t y = 0;
            } ASCII_Table_Cell_Position;
            typedef std::pair<std::string, ASCII_Cell_Flags> ASCII_Table_Cell;
            typedef std::vector<ASCII_Table_Cell> ASCII_Table_Row;
            typedef std::vector<ASCII_Table_Row> ASCII_Table_Matrix;

            ASCII_Table() = default;
            ~ASCII_Table() = default;

            // Dynamic generator (fixed table)
            // TODO Fixed ASCII table that updates only the data in it, storing a vector of cells that directly update their memory data,
            // unless the new length() exceeds the old length(), in which case it will have to be reassigned.
            void add_Row(const ASCII_Table_Row& row) noexcept;
            void add_Column(const ASCII_Table_Row& column) noexcept;
            void update_Row(const std::vector<std::string>& data, const size_t pos) noexcept;
            void update_Column(const std::vector<std::string>& data, const size_t pos) noexcept;
            void update_Cell(const std::string& data, const ASCII_Table_Cell_Position& pos) noexcept;

            std::string get_ASCII_Table() const noexcept;

            // Static generator (one-time use table)
            static inline ASCII_Table_Cell generate_ASCII_Table_Cell(const std::string& data) noexcept {
                return std::make_pair(data, ASCII_Cell_Flags::NONE);
            }

            static inline std::string generate_ASCII_Table(const std::vector<std::vector<std::string>>& matrix) noexcept {
                return generate_ASCII_Table("", matrix);
            }
            static std::string generate_ASCII_Table(
                const std::string& header,
                const std::vector<std::vector<std::string>>& matrix
            ) noexcept;
            static std::string generate_ASCII_Table(const ASCII_Table_Options& options, const ASCII_Table_Matrix& matrix) noexcept;

        private:
            static constexpr uint32_t DEFAULT_MAX_WIDTH = 64;
            static constexpr uint32_t DEFAULT_PADDING = 1;
    };
}

#endif // LOVE_ASCII_TABLE_HPP
