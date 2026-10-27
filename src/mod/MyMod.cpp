#include "MyMod.h"
#include <pl/memory/Signature.hpp>
#include <pl/memory/Hook.hpp>

// --- ASSINATURAS REAIS EXTRAÍDAS DO MOD ORIGINAL ---
constexpr const char* SIG_INVENTORY_SELECT = "?? ?? ?? D1 ?? ?? ?? A9 ?? ?? ?? A9 ?? ?? ?? A9 ?? ?? ?? A9 ?? ?? ?? A9 ?? ?? ?? A9 ?? ?? ?? 91 5B D0 3B D5 F3 03 08 AA F5 03 02 2A";
constexpr const char* SIG_SURVIVAL_DESTROY = "?? ?? ?? D1 ?? ?? ?? A9 ?? ?? ?? F9 ?? ?? ?? 91 53 D0 3B D5 E8 03 00 AA ?? ?? ?? 91 ?? ?? ?? F9 ?? ?? ?? 91 ?? ?? ?? F8 ?? ?? ?? 95 ?? ?? ?? 91 ?? ?? ?? 95 ?? ?? ?? 36 ?? ?? ?? 91 ?? ?? ?? 52 ?? ?? ?? 94";
constexpr const char* SIG_CREATIVE_DESTROY = "?? ?? ?? D1 ?? ?? ?? A9 ?? ?? ?? F9 ?? ?? ?? 91 53 D0 3B D5 E8 03 00 AA ?? ?? ?? 91 ?? ?? ?? F9 ?? ?? ?? 91 ?? ?? ?? F8 ?? ?? ?? 94 ?? ?? ?? 91 ?? ?? ?? 94 ?? ?? ?? 36 ?? ?? ?? 91 ?? ?? ?? 52";

// Ponteiros para as funções originais
void* orig_SurvivalDestroy = nullptr;
void* orig_CreativeDestroy = nullptr;

// O ABI (argumentos) CORRETO para startDestroyBlock: this, blockPos, face, hasDestroyedBlock
using StartDestroyFn = bool(*)(void* thisPtr, void* blockPos, unsigned char face, bool* hasDestroyedBlock);

bool detour_SurvivalDestroy(void* thisPtr, void* blockPos, unsigned char face, bool* hasDestroyedBlock) {
    MyMod::instance().getSelf().getLogger().info("AutoTool: Voce comecou a quebrar um bloco (Sobrevivencia)!");
    
    // Chama a função original do jogo para o bloco quebrar normalmente
    if (orig_SurvivalDestroy) {
        return reinterpret_cast<StartDestroyFn>(orig_SurvivalDestroy)(thisPtr, blockPos, face, hasDestroyedBlock);
    }
    return false;
}

bool detour_CreativeDestroy(void* thisPtr, void* blockPos, unsigned char face, bool* hasDestroyedBlock) {
    MyMod::instance().getSelf().getLogger().info("AutoTool: Voce comecou a quebrar um bloco (Criativo)!");
    
    if (orig_CreativeDestroy) {
        return reinterpret_cast<StartDestroyFn>(orig_CreativeDestroy)(thisPtr, blockPos, face, hasDestroyedBlock);
    }
    return false;
}

MyMod::MyMod() : mSelf(*ll::mod::NativeMod::current()) {}

bool MyMod::load() {
    getSelf().getLogger().info("AutoTool carregado com sucesso!");
    return true;
}

bool MyMod::enable() {
    auto& logger = getSelf().getLogger();
    
    // 1. Resolve as assinaturas
    uintptr_t addrSelectSlot = pl::memory::resolveSignature(SIG_INVENTORY_SELECT, "libminecraftpe.so");
    uintptr_t addrSurvivalDestroy = pl::memory::resolveSignature(SIG_SURVIVAL_DESTROY, "libminecraftpe.so");
    uintptr_t addrCreativeDestroy = pl::memory::resolveSignature(SIG_CREATIVE_DESTROY, "libminecraftpe.so");
    
    if (addrSelectSlot != 0) {
        logger.info("InventorySelectSlot encontrada: {:#x}", addrSelectSlot);
    } else {
        logger.warn("InventorySelectSlot NAO encontrada.");
    }

    // 2. Instala os hooks com as assinaturas reais
    if (addrSurvivalDestroy != 0) {
        if (pl::memory::hook((void*)addrSurvivalDestroy, (void*)detour_SurvivalDestroy, &orig_SurvivalDestroy) == 0) {
            logger.info("Hook instalado no SurvivalModeStartDestroyBlock!");
        } else {
            logger.error("Falha ao instalar hook no SurvivalModeStartDestroyBlock.");
        }
    } else {
        logger.warn("Assinatura SurvivalModeStartDestroyBlock nao encontrada.");
    }

    if (addrCreativeDestroy != 0) {
        if (pl::memory::hook((void*)addrCreativeDestroy, (void*)detour_CreativeDestroy, &orig_CreativeDestroy) == 0) {
            logger.info("Hook instalado no GameModeStartDestroyBlock!");
        } else {
            logger.error("Falha ao instalar hook no GameModeStartDestroyBlock.");
        }
    } else {
        logger.warn("Assinatura GameModeStartDestroyBlock nao encontrada.");
    }

    logger.info("AutoTool ativado.");
    return true;
}

bool MyMod::disable() {
    getSelf().getLogger().info("AutoTool desativado.");
    return true;
}

bool MyMod::unload() {
    return true;
}

PL_REGISTER_MOD(MyMod, MyMod::instance())
