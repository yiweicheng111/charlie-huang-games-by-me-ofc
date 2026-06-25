#include "IRenderer.h"
#include "OpenGL4/OpenGLRenderer.h"
namespace Cle::Renderer
{
	
	std::unique_ptr<IRenderer> Cle::Renderer::IRenderer::Create(entt::registry* registry)

	{
		if (Gfx::G_PIPE_LINE == Gfx::Pipeline::OPENGL)
		{
			return std::make_unique<Cle::OPENGL::Renderer>(registry);
		}
	}
}