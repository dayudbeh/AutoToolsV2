#pragma once
#include <pl/Mod.hpp>

class MyMod {
public:
    static MyMod& instance() {
        static MyMod inst;
        return inst;
    }

    MyMod();
    [[nodiscard]] ll::mod::NativeMod& getSelf() const { return mSelf; }

    bool load();
    bool enable();
    bool disable();
    bool unload();

private:
    ll::mod::NativeMod& mSelf;
};
