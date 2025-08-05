#ifndef FORGE_ATLAS_H
#define FORGE_ATLAS_H

enum class AtlasType {
    NONE = 0,
    BEST_FIT,
    GRID,
    COUNT,
};

enum class AtlasSubGrid {
    NONE = 0,
    STANDARD,
    DUAL,
    COUNT,
};

enum class AtlasOrientation {
    HORIZONTAL = 0,
    VERTICAL,
    COUNT,
};

enum class AtlasPriority {
    ROW_FIRST = 0,
    COLUMN_FIRST,
    COUNT,
};

#endif // FORGE_ATLAS_H
