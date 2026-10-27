#include "MyMod.h"
#include <pl/memory/Signature.hpp>
#include <pl/memory/Hook.hpp>

// Assinaturas extraídas do mod original
constexpr const char* SIG_INVENTORY_SELECT = "?? ?? ?? D1 ?? ?? ?? A9 ?? ?? ?? A9 ?? ?? ?? A9 ?? ?? ?? A9 ?? ?? ?? A9 ?? ?? ?? A9 ?? ?? ?? 91 5B D0 3B D5 F3 03 08 AA F5 03 02 2A";
// (Assinaturas genéricas para teste de hook - usaremos padrões conhecidos do Bedrock)
constexpr const char* SIG_SURVIVAL_DESTROY = "?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? 5B D0 3B D5 F3 03 08 AA F5 03 02 2A"; // Placeholder ajustado
constexpr const char* SIG_CREATIVE_DESTROY = "?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? 57 D0 3B D5 F6 03 00 AA"; // Placeholder ajustado

// Ponteiros para as funções originais
void* orig_SurvivalDestroy = nullptr;
void* orig_CreativeDestroy = nullptr;

// Nossas funções de "Detour" (onde o jogo vai passar primeiro)
bool detour_SurvivalDestroy(void* thisPtr, void* blockSource, void* blockPos, int face, void* something) {
    // Aqui o mod acorda quando você quebra um bloco no modo Sobrevivência
    // Por enquanto, apenas logamos que o hook funcionou
    MyMod::instance().getSelf().getLogger().info("AutoTool: Voce comecou a quebrar um bloco (Sobrevivencia)!");
    
    // Chama a função original do jogo para o bloco quebrar normalmente
    using Fn = bool(*)(void*, void*, void*, int, void*);
    return reinterpret_cast<Fn>(orig_SurvivalDestroy)(thisPtr, blockSource, blockPos, face, something);
}

bool detour_CreativeDestroy(void* thisPtr, void* blockSource, void* blockPos, int face, void* something) {
    // Aqui o mod acorda quando você quebra um bloco no modo Criativo
    MyMod::instance().getSelf().getLogger().info("AutoTool: Voce comecou a quebrar um bloco (Criativo)!");
    
    using Fn = bool(*)(void*, void*, void*, int, void*);
    return reinterpret_cast<Fn>(orig_CreativeDestroy)(thisPtr, blockSource, blockPos, face, something);
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
    }

    // 2. Instala os hooks de quebra de bloco
    if (addrSurvivalDestroy != 0) {
        if (pl::memory::hook((void*)addrSurvivalDestroy, (void*)detour_SurvivalDestroy, &orig_SurvivalDestroy) == 0) {
            logger.info("Hook instalado no SurvivalModeStartDestroyBlock!");
        }
    } else {
        logger.warn("Assinatura SurvivalModeStartDestroyBlock nao encontrada.");
    }

    if (addrCreativeDestroy != 0) {
        if (pl::memory::hook((void*)addrCreativeDestroy, (void*)detour_CreativeDestroy, &orig_CreativeDestroy) == 0) {
            logger.info("Hook instalado no GameModeStartDestroyBlock!");
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
