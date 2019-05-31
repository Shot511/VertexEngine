﻿#pragma once
#include <imgui.h>
#include <string>

namespace Vertex
{
    class Font
    {
    public:
        Font();
        Font(const std::string & filepathname, unsigned size_pixels);
        ~Font() = default;

    private:
        friend class GUI;

        ImFont * m_font;
    };
}