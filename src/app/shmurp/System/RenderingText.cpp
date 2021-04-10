#include "RenderingText.h"

#include "../configuration.h"

#include "RenderingShaders.h"
#include "Shaper.h"
#include "transformations.h"

namespace ad {


namespace sp = std::placeholders;


RenderingText::RenderingText(aunteater::Engine &aEngine, ::ad::Engine & aAppEngine) :
    mRenderables{aEngine},
    mImpl{aAppEngine.getFramebufferSize()}
{
    glClearColor(0.04f, 0.08f, 0.12f, 1.f);
    // Note: binding once construction is complete
    mSizeListener = aAppEngine.listenFramebufferResize(
            std::bind(&RenderingText::resizeRenderTarget, this, sp::_1));
}

void RenderingText::update(const aunteater::Timer aTimer)
{
    std::map<Shape::Value, std::vector<instance3D::Data>> sorted;

    for (const auto & renderable : mRenderables)
    {
        sorted[renderable->get<Shape>().enumerator]
            .push_back({renderable->get<Geometry>().position,
                        transform::makeOrientationMatrix(renderable->get<Geometry>().orientations)
                             * transform::scaleMatrix(renderable->get<Geometry>().radius)
                             * transform::scaleMatrix(renderable->get<Geometry>().scale) });
    }

    for (auto & [shape, instancing] : mImpl.mShapeToSpecification)
    {
        // indexing operator returns default constructed vector (i.e. empty) on absent shape
        instancing.updateIBO(sorted[shape]);
    }

    mImpl.draw();
}


void RenderingText::resizeRenderTarget(Size<2, GLsizei> aNewResolution)
{
    mImpl.mOkBloomer = Bloom{aNewResolution};
}


RenderingText::Impl::Impl(Size<2, GLsizei> aResolution):
    mProgram{makeLinkedProgram({ {GL_VERTEX_SHADER,   gVertexShader3D},
                                 {GL_FRAGMENT_SHADER, gFragmentShader3D} })},
    mOkBloomer{aResolution}
{
    mShapeToSpecification.emplace(Shape::RocketShip,
                                  ShapeInstancing(
                                      ship3D::gVertices,
                                      {},
                                      GL_LINE_LOOP,
                                      Vec<4, GLfloat>(0.44f, 0.9f, 1.0f, 1.0f)));

    mShapeToSpecification.emplace(Shape::Square,
                                  ShapeInstancing(
                                      cube::gVertices,
                                      {},
                                      GL_LINES,
                                      Vec<4, GLfloat>(0.96f, 0.14f, 0.97f, 1.0f)));

    mShapeToSpecification.emplace(Shape::Bullet,
                                  ShapeInstancing(
                                      circle3D::makeVertices<30, circle3D::Disc>(1.0f),
                                      {},
                                      GL_TRIANGLE_FAN,
                                      //Vec<4, GLfloat>(0.44f, 0.9f, 1.0f, 1.0f)));
                                      //Vec<4, GLfloat>(1.0f, 0.42f, 0.07f, 1.0f)));
                                      Vec<4, GLfloat>(0.98f, 0.23f, 0.74f, 1.0f)));

    mShapeToSpecification.emplace(Shape::Laser,
                                  ShapeInstancing(
                                      rectangle3D::makeVertices({4.f, 1.f}),
                                      {},
                                      GL_LINES,
                                      Vec<4, GLfloat>(0.44f, 0.9f, 1.0f, 1.0f)));

    mShapeToSpecification.emplace(Shape::FilledRectangle,
                                  ShapeInstancing(
                                      rectangle3D::makeVertices({1.f, 1.f}),
                                      {},
                                      GL_TRIANGLE_FAN,
                                      Vec<4, GLfloat>(1.0f, 0.8f, 0.9f, 1.0f)));

    mShapeToSpecification.emplace(Shape::Circle,
                                  ShapeInstancing(
                                      circle3D::makeVertices<40, circle3D::Circle>(1.0f),
                                      {},
                                      GL_LINES,
                                      Vec<4, GLfloat>(1.0f, 0.22f, 0.39f, 1.0f)));

    // Apparently, cannot instantiate a valid gsl::span from a temporary vector
    auto pyramidVertices = make_pyramid<3>(conf::gPyramidRadius, conf::gPyramidHeight);
    mShapeToSpecification.emplace(Shape::Pyramid,
                                  ShapeInstancing(
                                      pyramidVertices *=
                                        transform::rotateMatrix_Y(pi<Radian<>>/2.f)
                                        * transform::translateMatrix(-conf::gPyramidHeight/2.f, 0.f),
                                      {},
                                      GL_LINES,
                                      Vec<4, GLfloat>(1.0f, 0.22f, 0.39f, 1.0f)));

    glProgramUniformMatrix4fv(mProgram, glGetUniformLocation(mProgram, "u_WorldToDevice"),
                              1, /*transpose*/true, conf::worldToDevice().data());
}

void RenderingText::Impl::draw()
{
    {
        auto offscreenGuard{mOkBloomer.bindFramebuffer()};

        glClear(GL_COLOR_BUFFER_BIT);
        glClearBufferfv(GL_COLOR, 1, gBlack.data());

        //glEnable(GL_BLEND);
        //glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
        //glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ONE, GL_ONE);

        glUseProgram(mProgram);

        for (const auto & [shape, instancing] : mShapeToSpecification)
        {
            glProgramUniform4fv(mProgram, glGetUniformLocation(mProgram, "u_Color"),
                                1, instancing.mColor.data());
            instancing.draw();
        }
    }

    //glDisablei(GL_BLEND, 0);
    mOkBloomer.bloom(4);
    mOkBloomer.drawResult();
}

} // namespace add
