#include "TilemapRenderer.hpp"


TilemapRenderer::TilemapRenderer(const tiles_layer *tl, int x, int y, int w, int h, Tileset *ts)
{
    tileset = ts;
    width = w;
    height = h;
    Camera = Camera::GetInstance();
    for (int i = 0; i < tl->tiles.size(); i++)
    {
        int tile_index = tl->tiles[i];
        int tile_x = i % w;
        int tile_y = i / w;
        int tile_x_pos = x + tile_x * ts->get_tile_width();
        int tile_y_pos = y + tile_y * ts->get_tile_height();
        Tile tile({tile_x_pos, tile_y_pos}, {ts->get_tile_width(), ts->get_tile_height()}, tile_index);
        tiles.push_back(tile);
    }
    vertices = new float[tiles.size() * 4 * 4];
    indices = new unsigned int[tiles.size() * 6];
}

TilemapRenderer::~TilemapRenderer()
{
}

int TilemapRenderer::get_height()
{
    return height;
}

int TilemapRenderer::get_width()
{
    return width;
}

void TilemapRenderer::draw()
{
    float tile_width = 1.0 / tileset->get_tile_width();
    float tile_height = 1.0 / tileset->get_tile_height();
    float map_pos_x = -1.0;
    float map_pos_y = -1.0;
    float camera_pos_x = 0.0;
    float camera_pos_y = 0.0;
    float zoom = *this->Camera->GetZoom();
    unsigned int VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    for(int x = 0 ; x < width ; x++)
    {
        for(int y = 0 ; y < height ; y++)
        {

            int actual_tile_index_x_pos = tiles[x + y * width].tile_index % tileset->get_tile_width();
            int actual_tile_index_y_pos = tiles[x + y * width].tile_index / tileset->get_tile_width();

            vertices[(x + y * width) * 16] = (map_pos_x + x * tile_width - camera_pos_x) * zoom;
            vertices[(x + y * width) * 16 + 1] = (map_pos_y + y * tile_height - camera_pos_y) * zoom;
            vertices[(x + y * width) * 16 + 2] = actual_tile_index_x_pos * tile_width;
            vertices[(x + y * width) * 16 + 3] = actual_tile_index_y_pos * tile_height;

            vertices[(x + y * width) * 16 + 4] = (map_pos_x + x * tile_width - camera_pos_x) * zoom;
            vertices[(x + y * width) * 16 + 5] = (map_pos_y + y * tile_height + tile_height - camera_pos_y) * zoom;
            vertices[(x + y * width) * 16 + 6] = actual_tile_index_x_pos * tile_width;
            vertices[(x + y * width) * 16 + 7] = actual_tile_index_y_pos * tile_height + tile_height;

            vertices[(x + y * width) * 16 + 8] = (map_pos_x + x * tile_width + tile_width - camera_pos_x) * zoom;
            vertices[(x + y * width) * 16 + 9] = (map_pos_y + y * tile_height + tile_height - camera_pos_y) * zoom;
            vertices[(x + y * width) * 16 + 10] = actual_tile_index_x_pos * tile_width + tile_width;
            vertices[(x + y * width) * 16 + 11] = actual_tile_index_y_pos * tile_height + tile_height;

            vertices[(x + y * width) * 16 + 12] = (map_pos_x + x * tile_width + tile_width - camera_pos_x) * zoom;
            vertices[(x + y * width) * 16 + 13] = (map_pos_y + y * tile_height - camera_pos_y) * zoom;
            vertices[(x + y * width) * 16 + 14] = actual_tile_index_x_pos * tile_width + tile_width;
            vertices[(x + y * width) * 16 + 15] = actual_tile_index_y_pos * tile_height;

            indices[(x + y * width) * 6] = x + y * width;
            indices[(x + y * width) * 6 + 1] = x + y * width + 1;
            indices[(x + y * width) * 6 + 2] = x + y * width + width;
            indices[(x + y * width) * 6 + 3] = x + y * width + 1;
            indices[(x + y * width) * 6 + 4] = x + y * width + width + 1;
            indices[(x + y * width) * 6 + 5] = x + y * width + width;
        }
    }
    glBindVertexArray(VAO);


    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(*vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(*indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    GL_TEXTURE_2D, tileset->get_texture()->Bind();
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, sizeof(*indices), GL_UNSIGNED_INT, 0);

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    
    glBindVertexArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

Tileset *TilemapRenderer::get_tileset()
{
    return tileset;
}
