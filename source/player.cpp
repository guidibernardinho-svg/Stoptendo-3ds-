#include "player.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <strings.h>

// ── Estado ────────────────────────────────────────────────────────────────────

void appStateInit(AppState* state) {
    memset(state, 0, sizeof(AppState));
    state->volume       = 80;
    state->playerState  = PLAYER_STOPPED;
    state->showInfo     = false;
    state->selectedIndex = 0;
    state->scrollOffset  = 0;
}

void appStateExit(AppState* state) {
    if (state->audioBuffer) {
        linearFree(state->audioBuffer);
        state->audioBuffer = NULL;
    }
}

// ── Detecção de tipo ──────────────────────────────────────────────────────────

FileType detectFileType(const char* filename) {
    const char* ext = strrchr(filename, '.');
    if (!ext) return FILE_UNKNOWN;
    ext++; // pula o ponto
    if (strcasecmp(ext, "wav")  == 0) return FILE_WAV;
    if (strcasecmp(ext, "mp3")  == 0) return FILE_MP3;
    if (strcasecmp(ext, "ogg")  == 0) return FILE_OGG;
    if (strcasecmp(ext, "opus") == 0) return FILE_OPUS;
    if (strcasecmp(ext, "flac") == 0) return FILE_FLAC;
    return FILE_UNKNOWN;
}

// ── Carregamento de lista ─────────────────────────────────────────────────────

void loadFileList(AppState* state, const char* path) {
    state->fileCount = 0;

    DIR* dir = opendir(path);
    if (!dir) {
        // Tenta raiz se /media não existir
        dir = opendir("/");
        if (!dir) return;
        path = "/";
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL && state->fileCount < MAX_FILES) {
        if (entry->d_name[0] == '.') continue;

        FileType ft = detectFileType(entry->d_name);
        if (ft == FILE_UNKNOWN) continue;

        FileEntry* fe = &state->files[state->fileCount];
        strncpy(fe->name, entry->d_name, MAX_FILENAME - 1);
        snprintf(fe->path, sizeof(fe->path), "%s/%s", path, entry->d_name);
        fe->type = ft;
        fe->sizeBytes = 0;
        state->fileCount++;
    }
    closedir(dir);
}

// ── NDSP / Audio ──────────────────────────────────────────────────────────────

void playerInit(AppState* state) {
    ndspSetOutputMode(NDSP_OUTPUT_STEREO);
    ndspChnSetInterp(0, NDSP_INTERP_LINEAR);
    ndspChnSetRate(0, 44100.0f);
    ndspChnSetFormat(0, NDSP_FORMAT_STEREO_PCM16);

    float mix[12];
    memset(mix, 0, sizeof(mix));
    mix[0] = mix[1] = 1.0f;
    ndspChnSetMix(0, mix);

    // Buffer de 512KB em memória linear
    size_t bufSize = 512 * 1024;
    state->audioBuffer = (u8*)linearAlloc(bufSize);
    if (state->audioBuffer) {
        memset(state->waveBuf, 0, sizeof(state->waveBuf));
        state->waveBuf[0].data_vaddr = state->audioBuffer;
        state->waveBuf[0].nsamples   = bufSize / 4; // 16-bit stereo
        state->ndspReady = true;
    }

    // Volume inicial
    ndspChnSetMix(0, mix);
    playerVolume(state, 0);
}

void playerExit(AppState* state) {
    playerStop(state);
}

void playerVolume(AppState* state, int delta) {
    state->volume += delta;
    if (state->volume > VOLUME_MAX) state->volume = VOLUME_MAX;
    if (state->volume < VOLUME_MIN) state->volume = VOLUME_MIN;

    float vol = state->volume / 100.0f;
    float mix[12];
    memset(mix, 0, sizeof(mix));
    mix[0] = mix[1] = vol;
    ndspChnSetMix(0, mix);
}

void playerPlay(AppState* state) {
    if (state->fileCount == 0) return;
    if (state->playerState == PLAYER_PLAYING) playerStop(state);

    FileEntry* fe = &state->files[state->selectedIndex];
    strncpy(state->currentFile,   fe->path, sizeof(state->currentFile) - 1);
    strncpy(state->currentTitle,  fe->name, sizeof(state->currentTitle) - 1);
    strncpy(state->currentArtist, "Desconhecido", sizeof(state->currentArtist) - 1);

    // Aqui você conectaria um decoder (dr_wav, minimp3, stb_vorbis, etc.)
    // Por ora, apenas muda estado para demonstração da estrutura
    state->playerState = PLAYER_PLAYING;
    state->progress    = 0.0f;
    state->currentSec  = 0.0;
    state->totalSec    = 180.0; // placeholder 3 min
}

void playerStop(AppState* state) {
    if (state->playerState == PLAYER_STOPPED) return;
    ndspChnWaveBufClear(0);
    state->playerState = PLAYER_STOPPED;
    state->progress    = 0.0f;
    state->currentSec  = 0.0;
}

void playerTogglePause(AppState* state) {
    if (state->playerState == PLAYER_PLAYING) {
        ndspChnSetPaused(0, true);
        state->playerState = PLAYER_PAUSED;
    } else if (state->playerState == PLAYER_PAUSED) {
        ndspChnSetPaused(0, false);
        state->playerState = PLAYER_PLAYING;
    }
}

void playerSeek(AppState* state, int seconds) {
    if (state->playerState == PLAYER_STOPPED) return;
    state->currentSec += seconds;
    if (state->currentSec < 0)                  state->currentSec = 0;
    if (state->currentSec > state->totalSec)     state->currentSec = state->totalSec;
    if (state->totalSec > 0)
        state->progress = (float)(state->currentSec / state->totalSec);
}

void playerUpdate(AppState* state) {
    if (state->playerState != PLAYER_PLAYING) return;

    // Simula avanço de tempo (substituir pelo tick real do decoder)
    state->currentSec += 1.0 / 60.0;
    if (state->totalSec > 0)
        state->progress = (float)(state->currentSec / state->totalSec);

    if (state->currentSec >= state->totalSec) {
        playerStop(state);
        // Auto-próxima faixa
        if (state->selectedIndex + 1 < state->fileCount) {
            state->selectedIndex++;
            playerPlay(state);
        }
    }
}
