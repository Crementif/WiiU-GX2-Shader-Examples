#pragma once

struct TGA_HEADER {
    uint8_t identsize;          // size of ID field that follows 18 byte header (0 usually)
    uint8_t colourmaptype;      // type of colour map 0=none, 1=has palette
    uint8_t imagetype;          // type of image 0=none,1=indexed,2=rgb,3=grey,+8=rle packed

    uint8_t colourmapstart[2];     // first colour map entry in palette
    uint8_t colourmaplength[2];    // number of colours in palette
    uint8_t colourmapbits;      // number of bits per palette entry 15,16,24,32

    uint16_t xstart;             // image x origin
    uint16_t ystart;             // image y origin
    uint16_t width;              // image width in pixels
    uint16_t height;             // image height in pixels
    uint8_t bits;               // image bits per pixel 8,16,24,32
    uint8_t descriptor;         // image descriptor bits (vh flip bits)
};

// quick and dirty 32-bit TGA loader
static GX2Texture* TGA_LoadTexture(uint8_t* data, uint32_t length) {
    TGA_HEADER* tgaHeader = (TGA_HEADER*)data;

    uint32_t width = _swapU16(tgaHeader->width);
    uint32_t height = _swapU16(tgaHeader->height);
    if(tgaHeader->bits != 32)
    {
        WHBLogPrintf("Only 32bit TGA images are supported");
        return nullptr;
    }
    if(tgaHeader->imagetype != 2 && tgaHeader->imagetype != 3)
    {
        WHBLogPrintf("Only uncompressed TGA images are supported");
        return nullptr;
    }
    GX2Texture* texture = (GX2Texture*)MEMAllocFromDefaultHeap(sizeof(GX2Texture));
    memset(texture, 0, sizeof(GX2Texture));
    GX2SurfaceFormat gx2Format;

    texture->surface.width = width;
    texture->surface.height = height;
    texture->surface.depth = 1;
    texture->surface.mipLevels = 1;
    texture->surface.format = GX2_SURFACE_FORMAT_UNORM_R8_G8_B8_A8;
    texture->surface.aa = GX2_AA_MODE1X;
    texture->surface.use = GX2_SURFACE_USE_TEXTURE;
    texture->surface.dim = GX2_SURFACE_DIM_TEXTURE_2D;
    texture->surface.tileMode = GX2_TILE_MODE_LINEAR_ALIGNED;
    texture->surface.swizzle = 0;
    texture->viewFirstMip = 0;
    texture->viewNumMips = 1;
    texture->viewFirstSlice = 0;
    texture->viewNumSlices = 1;
    texture->compMap = 0x0010203;
    GX2CalcSurfaceSizeAndAlignment(&texture->surface);
    GX2InitTextureRegs(texture);

    if (texture->surface.imageSize == 0)
        return nullptr;
    texture->surface.image = MEMAllocFromDefaultHeapEx(texture->surface.imageSize, texture->surface.alignment);

    for (uint32_t y=0; y<height; y++) {
        uint32_t* tga_bgra_data = (uint32_t*)(data+sizeof(TGA_HEADER)) + ((height - y - 1) * width);
        uint32_t* out_data = (uint32_t*)texture->surface.image + (y * texture->surface.pitch);
        for (uint32_t x=0; x<width; x++) {
            uint32_t c = *tga_bgra_data;
            *out_data = ((c & 0x00FF00FF)) | ((c & 0xFF000000) >> 16) | ((c & 0x0000FF00) << 16);
            tga_bgra_data++;
            out_data++;
        }
    }

    // todo: create texture with optimal tile format and use GX2CopySurface to convert from linear to tiled format
    GX2Invalidate(GX2_INVALIDATE_MODE_CPU | GX2_INVALIDATE_MODE_TEXTURE, texture->surface.image, texture->surface.imageSize);
    return texture;
}

static void TGA_UnloadTexture(GX2Texture* texture) {
    if (texture == nullptr)
        return;
    if (texture->surface.image != nullptr)
        MEMFreeToDefaultHeap(texture->surface.image);
    MEMFreeToDefaultHeap(texture);
}