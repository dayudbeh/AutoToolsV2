#include "MyMod.h"
#include <pl/memory/Signature.hpp>

// Assinatura extraída do mod original para troca de slot
constexpr const char* SIG_INVENTORY_SELECT = "?? ?? ?? D1 ?? ?? ?? A9 ?? ?? ?? A9 ?? ?? ?? A9 ?? ?? ?? A9 ?? ?? ?? A9 ?? ?? ?? A9 ?? ?? ?? 91 5B D0 3B D5 F3 03 08 AA F5 03 02 2A";

MyMod::MyMod() : mSelf(*ll::mod::NativeMod::current()) {}

bool MyMod::load() {
    getSelf().getLogger().info("AutoTool carregado com sucesso!");
    return true;
}

bool MyMod::enable() {
    auto& logger = getSelf().getLogger();
    
    // Tenta encontrar a função de troca de slot no binário do Minecraft
    uintptr_t addrSelectSlot = pl::memory::resolveSignature(SIG_INVENTORY_SELECT, "libminecraftpe.so");
    
    if (addrSelectSlot != 0) {
        logger.info("InventorySelectSlot encontrada no endereco: {:#x}", addrSelectSlot);
    } else {
        logger.warn("InventorySelectSlot NAO encontrada. Verifique a versao do jogo.");
    }

    logger.info("AutoTool ativado e pronto para uso.");
    return true;
}

bool MyMod::disable() {
    getSelf().getLogger().info("AutoTool desativado.");
    return true;
}

bool MyMod::unload() {
    return true;
}

// A macro de registro deve ficar APENAS no arquivo .cpp
PL_REGISTER_MOD(MyMod, MyMod::instance())
