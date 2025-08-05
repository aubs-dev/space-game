// -------------------------------------------
// Includes
// -------------------------------------------

#include "pch.hpp"

#include "forge/component/atlas.hpp"

#define GEM_FORCE_LOGGING
#include "GEM/logger.hpp"
#include "GEM/core/filesystem.hpp"
#include "GEM/core/memory.hpp"
#include "GEM/math/mathf.hpp"
#include "GEM/math/geometry.hpp"
#include "GEM/math/vector.hpp"

// -------------------------------------------
// Constants
// -------------------------------------------

#define CONFIG_MAX_ASSET_FILES     256
#define CONFIG_MAX_SUB_GRID_IMAGES 64

#define CONFIG_ATLAS_MIN_WIDTH  64
#define CONFIG_ATLAS_MIN_HEIGHT 64

#define CONFIG_ATLAS_MAX_WIDTH  8192
#define CONFIG_ATLAS_MAX_HEIGHT 8192

// TODO: CONFIG_COMPONENT_PATH is not validated!
#define CONFIG_COMPONENT_PATH "../../src/forge/component"
// TODO: CONFIG_ASSET_PATH is not validated!
#define CONFIG_ASSET_PATH "../../assets"

#define CONFIG_TEMP_PATH "temp"
#define CONFIG_RESOURCE_PATH "resources"
// TODO: CONFIG_GEN_PATH is not validated!
#define CONFIG_GEN_PATH "../../src/GEM"

#define LOG_PREFIX_WARN ANSI_YELLOW "WARN" ANSI_RESET ": "
#define LOG_PREFIX_ERRO ANSI_RED    "ERRO" ANSI_RESET ": "

// -------------------------------------------
// Data Types
// -------------------------------------------

// -- Utility
enum class StatusCode {
    FAILURE = 0,
    SKIPPED,
    CHANGED,
};

enum class Flags {
    FORCE_GENERATION = 1 << 0, // Forces file generation, regardless of whether we have any changes or not
};

struct Image {
    unsigned char* data;
    i32 width;
    i32 height;
    i32 channels;
};

// -- Asset & Bundle
// TODO: Add animations & shaders as native asset types
enum class AssetType {
    SPRITE = 0,
    TILEMAP,
    PARTICLE,
    FONT,
    SOUND,
    MUSIC,
    ATLAS,
    COUNT,
};

enum class BundleType {
    PRIMARY = 0,
    AUXILIARY,
    COUNT,
};

struct AtlasConfig {
    // Shared
    AtlasType type;

    // Grid
    u32 gridSize;
    u32 activeLength;

    AtlasSubGrid subGridType;
    AtlasOrientation orientation;
    AtlasPriority priority;
};

struct AssetConfig {
    AssetType assetType;

    // Manual
    const char* type;
    const char* prefix;
    const char* fileExt[as_index(BundleType::COUNT)];
    AtlasConfig atlas;

    // Runtime
    char typeUpper[GEM_MAX_STRING_LENGTH];
    char typeCapital[GEM_MAX_STRING_LENGTH];
    char headerPath[MAX_PATH];
    char sourcePath[MAX_PATH];
};

struct Asset {
    char baseName[GEM_MAX_STRING_LENGTH];
    char fileName[GEM_MAX_STRING_LENGTH];

    // Optional
    struct {
        geometry::Rectangle rect;
    } data;
};

struct Bundle {
    const char* path;

    u32 assetCount;
    Asset assets[CONFIG_MAX_ASSET_FILES];

    bool containsChanges;
};

struct PersistentData {
    u32 atlasFileToAssetID[as_index(AssetType::COUNT)];

    struct {
        Vec2i size;
        u32 elementLimit;
        AtlasConfig config;
    } atlas[as_index(AssetType::COUNT)];
};

// -------------------------------------------
// Shared
// -------------------------------------------

StatusCode _internal_status_code = StatusCode::SKIPPED;
u32 _internal_flags = 0;

PersistentData gPersistent = {};

AssetConfig gAssetConfigs[as_index(AssetType::COUNT)] = {
    {
        .type = "sprite",
        .prefix = "SPR_",
        .fileExt = { ".png" },
        .atlas = {
            .type = AtlasType::BEST_FIT,
        },
    },
    {
        .type = "tilemap",
        .prefix = "MAP_",
        .fileExt = { ".png" },
        .atlas = {
            .type = AtlasType::GRID,
            .gridSize = 16,
            .activeLength = 16,
            .subGridType = AtlasSubGrid::DUAL,
            .orientation = AtlasOrientation::HORIZONTAL,
            .priority = AtlasPriority::ROW_FIRST,
        },
    },
    {
        .type = "particle",
        .prefix = "PAR_",
        .fileExt = { ".png" },
        .atlas = {
            .type = AtlasType::GRID,
            .gridSize = 16,
            .activeLength = 1,
            .orientation = AtlasOrientation::HORIZONTAL,
        },
    },
    {
        .type = "font",
        .prefix = "FNT_",
        .fileExt = { ".fnt", ".png" },
        .atlas = {
            .type = AtlasType::BEST_FIT,
        },
    },
    {
        .type = "sound",
        .prefix = "SND_",
        .fileExt = { ".wav" },
    },
    {
        .type = "music",
        .prefix = "MUS_",
        .fileExt = { ".ogg" },
    },
    {
        .type = "atlas",
        .prefix = "ATL_",
        .fileExt = { ".png" },
    },
};

u32 gSubGridConversionLUT[as_index(AtlasSubGrid::COUNT)][CONFIG_MAX_SUB_GRID_IMAGES] = {
    {}, // None
    {}, // Standard
    { 12, 15, 8, 9, 0, 11, 14, 7, 13, 4, 1, 10, 3, 2, 5, 6 }, // Dual
};

const char* gAtlasTypeToStr[as_index(AtlasType::COUNT)] = {
    "AtlasType::NONE",
    "AtlasType::BEST_FIT",
    "AtlasType::GRID",
};

const char* gAtlasSubGridToStr[as_index(AtlasSubGrid::COUNT)] = {
    "AtlasSubGrid::NONE",
    "AtlasSubGrid::STANDARD",
    "AtlasSubGrid::DUAL",
};

const char* gAtlasOrientationToStr[as_index(AtlasOrientation::COUNT)] = {
    "AtlasOrientation::HORIZONTAL",
    "AtlasOrientation::VERTICAL",
};

const char* gAtlasPriorityToStr[as_index(AtlasPriority::COUNT)] = {
    "AtlasPriority::ROW_FIRST",
    "AtlasPriority::COLUMN_FIRST",
};

// -------------------------------------------
// Functions
// -------------------------------------------

// -- Status
StatusCode forge_get_status() {
    return _internal_status_code;
}

void forge_set_status(StatusCode status) {
    _internal_status_code = status;
}

u32 forge_get_flags() {
    return _internal_flags;
}

void forge_set_flags(u32 flags) {
    _internal_flags = flags;
}

bool forge_is_flag_set(Flags flag) {
    return FLAG_GET(_internal_flags, flag);
}

// -- Bundle
bool forge_try_fill_bundle(const AssetConfig* config, Bundle* bundle, u32 bundleIdx, const char* scanPath) {
    if (!config && !bundle && bundleIdx >= as_index(BundleType::COUNT)) return false;
    bundle->path = scanPath;

    // Scan for any asset files of a particular type
    errno = 0;
    DIR* handle = opendir(bundle->path);
    if (!handle || errno == ENOENT) {
        log_format(LOG_PREFIX_WARN "BUNDLE > Could not open directory " ANSI_GREEN "'%s'" ANSI_RESET, bundle->path);
        forge_set_status(StatusCode::FAILURE);
        return false;
    }

    struct dirent* content;
    while ((content = readdir(handle)) != NULL) {
        if (file::has_extension(content->d_name, config->fileExt[bundleIdx])) {
            if (bundle->assetCount < CONFIG_MAX_ASSET_FILES) {
                // Filename
                strcpy(bundle->assets[bundle->assetCount].fileName, content->d_name);
                // Basename
                content->d_name[strlen(content->d_name) - strlen(config->fileExt[bundleIdx])] = '\0';
                strcpy(bundle->assets[bundle->assetCount].baseName, content->d_name);

                // Map the current file to it's respective asset type
                for (u32 i = 0; i < as_index(AssetType::COUNT); i++) {
                    if (strcmp(content->d_name, gAssetConfigs[i].type) == 0) {
                        gPersistent.atlasFileToAssetID[bundle->assetCount] = i;
                    }
                }

                bundle->assetCount++;
            } else {
                log_format(LOG_PREFIX_WARN "BUNDLE > Maximum number of assets has exceeded, expected value < %u got %u", CONFIG_MAX_ASSET_FILES, bundle->assetCount);
                return false;
            }
        }
    }

    closedir(handle);

    // Check if any files were actually found
    if (bundle->assetCount == 0) {
        log_format(LOG_PREFIX_WARN "BUNDLE > No assets of type " ANSI_GREEN "'%s' " ANSI_RESET "were found!", config->fileExt[bundleIdx] + 1);
        return false;
    }

    // Check the manifest for changes
    char manifestPath[GEM_MAX_STRING_LENGTH] = "";
    strcpy(manifestPath, scanPath);
    strcat(manifestPath, "/");
    strcat(manifestPath, "asset-");
    strcat(manifestPath, config->fileExt[bundleIdx] + 1);
    strcat(manifestPath, ".manifest");

    file::File manifestFile = file::open(manifestPath, file::Mode::READ_APPEND);
    bool manifestExists = file::get_size(&manifestFile) != 0;

    if (manifestExists) {
        // Check for file count change (major change)
        char fileCountStr[GEM_MAX_STRING_LENGTH] = "";
        file::read_line(&manifestFile, fileCountStr, GEM_MAX_STRING_LENGTH);
        u32 fileCount = (u32)atoi(fileCountStr);

        // Check for file name/time change (minor change)
        bundle->containsChanges = fileCount != bundle->assetCount;
        if (!bundle->containsChanges) {
            for (u32 i = 0; i < bundle->assetCount; i++) {
                Asset* asset = &bundle->assets[i];

                char filePathStr[MAX_PATH] = "";
                strcpy(filePathStr, scanPath);
                strcat(filePathStr, "/");
                strcat(filePathStr, asset->fileName);

                char lineStr[GEM_MAX_STRING_LENGTH] = "";
                file::read_line(&manifestFile, lineStr, GEM_MAX_STRING_LENGTH);

                const char* delimiter = "|";
                if (strchr(lineStr, delimiter[0]) == NULL) {
                    bundle->containsChanges = true;
                    break;
                }

                // Check the asset's file name
                // NOTE: Name checks aren't strictly necessary, as Windows updates the file modification time on name change.
                //       I'm keeping this here as a "sanity" check.
                char* storedNameStr = strtok(lineStr, delimiter);
                bool hasNameChanged = strcmp(storedNameStr, asset->fileName) != 0;

                // Check the asset's file modification time
                char* storedTimeStr = strtok(NULL, delimiter);
                bool hasTimeChanged = (i64)atoi(storedTimeStr) != file::get_timestamp(filePathStr);

                bundle->containsChanges = hasNameChanged || hasTimeChanged;
                if (bundle->containsChanges) break;
            }
        }
    } else {
        bundle->containsChanges = true;
    }

    file::close(&manifestFile);

    // Rebuild the manifest if changes are present
    if (bundle->containsChanges) {
        manifestFile = file::open(manifestPath, file::Mode::WRITE, true);

        char maxCountStr[GEM_MAX_STRING_LENGTH] = "";
        sprintf(maxCountStr, "%i", CONFIG_MAX_ASSET_FILES);
        u64 maxNumLen = strlen(maxCountStr);

        for (u32 i = 0; i < bundle->assetCount; i++) {
            Asset* asset = &bundle->assets[i];

            char filePathStr[MAX_PATH] = "";
            strcpy(filePathStr, scanPath);
            strcat(filePathStr, "/");
            strcat(filePathStr, asset->fileName);

            // File Count Placeholder
            char numStr[GEM_MAX_STRING_LENGTH] = "";
            if (i == 0) {
                for (u64 i = 0; i < maxNumLen; i++) {
                    numStr[i] = ' ';
                }
                numStr[maxNumLen] = '\0';

                file::write_line(&manifestFile, numStr);
            }

            // Name & Timestamp
            char dataStr[GEM_MAX_STRING_LENGTH] = "";
            strcpy(dataStr, asset->fileName);
            strcat(dataStr, "|");
            sprintf(numStr, "%lli", file::get_timestamp(filePathStr));
            strcat(dataStr, numStr);
            file::write_line(&manifestFile, dataStr);
        }

        char fileCountStr[GEM_MAX_STRING_LENGTH] = "";
        sprintf(fileCountStr, "%u", bundle->assetCount);
        file::set_offset_start(&manifestFile);
        file::write(&manifestFile, fileCountStr, (i32)strlen(fileCountStr));

        log_format("- Updated manifest for type " ANSI_GREEN "'%s'" ANSI_RESET, config->fileExt[bundleIdx] + 1);
        file::close(&manifestFile);
        forge_set_status(StatusCode::CHANGED);
    } else {
        if (forge_is_flag_set(Flags::FORCE_GENERATION)) {
            log_format("- Files of type " ANSI_GREEN "'%s'" ANSI_RESET " have changed | " ANSI_MAGENTA "Forced" ANSI_RESET, config->fileExt[bundleIdx] + 1);
            forge_set_status(StatusCode::CHANGED);
        } else {
            log_format("- Files of type " ANSI_GREEN "'%s'" ANSI_RESET " haven't changed | " ANSI_YELLOW "Skipped" ANSI_RESET, config->fileExt[bundleIdx] + 1);
            forge_set_status(StatusCode::SKIPPED);
        }
    }

    return true;
}

// -- Atlas
bool forge_atlas_try_stb_pack(Vec2i atlasSize, stbrp_rect* rects, u32 rectCount) {
    if (atlasSize.w < CONFIG_ATLAS_MIN_WIDTH && atlasSize.h < CONFIG_ATLAS_MIN_HEIGHT && !rects && rectCount == 0) return false;

    stbrp_context context = {};
    i32 nodeCount = atlasSize.w;
    stbrp_node* nodes = (stbrp_node*)memory::alloc(sizeof(stbrp_node) * nodeCount);

    stbrp_init_target(&context, atlasSize.w, atlasSize.h, nodes, nodeCount);
    stbrp_pack_rects(&context, rects, rectCount);

    bool success = true;
    for (u32 i = 0; i < rectCount; i++) {
        if (!rects[i].was_packed) {
            success = false;
        }
    }

    memory::free(nodes);
    return success;
}

bool forge_generate_atlas(const AssetConfig* config, Bundle* bundle, u32 bundleIdx) {
    if (!config && !bundle && bundleIdx >= as_index(BundleType::COUNT)) return false;

    Image* images = (Image*)memory::alloc(sizeof(Image) * bundle->assetCount);
    stbrp_rect* rects = (stbrp_rect*)memory::alloc(sizeof(stbrp_rect) * bundle->assetCount);

    if (config->atlas.type == AtlasType::COUNT) {
        log_format(LOG_PREFIX_WARN "ATLAS > Cannot set type to its count!");
        goto exit_generate_atlas;
    }

    // Get images & rect data from bundle
    for (u32 i = 0; i < bundle->assetCount; i++) {
        Asset* asset = &bundle->assets[i];
        Image* assetImg = &images[i];

        char filePathStr[MAX_PATH] = "";
        strcpy(filePathStr, bundle->path);
        strcat(filePathStr, "/");
        strcat(filePathStr, asset->fileName);

        assetImg->data = stbi_load(filePathStr, &assetImg->width, &assetImg->height, &assetImg->channels, 4);
        if (assetImg->data) {
            rects[i].id = i;
            rects[i].w = assetImg->width;
            rects[i].h = assetImg->height;
        } else {
            log_format(LOG_PREFIX_WARN "ATLAS > Failed to load image " ANSI_GREEN "'%s'" ANSI_RESET, filePathStr);
            goto exit_generate_atlas;
        }
    }

    char atlasPathStr[GEM_MAX_STRING_LENGTH] = CONFIG_RESOURCE_PATH;
    strcat(atlasPathStr, "/atlas/");
    strcat(atlasPathStr, config->type);
    strcat(atlasPathStr, ".png");

    Vec2i atlasSize = Vec2i();
    unsigned char* atlasImgData = NULL;

    switch (config->atlas.type) {
        using enum AtlasType;
        case BEST_FIT:
            {
                atlasSize = { CONFIG_ATLAS_MIN_WIDTH, CONFIG_ATLAS_MIN_HEIGHT };

                for (;;) {
                    // Reset the state
                    for (u32 i = 0; i < bundle->assetCount; i++) {
                        rects[i].was_packed = 0;
                    }

                    if (forge_atlas_try_stb_pack(atlasSize, rects, bundle->assetCount)) break;

                    // If the pack failed, increase the size of the atlas. Width first, then height keeping it "square".
                    if ((atlasSize.w * 2 <= CONFIG_ATLAS_MAX_WIDTH) && (atlasSize.w <= atlasSize.h || atlasSize.h * 2 > CONFIG_ATLAS_MAX_HEIGHT)) {
                        atlasSize.w *= 2;
                    } else if (atlasSize.h * 2 <= CONFIG_ATLAS_MAX_HEIGHT) {
                        atlasSize.h *= 2;
                    } else {
                        atlasSize.h *= 2;
                    }

                    // If the current dimensions exceed the maximum atlas size, stop packing!
                    if (atlasSize.w > CONFIG_ATLAS_MAX_WIDTH || atlasSize.h > CONFIG_ATLAS_MAX_HEIGHT) {
                        log_format(LOG_PREFIX_WARN "ATLAS > Images cannot be packed into a reasonable atlas size!");
                        goto exit_generate_atlas;
                    }
                }

                // Copy over the pixels from each image into the atlas
                atlasImgData = (unsigned char*)memory::alloc(atlasSize.w * atlasSize.h * 4);

                for (u32 i = 0; i < bundle->assetCount; i++) {
                    if (rects[i].was_packed) {
                        Asset* asset = &bundle->assets[i];
                        Image* assetImg = &images[i];

                        asset->data.rect.x      = rects[i].x;
                        asset->data.rect.y      = rects[i].y;
                        asset->data.rect.width  = rects[i].w;
                        asset->data.rect.height = rects[i].h;

                        for (i32 y = 0; y < assetImg->height; y++) {
                            for (i32 x = 0; x < assetImg->width; x++) {
                                i32 genPixel = ((rects[i].y + y) * atlasSize.w) + (rects[i].x + x);
                                genPixel *= 4;

                                i32 assetPixel = (y * assetImg->width) + x;
                                assetPixel *= 4;

                                atlasImgData[genPixel + 0] = assetImg->data[assetPixel + 0];
                                atlasImgData[genPixel + 1] = assetImg->data[assetPixel + 1];
                                atlasImgData[genPixel + 2] = assetImg->data[assetPixel + 2];
                                atlasImgData[genPixel + 3] = assetImg->data[assetPixel + 3];
                            }
                        }
                    }
                }
            }
            break;
        case GRID:
            {
                // Validate the config
                if (config->atlas.gridSize == 0) {
                    log_format(LOG_PREFIX_WARN "ATLAS > No gridSize was set in atlas config!");
                    goto exit_generate_atlas;
                }

                if (config->atlas.activeLength == 0) {
                    log_format(LOG_PREFIX_WARN "ATLAS > No activeLength was set in atlas config!");
                    goto exit_generate_atlas;
                }

                if (config->atlas.subGridType == AtlasSubGrid::COUNT) {
                    log_format(LOG_PREFIX_WARN "ATLAS > Cannot set subGridType to its count!");
                    goto exit_generate_atlas;
                }

                if (config->atlas.orientation == AtlasOrientation::COUNT) {
                    log_format(LOG_PREFIX_WARN "ATLAS > Cannot set orientation to its count!");
                    goto exit_generate_atlas;
                }

                if (config->atlas.priority == AtlasPriority::COUNT) {
                    log_format(LOG_PREFIX_WARN "ATLAS > Cannot set priority to its count!");
                    goto exit_generate_atlas;
                }

                // Validate image sizes
                u32 totalImageCount = 0;
                for (u32 i = 0; i < bundle->assetCount; i++) {
                    Image* assetImg = &images[i];

                    u32 imageCount = (assetImg->width / config->atlas.gridSize) * (assetImg->height / config->atlas.gridSize);
                    totalImageCount += imageCount;

                    if (config->atlas.subGridType == AtlasSubGrid::NONE) {
                        if (assetImg->width != (i32)config->atlas.gridSize || assetImg->height != (i32)config->atlas.gridSize) {
                            log_format(LOG_PREFIX_WARN "ATLAS > Images must match grid size!");
                            goto exit_generate_atlas;
                        }
                    } else {
                        if (assetImg->width < (i32)config->atlas.gridSize || assetImg->height < (i32)config->atlas.gridSize) {
                            log_format(LOG_PREFIX_WARN "ATLAS > Grid size must be less than or equal to the sub-grid's image size!");
                            goto exit_generate_atlas;
                        }

                        if (imageCount > CONFIG_MAX_SUB_GRID_IMAGES) {
                            log_format(LOG_PREFIX_WARN "ATLAS > Too many images in sub-grid, expected value < %u got %u", CONFIG_MAX_SUB_GRID_IMAGES, imageCount);
                            goto exit_generate_atlas;
                        }

                        if (config->atlas.subGridType == AtlasSubGrid::DUAL && imageCount != 16) {
                            log_format(LOG_PREFIX_WARN "ATLAS > Invalid image count in sub-grid, expected 16 (Dual Grid) got %u", imageCount);
                            goto exit_generate_atlas;
                        }
                    }
                }

                // Prepare the atlas shape
                if (config->atlas.subGridType == AtlasSubGrid::NONE) {
                    u32 columnCount = mathf::ceil_to_int(bundle->assetCount / (f32)config->atlas.activeLength);
                    atlasSize = Vec2i(columnCount * config->atlas.gridSize, config->atlas.activeLength * config->atlas.gridSize);
                } else {
                    u32 columnCount = mathf::ceil_to_int(totalImageCount / (f32)config->atlas.activeLength);
                    atlasSize = Vec2i(config->atlas.activeLength * config->atlas.gridSize, columnCount * config->atlas.gridSize);
                }

                if (config->atlas.orientation == AtlasOrientation::VERTICAL) {
                    i32 temp = atlasSize.w;
                    atlasSize.w = atlasSize.h;
                    atlasSize.h = temp;
                }

                atlasImgData = (unsigned char*)memory::alloc(atlasSize.w * atlasSize.h * 4);

                // Copy over the pixels from each image into the atlas
                u32 subImagesPassed = 0;
                for (u32 i = 0; i < bundle->assetCount; i++) {
                    Image* assetImg = &images[i];
                    u32 subImageCount = (assetImg->width / config->atlas.gridSize) * (assetImg->height / config->atlas.gridSize);

                    u32 columnCount = atlasSize.w / config->atlas.gridSize;
                    u32 rowCount    = atlasSize.h / config->atlas.gridSize;
                    u32 row = 0, col = 0;

                    for (u32 subIdx = 0; subIdx < subImageCount; subIdx++) {
                        if (config->atlas.priority == AtlasPriority::COLUMN_FIRST) {
                            col = (subImagesPassed + subIdx) / rowCount;
                            row = (subImagesPassed + subIdx) % rowCount;
                        } else {
                            col = (subImagesPassed + subIdx) % columnCount;
                            row = (subImagesPassed + subIdx) / columnCount;
                        }

                        u32 subColumnCount = assetImg->width / config->atlas.gridSize;
                        u32 convertedSubIdx = (config->atlas.subGridType == AtlasSubGrid::STANDARD) ? subIdx : gSubGridConversionLUT[as_index(config->atlas.subGridType)][subIdx];
                        u32 subCol = convertedSubIdx / subColumnCount;
                        u32 subRow = convertedSubIdx % subColumnCount;

                        for (u32 y = 0; y < config->atlas.gridSize; y++) {
                            for (u32 x = 0; x < config->atlas.gridSize; x++) {
                                u32 genPixel = (y * atlasSize.w) + (row * config->atlas.gridSize * atlasSize.w) + (col * config->atlas.gridSize) + x;
                                genPixel *= 4;

                                u32 assetPixel = (y * assetImg->width) + (subRow * config->atlas.gridSize) + (subCol * config->atlas.gridSize * assetImg->width) + x;
                                assetPixel *= 4;

                                atlasImgData[genPixel + 0] = assetImg->data[assetPixel + 0];
                                atlasImgData[genPixel + 1] = assetImg->data[assetPixel + 1];
                                atlasImgData[genPixel + 2] = assetImg->data[assetPixel + 2];
                                atlasImgData[genPixel + 3] = assetImg->data[assetPixel + 3];
                            }
                        }
                    }

                    subImagesPassed += subImageCount;
                }
            }
            break;
        default:
            {
                log_format(LOG_PREFIX_WARN "ATLAS > Generator not implemented!");
                goto exit_generate_atlas;
            }
            break;
    }

    // -- SUCCESS --
    // Write out the atlas image
    bool success = false;
    if (atlasImgData && atlasSize.w > 0 && atlasSize.h > 0) {
        i32 stride = atlasSize.w * 4;
        stbi_write_png(atlasPathStr, atlasSize.w, atlasSize.h, 4, atlasImgData, stride);

        u32 typeIdx = as_index(config->assetType);
        gPersistent.atlas[typeIdx].config = config->atlas;
        gPersistent.atlas[typeIdx].size = atlasSize;
        gPersistent.atlas[typeIdx].elementLimit = bundle->assetCount;

        log_format("- Generated atlas: " ANSI_GREEN "'%s'" ANSI_RESET "", atlasPathStr);
        success = true;
    } else {
        log_format(LOG_PREFIX_WARN "ATLAS > Image data was not set correctly!");
    }

    // -- FAILURE --
exit_generate_atlas:
    if (images) {
        for (u32 i = 0; i < bundle->assetCount; i++) {
            unsigned char* data = images[i].data;
            if (data) stbi_image_free(data);
        }

        memory::free(images);
    }

    if (rects) memory::free(rects);
    if (atlasImgData) memory::free(atlasImgData);

    // -- END --
    return success;
}

// -- Generation
// NOTE: Could be better, c-strings are a pain
void forge_format_string_as_enum(char* buffer, u64 bufferSize, const char* name, bool appendZero) {
    u64 offset = strlen(buffer);
    u64 nameLen = strlen(name);

    // Skip if name exceededs buffer size | includes space for " = 0," if needed
    if (offset + nameLen > bufferSize - 6) return;

    // Loop through every character in the unformatted name and convert each character as necessary
    // TODO: Why is curVal & prevVal an unsigned int? Shouldn't it be an unsigned char?
    u32 curVal = 0, prevVal = 0;
    for (u32 cIdx = 0; cIdx < nameLen; cIdx++) {
        prevVal = curVal;
        curVal = (u32)name[cIdx];

        // Check if the current character is a 'Space', 'Hyphen-minus' or 'Underscore'
        if (curVal == 32 || curVal == 45 || curVal == 95) {
            // Check if an underscore was already set
            if (buffer[cIdx + offset - 1] == '_') {
                offset--;
            } else {
                buffer[cIdx + offset] = '_';
            }

            continue;
        }

        // Check if the current character is a 'Latin Small Letter'
        if (curVal >= 97 && curVal <= 122) {
            // Convert the current lower case letter into a capital one
            buffer[cIdx + offset] = (char)(curVal - 32);
            continue;
        }

        // Check if the current character is a 'Latin Capital Letter'
        if (curVal >= 65 && curVal <= 90) {
            // Keep the first capital letter the same
            if (cIdx == 0) {
                buffer[cIdx + offset] = (char)curVal;
            } else {
                // Check if the previous character was a 'Latn Capital Letter'
                bool isPrevCapital = prevVal >= 65 && prevVal <= 90;
                if (!isPrevCapital && buffer[cIdx + offset - 1] != '_') {
                    buffer[cIdx + offset] = '_';
                    offset++;
                }

                buffer[cIdx + offset] = (char)curVal;
            }

            continue;
        }

        // Check if the current character is a 'Digit'
        if (curVal >= 48 && curVal <= 57) { 
            buffer[cIdx + offset] = (char)curVal;
            continue;
        }

        // Ignore characters that aren't processed
        offset--;
    }

    // Append the enum suffix
    if (appendZero) {
        strcat(buffer, " = 0");
        offset += 4;
    }

    buffer[offset + nameLen] = ',';
}

// TODO: Generated asset files should have a manifest as well, so if any changes are detected they're rebuilt (prevents accidental changes)
bool forge_write_asset_file(AssetType assetType, const Bundle* bundles) {
    if (assetType == AssetType::COUNT && !bundles) return false;
    AssetConfig* config = &gAssetConfigs[as_index(assetType)];

    // Initialize some reusable data
    const Bundle* primaryBundle = &bundles[as_index(BundleType::PRIMARY)];
    char tempStr[GEM_MAX_STRING_LENGTH] = "";

    // -- [ ASSET HEADER ] --
    sprintf(config->headerPath, CONFIG_TEMP_PATH "/%s_header.tmp", config->type);
    file::File headerFile = file::open(config->headerPath, file::Mode::WRITE);

    sprintf(tempStr, "// %s", config->typeCapital);
    file::write_line(&headerFile, "// -------------------------------------------");
    file::write_line(&headerFile, tempStr);
    file::write_line(&headerFile, "// -------------------------------------------");
    file::write_line(&headerFile, "");

    // -- Asset Structure
    bool assetHasStructure = true;
    if (assetType == AssetType::TILEMAP || assetType == AssetType::PARTICLE) {
        assetHasStructure = false;
        goto skip_asset_structure;
    }

    if (assetType == AssetType::FONT) {
        // Unicode Block
        file::write_line(&headerFile, "enum class UnicodeBlock {");
        file::write_line(&headerFile, "    LATIN = 128,");
        file::write_line(&headerFile, "    LATIN1_SUPPLEMENT = 256,");
        file::write_line(&headerFile, "};");
        file::write_line(&headerFile, "");

        // Glyph
        file::write_line(&headerFile, "struct Glyph {");
        file::write_line(&headerFile, "    u16 x;");        // (2 bytes) Horizontal position of the glyph in the texture (0 is the leftmost point in the texture)
        file::write_line(&headerFile, "    u16 y;");        // (2 bytes) Vertical position of the glyph in the texture (0 is the topmost point in the texture)
        file::write_line(&headerFile, "    u16 width;");    // (2 bytes) Width of the glyph in the texture
        file::write_line(&headerFile, "    u16 height;");   // (2 bytes) Height of the glyph in the texture
        file::write_line(&headerFile, "    i16 offsetX;");  // (2 bytes) Horizontal offset when drawing the glyph to the screen
        file::write_line(&headerFile, "    i16 offsetY;");  // (2 bytes) Vertical offset when drawing the glyph to the screen
        file::write_line(&headerFile, "    i16 advanceX;"); // (2 bytes) Number of pixels to advance for when drawing the next glyph
        file::write_line(&headerFile, "    u16 isValid;");  // (2 bytes) Boolean representing if the glyph was initialized with correct values
        file::write_line(&headerFile, "};");        // total: 16 bytes
        file::write_line(&headerFile, "");
    }

    if (assetType == AssetType::ATLAS) {
        file::File atlasFile = file::open(CONFIG_COMPONENT_PATH "/atlas.hpp", file::Mode::READ);

        i32 size = file::get_size(&atlasFile);
        char* data = (char*)memory::alloc(size);

        i32 bytesRead = 0;
        file::read(&atlasFile, data, size, &bytesRead);

        i32 offset = file::get_offset(&headerFile);
        file::set_offset_end(&headerFile);
        file::write(&headerFile, data, bytesRead);
        file::set_offset(&headerFile, offset, file::Offset::SET);

        memory::free(data);
    }

    sprintf(tempStr, "struct %s {", config->typeCapital);
    file::write_line(&headerFile, tempStr);

    switch (assetType) {
        using enum AssetType;
        case SPRITE:
            {
                file::write_line(&headerFile, "    geometry::Rectangle atlasRect;");
            }
            break;
        case TILEMAP:
        case PARTICLE:
            break;
        case FONT:
            {
                file::write_line(&headerFile, "    const char* filePath;");
                file::write_line(&headerFile, "    geometry::Rectangle atlasRect;");
                file::write_line(&headerFile, "    // Runtime");
                file::write_line(&headerFile, "    u16 lineHeight;");
                file::write_line(&headerFile, "    u16 baseLine;");
                file::write_line(&headerFile, "    u8 paddingUp;");
                file::write_line(&headerFile, "    u8 paddingRight;");
                file::write_line(&headerFile, "    u8 paddingDown;");
                file::write_line(&headerFile, "    u8 paddingLeft;");
                file::write_line(&headerFile, "    u32 glyphCount;");
                file::write_line(&headerFile, "    i32 pad[1];");
                file::write_line(&headerFile, "    Glyph glyphBank[(u32)UnicodeBlock::LATIN1_SUPPLEMENT];");
            }
            break;
        case SOUND:
        case MUSIC:
            {
                file::write_line(&headerFile, "    const char* filePath;");
            }
            break;
        case ATLAS:
            {
                file::write_line(&headerFile, "    const char* filePath;");
                file::write_line(&headerFile, "    AtlasType type;");
                file::write_line(&headerFile, "    Vec2i size;");
                file::write_line(&headerFile, "    // -- Best Fit");
                file::write_line(&headerFile, "    u32 elementLimit;");
                file::write_line(&headerFile, "    // -- Grid");
                file::write_line(&headerFile, "    u32 gridSize;");
                file::write_line(&headerFile, "    AtlasOrientation orientation;");
                file::write_line(&headerFile, "    AtlasPriority priority;");
                file::write_line(&headerFile, "    AtlasSubGrid subGridType;");
            }
            break;
        default:
            {
                log_format(LOG_PREFIX_WARN "GEN > Structure fields for asset type '%s' aren't defined", config->type);
                file::write_line(&headerFile, "    // ...");
            }
            break;
    }

    file::write_line(&headerFile, "};");
    file::write_line(&headerFile, "");

skip_asset_structure:
    // -- Asset Enum
    char enumCountStr[GEM_MAX_STRING_LENGTH] = "";
    sprintf(enumCountStr, "ASSET_%s_COUNT", config->typeUpper);

    sprintf(tempStr, "enum %sName {", config->typeCapital);
    file::write_line(&headerFile, tempStr);

    // Format and write all the file names as enum values
    sprintf(tempStr, "    %sNONE = -1,", config->prefix);
    file::write_line(&headerFile, tempStr);

    for (u32 i = 0; i < primaryBundle->assetCount; i++) {
        const Asset* asset = &primaryBundle->assets[i];

        char enumValStr[GEM_MAX_STRING_LENGTH] = "";
        sprintf(enumValStr, "    %s", config->prefix);
        forge_format_string_as_enum(enumValStr, GEM_MAX_STRING_LENGTH, asset->baseName, i == 0);

        file::write_line(&headerFile, enumValStr);
    }

    // Write enum count value | example: "    TEST_COUNT,"
    sprintf(tempStr, "    %s,", enumCountStr);
    file::write_line(&headerFile, tempStr);
    file::write_line(&headerFile, "};");

    // -- Forward declare the asset bank
    if (assetHasStructure) {
        sprintf(tempStr, "extern %s g%sBank[%s];", config->typeCapital, config->typeCapital, enumCountStr);
        file::write_line(&headerFile, "");
        file::write_line(&headerFile, tempStr);
    }

    log_format("- Generated file: " ANSI_GREEN "'%s'" ANSI_RESET, config->headerPath);
    file::close(&headerFile);

    // -- [ ASSET SOURCE ] --
    if (assetType == AssetType::TILEMAP || assetType == AssetType::PARTICLE) goto skip_asset_source;

    sprintf(config->sourcePath, CONFIG_TEMP_PATH "/%s_source.tmp", config->type);
    file::File sourceFile = file::open(config->sourcePath, file::Mode::WRITE);

    sprintf(tempStr, "// %s", config->typeCapital);
    file::write_line(&sourceFile, "// -------------------------------------------");
    file::write_line(&sourceFile, tempStr);
    file::write_line(&sourceFile, "// -------------------------------------------");
    file::write_line(&sourceFile, "");

    // -- Asset Bank Array
    sprintf(tempStr, "%s g%sBank[%s] = {", config->typeCapital, config->typeCapital, enumCountStr);
    file::write_line(&sourceFile, tempStr);

    for (u32 i = 0; i < primaryBundle->assetCount; i++) {
        const Asset* asset = &primaryBundle->assets[i];

        const Bundle* auxiliaryBundle = &bundles[as_index(BundleType::AUXILIARY)];
        const Asset* auxiliaryAsset = &auxiliaryBundle->assets[i];

        char numBuf[GEM_MAX_STRING_LENGTH] = "";
        memory::zero(tempStr, GEM_MAX_STRING_LENGTH);
        strcpy(tempStr, "    {");

        if (assetType == AssetType::FONT || assetType == AssetType::SOUND || assetType == AssetType::MUSIC || assetType == AssetType::ATLAS) {
            strcat(tempStr, " .filePath = \"");
            strcat(tempStr, CONFIG_RESOURCE_PATH);
            strcat(tempStr, "/");
            strcat(tempStr, config->type);
            strcat(tempStr, "/");
            strcat(tempStr, asset->fileName);
            strcat(tempStr, "\"");
        }

        if (assetType == AssetType::FONT || assetType == AssetType::ATLAS) {
            strcat(tempStr, ",");
        }

        if (assetType == AssetType::SPRITE || assetType == AssetType::FONT) {
            const geometry::Rectangle* rect = (assetType == AssetType::SPRITE) ? &asset->data.rect : &auxiliaryAsset->data.rect;

            strcat(tempStr, " .atlasRect = { ");
            sprintf(numBuf, "%i, ", rect->x);
            strcat(tempStr, numBuf);
            sprintf(numBuf, "%i, ", rect->y);
            strcat(tempStr, numBuf);
            sprintf(numBuf, "%i, ", rect->width);
            strcat(tempStr, numBuf);
            sprintf(numBuf, "%i", rect->height);
            strcat(tempStr, numBuf);
            strcat(tempStr, " }");
        }

        if (assetType == AssetType::ATLAS) {
            u32 assetID = gPersistent.atlasFileToAssetID[i];
            AtlasConfig* atlasConfig = &gPersistent.atlas[assetID].config;

            strcat(tempStr, " .type = ");
            strcat(tempStr, gAtlasTypeToStr[as_index(atlasConfig->type)]);

            strcat(tempStr, ",");
            strcat(tempStr, " .size = { ");
            sprintf(numBuf, "%i, %i", gPersistent.atlas[assetID].size.w, gPersistent.atlas[assetID].size.h);
            strcat(tempStr, numBuf);
            strcat(tempStr, " }");

            if (atlasConfig->type == AtlasType::BEST_FIT) {
                strcat(tempStr, ",");
                strcat(tempStr, " .elementLimit = ");
                sprintf(numBuf, "%i", gPersistent.atlas[assetID].elementLimit);
                strcat(tempStr, numBuf);
            }

            if (atlasConfig->type == AtlasType::GRID) {
                strcat(tempStr, ",");
                strcat(tempStr, " .gridSize = ");
                sprintf(numBuf, "%i", atlasConfig->gridSize);
                strcat(tempStr, numBuf);

                // Write enum values if they aren't the default
                if (as_index(atlasConfig->subGridType) != 0) {
                    strcat(tempStr, ",");
                    strcat(tempStr, " .subGridType = ");
                    strcat(tempStr, gAtlasSubGridToStr[as_index(atlasConfig->subGridType)]);
                }

                if (as_index(atlasConfig->orientation) != 0) {
                    strcat(tempStr, ",");
                    strcat(tempStr, " .orientation = ");
                    strcat(tempStr, gAtlasOrientationToStr[as_index(atlasConfig->orientation)]);
                }

                if (as_index(atlasConfig->priority) != 0) {
                    strcat(tempStr, ",");
                    strcat(tempStr, " .priority = ");
                    strcat(tempStr, gAtlasPriorityToStr[as_index(atlasConfig->priority)]);
                }
            }
        }

        strcat(tempStr, " },");
        file::write_line(&sourceFile, tempStr);
    }

    file::write_line(&sourceFile, "};");

    file::close(&sourceFile);
    log_format("- Generated file: " ANSI_GREEN "'%s'" ANSI_RESET, config->sourcePath);

skip_asset_source:
    return true;
}

void forge_generate_asset_part(AssetType assetType) {
    if (assetType == AssetType::COUNT) return;
    AssetConfig* config = &gAssetConfigs[as_index(assetType)];
    config->assetType = assetType;

    // Validate config
    bool validConfig = string_is_valid(config->type) || string_is_valid(config->prefix) || string_is_valid(config->fileExt[0]);
    if (!validConfig) {
        log_format(LOG_PREFIX_WARN "ASSET > Invalid config provided for type: %i", (i32)assetType);
        forge_set_status(StatusCode::FAILURE);
        return;
    }

    // Set alternate string formatting
    // -- Type Upper
    u64 typeStrLen = strlen(config->type);
    for (u64 i = 0; i < typeStrLen; i++) {
        u8 val = (u8)config->type[i];
        if (val >= 97 && val <= 122) config->typeUpper[i] = (char)(val - 32);
    }

    // -- Type Capital
    strcpy(config->typeCapital, config->type);
    config->typeCapital[0] = (char)((u8)config->typeCapital[0] - 32);

    // Collect & bundle all necessary data from an asset directory
    char scanPath[MAX_PATH] = "";
    if (assetType == AssetType::ATLAS) {
        strcpy(scanPath, CONFIG_RESOURCE_PATH);
    } else {
        strcpy(scanPath, CONFIG_ASSET_PATH);
    }

    strcat(scanPath, "/");
    strcat(scanPath, config->type);
    log_format(ANSI_CYAN "[%s] " ANSI_RESET "Scanning files in: %s", config->typeUpper, scanPath);

    Bundle* bundles = (Bundle*)memory::alloc(sizeof(Bundle) * (u32)BundleType::COUNT);

    for (u32 bundleIdx = 0; bundleIdx < as_index(BundleType::COUNT); bundleIdx++) {
        if (!string_is_valid(config->fileExt[bundleIdx])) continue;
        Bundle* bundle = &bundles[bundleIdx];

        if (!forge_try_fill_bundle(config, bundle, bundleIdx, scanPath)) {
            log_format(LOG_PREFIX_WARN "ASSET > Failed to fill bundle!");
            forge_set_status(StatusCode::FAILURE);
            break;
        }

        if (forge_get_status() == StatusCode::SKIPPED) continue;

        // Generate composite assets from existing asset files
        // -- Atlas
        if (config->atlas.type != AtlasType::NONE) {
            if (assetType == AssetType::ATLAS) {
                log_format(LOG_PREFIX_WARN "ASSET > Atlas types cannot generate texture atlases!");
                forge_set_status(StatusCode::FAILURE);
                break;
            } else {
                if (strcmp(config->fileExt[bundleIdx], ".png") == 0) {
                    if (!forge_generate_atlas(config, bundle, bundleIdx)) {
                        log_format(LOG_PREFIX_WARN "ASSET > Failed to generate atlas!");
                        forge_set_status(StatusCode::FAILURE);
                        break;
                    }
                }
            }
        }
    }

    // Write out the data
    if (forge_get_status() == StatusCode::CHANGED) {
        if (!forge_write_asset_file(assetType, bundles)) {
            log_format(LOG_PREFIX_WARN "ASSET > Failed to generate asset files!");
            forge_set_status(StatusCode::FAILURE);
        }
    }

    memory::free(bundles);
}

void forge_write_generated_header(const file::File* file) {
    file::write_line(file, "// ------------------------------------------------------");
    file::write_line(file, "// WARNING: This file is generated by the Asset Forge!");
    file::write_line(file, "//          Any modifications will be overridden.");
    file::write_line(file, "// ------------------------------------------------------");
    file::write_line(file, "");
}

void forge_combine_asset_files() {
    log_format(ANSI_CYAN "[FORGE] " ANSI_RESET "Combining asset files!");

    // -- File Headers
    const char* headerPath = CONFIG_GEN_PATH "/assets.hpp";
    file::File headerFile = file::open(headerPath, file::Mode::WRITE);

    forge_write_generated_header(&headerFile);
    file::write_line(&headerFile, "#pragma once");
    file::write_line(&headerFile, "");
    file::write_line(&headerFile, "#include \"pch.hpp\"");
    file::write_line(&headerFile, "");
    file::write_line(&headerFile, "#include \"GEM/math/geometry.hpp\"");
    file::write_line(&headerFile, "#include \"GEM/math/vector.hpp\"");
    file::write_line(&headerFile, "");
    file::write_line(&headerFile, "namespace asset {");

    const char* sourcePath = CONFIG_GEN_PATH "/assets.cpp";
    file::File sourceFile = file::open(sourcePath, file::Mode::WRITE);

    forge_write_generated_header(&sourceFile);
    file::write_line(&sourceFile, "#include \"GEM/assets.hpp\"");
    file::write_line(&sourceFile, "");
    file::write_line(&sourceFile, "namespace asset {");

    // -- Copy over the partial asset files
    for (u32 i = 0; i < as_index(AssetType::COUNT); i++) {
        AssetConfig* config = &gAssetConfigs[i];

        // Include the header
        if (string_is_valid(config->headerPath)) {
            file::File headerPartFile = file::open(config->headerPath, file::Mode::READ);

            i32 size = file::get_size(&headerPartFile);
            i32 bytesRead = 0;
            char* data = (char*)memory::alloc(size);

            file::read(&headerPartFile, data, size, &bytesRead);
            file::write(&headerFile, data, bytesRead);
            if (i != as_index(AssetType::COUNT) - 1) file::write_line(&headerFile, "");

            file::close(&headerPartFile);
        }

        // Include the source (if available)
        if (string_is_valid(config->sourcePath)) {
            file::File sourcePartFile = file::open(config->sourcePath, file::Mode::READ);

            i32 size = file::get_size(&sourcePartFile);
            i32 bytesRead = 0;
            char* data = (char*)memory::alloc(size);

            file::read(&sourcePartFile, data, size, &bytesRead);
            file::write(&sourceFile, data, bytesRead);
            if (i != as_index(AssetType::COUNT) - 1) file::write_line(&sourceFile, "");

            file::close(&sourcePartFile);
        }
    }

    // -- File Footers
    file::write_line(&headerFile, "}; // namespace: asset");
    file::close(&headerFile);
    log_format("- Generated file: " ANSI_GREEN "'%s'" ANSI_RESET, headerPath);

    file::write_line(&sourceFile, "}; // namespace: asset");
    file::close(&sourceFile);
    log_format("- Generated file: " ANSI_GREEN "'%s'" ANSI_RESET, sourcePath);
}

// -------------------------------------------
// Asset Forge Entry
// -------------------------------------------

i32 main(int argc, char* argv[]) {
    // Enable flags if provided
    if (argc > 1) {
        u32 flags = 0;
        for (i32 i = 0; i < argc; i++) {
            if (strcmp(argv[i], "--force") == 0) {
                FLAG_ADD(flags, Flags::FORCE_GENERATION);
            }
        }

        forge_set_flags(flags);
    }

    // Generate partial asset files and their resources
    errno = 0;
    DIR* handle = opendir(CONFIG_TEMP_PATH);
    if (!handle || errno == ENOENT) {
        log_format(LOG_PREFIX_ERRO "FORGE > Could not open directory " ANSI_GREEN "'%s'" ANSI_RESET, CONFIG_TEMP_PATH);
        forge_set_status(StatusCode::FAILURE);
        goto exit_main;
    }

    for (u32 i = 0; i < as_index(AssetType::COUNT); i++) {
        forge_generate_asset_part((AssetType)i);

        if (forge_get_status() == StatusCode::FAILURE) {
            log_format(LOG_PREFIX_ERRO "FORGE > Failed to generate asset!");
        }
    }

    // Combine asset files
    if (forge_get_status() == StatusCode::CHANGED) {
        forge_combine_asset_files();

        if (forge_get_status() == StatusCode::FAILURE) {
            log_format(LOG_PREFIX_ERRO "FORGE > Failed to combine asset files!");
        }
    }

exit_main:
    return (i32)forge_get_status();
}
