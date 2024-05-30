float checkerboard(vec2 pos, float size)
{
    float x = floor(pos.x * size);
    float y = floor(pos.y * size);

    float t = mod(x + y, 2.0);
    float v = max(sign(t), 0.0);

    return v;
}