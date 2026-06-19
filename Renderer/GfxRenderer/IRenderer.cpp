#include "IRenderer.h"
#include "OpenGL4/OpenGLRenderer.h"
namespace Cle::Renderer
{
	std::shared_ptr<IRenderer> Cle::Renderer::IRenderer::Create(entt::registry* registry)

	{
		if (Gfx::G_PIPE_LINE == Gfx::Pipeline::OPENGL)
		{
			return std::make_shared<Cle::OPENGL::Renderer>(registry);
		}
	}
}