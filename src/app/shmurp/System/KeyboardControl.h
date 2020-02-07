#pragma once

#include <Utils/Periodic.h>

#include <aunteater/System.h>

namespace ad {

class KeyboardControl : public aunteater::System
{
    enum Direction : short
    {
        None    = 0,
        Up      = (1 << 0),
        Down    = (1 << 1),
        Left    = (1 << 2),
        Up_Left = (Up & Left),
        Down_Left = (Down & Left),
        Right   = (1 << 3),
        Up_Right = (Up & Right),
        Down_Right = (Down & Right),
    };

    struct Callback
    {
        void operator()(int key, int scancode, int action, int mods);

        short mDirection{0};
        bool mFiring{false};
    };

public:
    KeyboardControl(aunteater::Engine &aEngine);
    void update(double time) override;

    std::shared_ptr<Callback> getCallback();

private:
    std::shared_ptr<Callback> mCallback{std::make_shared<Callback>()};
    aunteater::Family & mPlayerMovable;
    aunteater::Engine & mEngine;
    Periodic mBulletPeriod{0.02f};
};

inline std::shared_ptr<KeyboardControl::Callback> KeyboardControl::getCallback()
{
    return mCallback;
}

} // namespace ad
