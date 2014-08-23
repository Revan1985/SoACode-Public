#pragma once
#include "PtrRecycler.h"

class DepthState;
class RasterizerState;
class SamplerState;
class SpriteFont;

struct VertexSpriteBatch {
public:
    VertexSpriteBatch();
    VertexSpriteBatch(const f32v3& pos, const f32v2& uv, const f32v4& uvr, const color8& color);

    f32v3 position;
    f32v2 uv;
    f32v4 uvRect;
    color8 color;
};

enum class SpriteSortMode {
    NONE,
    FRONT_TO_BACK,
    BACK_TO_FRONT,
    TEXTURE
};

class SpriteGlyph {
public:
    SpriteGlyph();
    SpriteGlyph(ui32 texID, f32 d);

    ui32 textureID;
    f32 depth;

    VertexSpriteBatch vtl;
    VertexSpriteBatch vtr;
    VertexSpriteBatch vbl;
    VertexSpriteBatch vbr;
};

class SpriteBatch {
public:
    SpriteBatch(bool isDynamic = true);
    ~SpriteBatch();

    void init();
    void dispose();

    void begin();

    void draw(ui32 t, f32v4* uvRect, f32v2* uvTiling, f32v2 position, f32v2 offset, f32v2 size, f32 rotation, color8 tint, f32 depth = 0.0f);
    void draw(ui32 t, f32v4* uvRect, f32v2* uvTiling, f32v2 position, f32v2 offset, f32v2 size, color8 tint, f32 depth = 0.0f);
    void draw(ui32 t, f32v4* uvRect, f32v2* uvTiling, f32v2 position, f32v2 size, color8 tint, f32 depth = 0.0f);
    void draw(ui32 t, f32v4* uvRect, f32v2 position, f32v2 size, color8 tint, f32 depth = 0.0f);
    void draw(ui32 t, f32v2 position, f32v2 size, color8 tint, f32 depth = 0.0f);
    void drawString(SpriteFont* font, const cString s, f32v2 position, f32v2 scaling, color8 tint, f32 depth = 0.0f);
    void drawString(SpriteFont* font, const cString s, f32v2 position, f32 desiredHeight, f32 scaleX, color8 tint, f32 depth = 0.0f);
    void end(SpriteSortMode ssm = SpriteSortMode::TEXTURE);

    void renderBatch(f32m4 mWorld, f32m4 mCamera, /*const BlendState* bs = nullptr,*/ const SamplerState* ss = nullptr, const DepthState* ds = nullptr, const RasterizerState* rs = nullptr);
    void renderBatch(f32m4 mWorld, const f32v2& screenSize, /*const BlendState* bs = nullptr,*/ const SamplerState* ss = nullptr, const DepthState* ds = nullptr, const RasterizerState* rs = nullptr);
    void renderBatch(const f32v2& screenSize, /*const BlendState* bs = nullptr,*/ const SamplerState* ss = nullptr, const DepthState* ds = nullptr, const RasterizerState* rs = nullptr);

    void sortGlyphs(SpriteSortMode ssm);
    void generateBatches();
private:
    static bool SSMTexture(SpriteGlyph* g1, SpriteGlyph* g2) {
        return g1->textureID < g2->textureID;
    }
    static bool SSMFrontToBack(SpriteGlyph* g1, SpriteGlyph* g2) {
        return g1->depth < g2->depth;
    }
    static bool SSMBackToFront(SpriteGlyph* g1, SpriteGlyph* g2) {
        return g1->depth > g2->depth;
    }

    void createProgram();
    void searchUniforms();
    void createVertexArray();

    class SpriteBatchCall {
    public:
        ui32 textureID;
        i32 indices;
        i32 indexOffset;

        void set(i32 iOff, ui32 texID, std::vector<SpriteBatchCall*>& calls);
        SpriteBatchCall* append(SpriteGlyph* g, std::vector<SpriteBatchCall*>& calls, PtrRecycler<SpriteBatchCall>* recycler);
    };

    // Glyph Information
    std::vector<SpriteGlyph*> _glyphs;
    PtrRecycler<SpriteGlyph> _glyphRecycler;

    // Render Batches
    ui32 _bufUsage;
    ui32 _vao, _vbo;
    i32 _glyphCapacity;
    std::vector<SpriteBatchCall*> _batches;
    PtrRecycler<SpriteBatchCall> _batchRecycler;

    // Custom Shader
    ui32 _idProg, _idVS, _idFS;
    ui32 _unWorld, _unVP, _unTexture;

    static const i32 _INITIAL_GLYPH_CAPACITY = 32;
};