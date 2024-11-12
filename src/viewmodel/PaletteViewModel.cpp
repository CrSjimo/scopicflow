//
// Created by Crs_1 on 2024/11/12.
//

#include "PaletteViewModel.h"

namespace sflow {
    PaletteViewModel::PaletteViewModel(QObject *parent) : QObject(parent), m_palette(nullptr) {
    }
    PaletteViewModel::~PaletteViewModel() = default;

    QObject *PaletteViewModel::palette() const {
        return m_palette;
    }
    void PaletteViewModel::setPalette(QObject *palette) {
        if (m_palette != palette) {
            m_palette = palette;
            emit paletteChanged(palette);
        }
    }
} // sflow