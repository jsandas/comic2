#include "comic2/resource_loader.hpp"

#include <fstream>
#include <stdexcept>
#include <string>

namespace comic2 {
namespace {

std::uint16_t read_u16(std::span<const std::uint8_t> bytes, std::size_t off) {
    if (off + 1 >= bytes.size()) {
        throw std::runtime_error("read_u16 out of range");
    }
    return static_cast<std::uint16_t>(bytes[off] | (static_cast<std::uint16_t>(bytes[off + 1]) << 8));
}

}  // namespace

std::vector<std::uint8_t> load_file_bytes(const std::filesystem::path& path) {
    std::ifstream input(path, std::ios::binary);
    if (!input) {
        throw std::runtime_error("failed to open file: " + path.string());
    }

    input.seekg(0, std::ios::end);
    const auto size = static_cast<std::size_t>(input.tellg());
    input.seekg(0, std::ios::beg);

    std::vector<std::uint8_t> bytes(size);
    if (size > 0) {
        input.read(reinterpret_cast<char*>(bytes.data()), static_cast<std::streamsize>(size));
        if (!input) {
            throw std::runtime_error("failed to read file: " + path.string());
        }
    }
    return bytes;
}

SignedRleResult decode_signed_rle(std::span<const std::uint8_t> encoded) {
    SignedRleResult out;
    std::size_t i = 0;
    while (i < encoded.size()) {
        const auto control = static_cast<std::int8_t>(encoded[i++]);
        if (control == 0) {
            out.consumed = i;
            return out;
        }

        if (control < 0) {
            if (i >= encoded.size()) {
                throw std::runtime_error("signed-rle truncated run value");
            }
            const auto value = encoded[i++];
            const auto count = static_cast<std::size_t>(-control);
            out.bytes.insert(out.bytes.end(), count, value);
            continue;
        }

        const auto count = static_cast<std::size_t>(control);
        if (i + count > encoded.size()) {
            throw std::runtime_error("signed-rle truncated literal");
        }
        out.bytes.insert(out.bytes.end(), encoded.begin() + static_cast<std::ptrdiff_t>(i),
                         encoded.begin() + static_cast<std::ptrdiff_t>(i + count));
        i += count;
    }

    throw std::runtime_error("signed-rle missing terminator");
}

Ega4PlaneImage decode_ega_4plane_rle(std::span<const std::uint8_t> encoded, bool require_full_consumption) {
    if (encoded.size() < sizeof(EgaRle4PlaneHeader)) {
        throw std::runtime_error("ega-rle data too small");
    }

    Ega4PlaneImage image;
    image.row_span_bytes = read_u16(encoded, 0);
    if (image.row_span_bytes == 0) {
        throw std::runtime_error("ega-rle row span cannot be zero");
    }

    std::size_t i = sizeof(EgaRle4PlaneHeader);
    for (std::size_t plane = 0; plane < image.planes.size(); ++plane) {
        auto& out = image.planes[plane];
        out.reserve(image.row_span_bytes);
        while (out.size() < image.row_span_bytes) {
            if (i >= encoded.size()) {
                throw std::runtime_error("ega-rle truncated packet stream");
            }

            const std::uint8_t packet = encoded[i++];
            if ((packet & 0x80U) == 0) {
                const auto count = static_cast<std::size_t>(packet);
                if (i + count > encoded.size()) {
                    throw std::runtime_error("ega-rle truncated literal packet");
                }
                out.insert(out.end(), encoded.begin() + static_cast<std::ptrdiff_t>(i),
                           encoded.begin() + static_cast<std::ptrdiff_t>(i + count));
                i += count;
            } else {
                const auto count = static_cast<std::size_t>(packet & 0x7FU);
                if (i >= encoded.size()) {
                    throw std::runtime_error("ega-rle truncated run packet");
                }
                const auto value = encoded[i++];
                out.insert(out.end(), count, value);
            }

            if (out.size() > image.row_span_bytes) {
                throw std::runtime_error("ega-rle plane overrun");
            }
        }
    }

    if (require_full_consumption && i != encoded.size()) {
        throw std::runtime_error("ega-rle trailing bytes after 4-plane decode");
    }

    return image;
}

Ega4PlaneImage load_frpak_fullscreen_image(const std::filesystem::path& path) {
    const auto bytes = load_file_bytes(path);
    return decode_ega_4plane_rle(bytes, true);
}

SignedRleResult load_signed_rle_blob(const std::filesystem::path& path) {
    const auto bytes = load_file_bytes(path);
    return decode_signed_rle(bytes);
}

FrdataRoomEntry read_frdata_room_entry(std::span<const std::uint8_t> frdata_bytes, std::uint16_t room_index) {
    constexpr std::size_t kHeaderRoomTable = 0x0004;
    constexpr std::size_t kRoomEntrySize = sizeof(FrdataRoomEntry);
    const auto off = kHeaderRoomTable + static_cast<std::size_t>(room_index) * kRoomEntrySize;
    if (off + kRoomEntrySize > frdata_bytes.size()) {
        throw std::runtime_error("frdata room entry out of range");
    }

    FrdataRoomEntry e{};
    e.tile_w = read_u16(frdata_bytes, off + 0);
    e.tile_h = read_u16(frdata_bytes, off + 2);
    e.rle_data_off = read_u16(frdata_bytes, off + 4);
    return e;
}

}  // namespace comic2
