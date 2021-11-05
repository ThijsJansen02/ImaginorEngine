
namespace IME
{

    void drawTilesInClipSpace(const TileWorld &tileworld, const Camera2D &camera, const mat3 &rastermatrix, Canvas canvas)
    {
        mat3 rasterviewprojection = rastermatrix * camera.projection * translateMat3(-1.0f * camera.position);

        real32 chunkheight = tileworld.chunkdim * tileworld.tileheight;
        real32 chunkwidth = tileworld.chunkdim * tileworld.tilewidth;

        BoundingBox centertilebounds;
        centertilebounds.bottomleft = {0.0f, 0.0f};
        centertilebounds.topright = {chunkwidth, chunkheight};

        BoundingBox absoluteCameraBounds;
        absoluteCameraBounds.bottomleft = camera.position + camera.clippingspace.bottomleft;
        absoluteCameraBounds.topright = camera.position + camera.clippingspace.topright;

        BoundingBox absoluteCameraBoundsOrigin = absoluteCameraBounds;

        if (absoluteCameraBounds.bottomleft.x < centertilebounds.bottomleft.x)
        {

            BoundingBox drawingbounds = absoluteCameraBounds;
            drawingbounds.bottomleft.x += chunkwidth;
            drawingbounds.topright.x = chunkwidth;

            if (drawingbounds.bottomleft.y < 0.0f)
            {
                drawingbounds.bottomleft.y = 0.0f;
            }

            if (drawingbounds.topright.y >= chunkheight)
            {
                drawingbounds.topright.y = chunkheight;
            }

            drawTileChunckRegion(floorf(drawingbounds.bottomleft.x), floorf(drawingbounds.bottomleft.y),
                                 floorf(drawingbounds.topright.x), floorf(drawingbounds.topright.y), canvas, tileworld.loadedchunks[3],
                                 rasterviewprojection * translateMat3({-chunkwidth, 0.0f}));

            absoluteCameraBounds.bottomleft.x = 0.0f;
        }

        if (absoluteCameraBounds.bottomleft.y < centertilebounds.bottomleft.y)
        {

            BoundingBox drawingbounds = absoluteCameraBounds;
            drawingbounds.bottomleft.y += chunkheight;
            drawingbounds.topright.y = chunkheight;

            if (drawingbounds.bottomleft.x < 0.0f)
            {
                drawingbounds.bottomleft.x = 0.0f;
            }

            if (drawingbounds.topright.x >= chunkwidth)
            {
                drawingbounds.topright.x = chunkwidth;
            }

            drawTileChunckRegion(floorf(drawingbounds.bottomleft.x), floorf(drawingbounds.bottomleft.y),
                                 floorf(drawingbounds.topright.x), floorf(drawingbounds.topright.y), canvas, tileworld.loadedchunks[1 + 2 * 3],
                                 rasterviewprojection * translateMat3({0.0f, -chunkheight}));

            absoluteCameraBounds.bottomleft.y = 0.0f;
        }

        if (absoluteCameraBounds.topright.x >= centertilebounds.topright.x)
        {

            BoundingBox drawingbounds = absoluteCameraBounds;
            drawingbounds.topright.x -= chunkwidth;
            drawingbounds.bottomleft.x = 0.0f;

            if (drawingbounds.bottomleft.y < 0.0f)
            {
                drawingbounds.bottomleft.y = 0.0f;
            }

            if (drawingbounds.topright.y >= chunkheight)
            {
                drawingbounds.topright.y = chunkheight;
            }

            drawTileChunckRegion(floorf(drawingbounds.bottomleft.x), floorf(drawingbounds.bottomleft.y),
                                 floorf(drawingbounds.topright.x), floorf(drawingbounds.topright.y), canvas, tileworld.loadedchunks[2 + 1 * 3],
                                 rasterviewprojection * translateMat3({chunkwidth, 0.0f}));

            absoluteCameraBounds.topright.x = centertilebounds.topright.x;
        }

        if (absoluteCameraBounds.topright.y >= centertilebounds.topright.y)
        {
            BoundingBox drawingbounds = absoluteCameraBounds;
            drawingbounds.topright.y -= chunkwidth;
            drawingbounds.bottomleft.y = 0.0f;

            if (drawingbounds.bottomleft.x < 0.0f)
            {
                drawingbounds.bottomleft.x = 0.0f;
            }

            if (drawingbounds.topright.x >= chunkwidth)
            {
                drawingbounds.topright.x = chunkwidth;
            }

            drawTileChunckRegion(floorf(drawingbounds.bottomleft.x), floorf(drawingbounds.bottomleft.y),
                                 floorf(drawingbounds.topright.x), floorf(drawingbounds.topright.y), canvas, tileworld.loadedchunks[1 + 0 * 3],
                                 rasterviewprojection * translateMat3({0.0f, chunkheight}));

            absoluteCameraBounds.topright.y = centertilebounds.topright.y;
        }

        drawTileChunckRegion(floorf(absoluteCameraBounds.bottomleft.x), floorf(absoluteCameraBounds.bottomleft.y),
                             floorf(absoluteCameraBounds.topright.x), floorf(absoluteCameraBounds.topright.y), canvas, tileworld.loadedchunks[4], rasterviewprojection);

        if (absoluteCameraBoundsOrigin.bottomleft.x < 0.0f && absoluteCameraBoundsOrigin.bottomleft.y < 0.0f)
        {
            BoundingBox drawingbounds = absoluteCameraBoundsOrigin;
            drawingbounds.topright.x = chunkwidth;
            drawingbounds.topright.y = chunkheight;

            drawingbounds.bottomleft.x = chunkwidth + absoluteCameraBoundsOrigin.bottomleft.x;
            drawingbounds.bottomleft.y = chunkheight + absoluteCameraBoundsOrigin.bottomleft.y;

            drawTileChunckRegion(floorf(drawingbounds.bottomleft.x), floorf(drawingbounds.bottomleft.y),
                                 floorf(drawingbounds.topright.x), floorf(drawingbounds.topright.y), canvas, tileworld.loadedchunks[2 + 2 * 3],
                                 rasterviewprojection * translateMat3({-chunkwidth, -chunkheight}));
        }

        if (absoluteCameraBoundsOrigin.bottomleft.x < 0.0f && absoluteCameraBoundsOrigin.topright.y >= centertilebounds.topright.y)
        {
            BoundingBox drawingbounds = absoluteCameraBoundsOrigin;
            drawingbounds.topright.x = chunkwidth;
            drawingbounds.topright.y -= chunkheight;

            drawingbounds.bottomleft.x = chunkwidth + absoluteCameraBoundsOrigin.bottomleft.x;
            drawingbounds.bottomleft.y = 0.0f;

            drawTileChunckRegion(floorf(drawingbounds.bottomleft.x), floorf(drawingbounds.bottomleft.y),
                                 floorf(drawingbounds.topright.x), floorf(drawingbounds.topright.y), canvas, tileworld.loadedchunks[0 + 0 * 3],
                                 rasterviewprojection * translateMat3({-chunkwidth, chunkheight}));
        }

        if (absoluteCameraBoundsOrigin.topright.x >= centertilebounds.topright.x && absoluteCameraBoundsOrigin.bottomleft.y < 0.0f)
        {
            BoundingBox drawingbounds = absoluteCameraBoundsOrigin;
            drawingbounds.topright.x -= chunkwidth;
            drawingbounds.topright.y = chunkheight;

            drawingbounds.bottomleft.x = 0.0f;
            drawingbounds.bottomleft.y = chunkheight + absoluteCameraBoundsOrigin.bottomleft.y;

            drawTileChunckRegion(floorf(drawingbounds.bottomleft.x), floorf(drawingbounds.bottomleft.y),
                                 floorf(drawingbounds.topright.x), floorf(drawingbounds.topright.y), canvas, tileworld.loadedchunks[2 + 2 * 3],
                                 rasterviewprojection * translateMat3({chunkwidth, -chunkheight}));
        }

        if (absoluteCameraBoundsOrigin.topright.x >= centertilebounds.topright.x && absoluteCameraBoundsOrigin.topright.y >= centertilebounds.topright.y)
        {
            BoundingBox drawingbounds = absoluteCameraBoundsOrigin;
            drawingbounds.topright.x -= chunkwidth;
            drawingbounds.topright.y -= chunkheight;

            drawingbounds.bottomleft.x = 0.0f;
            drawingbounds.bottomleft.y = 0.0f;

            drawTileChunckRegion(floorf(drawingbounds.bottomleft.x), floorf(drawingbounds.bottomleft.y),
                                 floorf(drawingbounds.topright.x), floorf(drawingbounds.topright.y), canvas, tileworld.loadedchunks[2 + 2 * 3],
                                 rasterviewprojection * translateMat3({chunkwidth, chunkheight}));
        }
    }
}