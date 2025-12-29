# Delta Multimedia Encoding Specification (DM)

## Overview
DM is a broad and extensible way of storing and playing digital media.
It supports the most common forms of digital media, such as videos and images, but can also be expanded to include things like 3D models.

---

## Data Types

All multi-byte integers are **little-endian**.

| Type   | Size    | Description          |
|--------|---------|----------------------|
| `u8`   | 1 byte  | Unsigned 8-bit       |
| `u16`  | 2 bytes | Unsigned 16-bit LE   |
| `u32`  | 4 bytes | Unsigned 32-bit LE   |
| `u64`  | 8 bytes | Unsigned 64-bit LE   |

---

## Generic DM Header

```c
struct dm_header {
    u32 magic;      // 0x444D0001 ('D' 'M' 0x00 0x01)
    u32 checksum;   // CRC32 of header (with this field as 0)
    u16 version;    // Format version
    u8 type;        // Type of encoding, whether it be an image or a video
}
```

---

## Type-specific Headers (found aligned after above header)

### Images

```c
struct dm_image {
    u32 width, height;  // Image dimensions
    u32 *data;          // Pointer to beginning of image
    u8 pixel_format;    // RGBX, BRGX etc
    u8 compression;     // More on the later
};
```

---

### Videos

```c
struct dm_video {
    u32 width, height;  // Dimensions of one frame
    u32 frames;         // Total number of frames
    u32 *data;          // Pointer to the beginning of first frame
    u8 fps;             // How many frames to render each second
    u8 pixel_format;    // PRGX, BRGX etc
    u8 compression;     // More on that later
};
```

---

### Audio

```c
struct dm_audio {
    u32 sps;        // Samples per second
    u32 bps;        // Bits per sample
    u32 duration;   // How long the audio is
    u32 *data;      // Pointer to first sample
    u8 compression; // More on that later
};
```

---

## Compression types

There are many different types of available compression. The two simplest are none, and RLE.

---

### None

Basic, plain file. Can be directly mapped to video output.

---

### RLE

Run-Length Encoding. Compresses rows of identical pixels by utilising the 'X' byte in the pixel format if possible, or with a prepended byte. The data in this byte stores how many copies of that pixel to make. Example: 
The decoder comes across this word. The `pixel_format` is RGBX, and as the compression is RLE, it knows to repeat this pixel X amount of times.

`0xFF00FF04` becomes `0xFF00FF00 0xFF00FF00 0xFF00FF00 0xFF00FF00`

---

### Byte Compression

Byte Compression is similar to RLE, but instead of operating on words, it operates on the byte level.
An example of how a stream would be encoded using Byte Compression is this:

`0b00001100 0b1100110 0b01100000 0b11000000` has a repeating pattern of `0b001100`, so it could be compressed by replacing all 4 of those occurences with `0b11001100` for example