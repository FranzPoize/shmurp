#pragma once

#include <aunteater/Component.h>

namespace ad {
    

struct Text : public aunteater::Component<Text>
{
    explicit Text(string aText,
            Vec<2, GLfloat> aPosition = Vec<2>::Zero(),
            Font aFont,
            )
        text{aText},
        size{aSize},
        font{std::move(aFont)},
    {}
}


} // namespace ad
