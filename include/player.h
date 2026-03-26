#pragma once
#include <3ds.h>
#include <string>
#include <vector>

#define MAX_FILES     256
#define MAX_FILENAME  256
#define VOLUME_MAX    100
#define VOLUME_MIN    0
#define SEEK_STEP_SEC 10

// Tipos de arquivo suportados
typedef enum {
    FILE_UNKNOWN = 0,
    FILE_WAV,
    FILE_MP3,
    FILE_OGG,
    FILE_OPUS,
    FILE_FLAC,
} FileType;

// Estado do player
typedef enum {
    PLAYER_STOPPED = 0,
    PLAYER_PLAYING,
    PLAYER_PAUSED,
} PlayerState;

// Entrada de arquivo
typedef struct {
    char      name[MAX_FILENAME];
    char      path[512];
    FileType  type;
    u64       sizeBytes;
} FileEntry;

// Estado global do app
typedef struct {
    // Lista de arquivos
    FileEntry files[MAX_FILES];
    int       fileCount;
    int       selectedIndex;
    int       scrollOffset;

    // Player
    PlayerState playerState;
    int         volume;         // 0..100
    float       progress;       // 0.0..1.0
    double      currentSec;
    double      totalSec;
    bool        showInfo;

    // Nome do arquivo atual
    char currentFile[512];
    char currentTitle[256];
    char currentArtist[256];

    // Buffer de audio ndsp
    ndspWaveBuf  waveBuf[2];
    u8*          audioBuffer;
    bool         ndspReady;
} AppState;

// Funções do player
void appStateInit(AppState* state);
void appStateExit(AppState* state);

void playerInit(AppState* state);
void playerExit(AppState* state);
void playerUpdate(AppState* state);
void playerPlay(AppState* state);
void playerStop(AppState* state);
void playerTogglePause(AppState* state);
void playerSeek(AppState* state, int seconds);
void playerVolume(AppState* state, int delta);

void loadFileList(AppState* state, const char* path);
FileType detectFileType(const char* filename);
