#include <aunteater/Archetype.h>
#include <aunteater/FamilyHelp.h>
#include <aunteater/System.h>

#include <engine/Engine.h>

#include <renderer/Shading.h>

namespace ad {

typedef aunteater::Archetype<Geometry, Text> RenderableText;

class RenderingText : public aunteater::System
{
    using ShapeInstancing = Instancing<vertex3D::Data, instance3D::Data>;

    struct Impl
    {
        Impl(Size<2, GLsizei> aResolution);
        void draw();

        Program mProgram;
    };

public:
    RenderingText(aunteater::Engine &aEngine, ::ad::Engine & aAppEngine);

    void update(const aunteater::Timer aTimer) override;

    void resizeRenderTarget(Size<2, GLsizei> aNewResolution);

private:
    aunteater::FamilyHelp<RenderableText> mRenderablesText;
    std::shared_ptr<::ad::Engine::SizeListener> mSizeListener;
    Impl mImpl;
};

} // namespace ad
