#pragma once
#include "GfxBase.h"
#include "IRenderer.h"
#include <memory>
namespace Cle::Gfx
{	
	class IWindow
	{
	public:
		virtual bool shouldClose() const = 0;
		virtual void swapBuffers() const = 0;
		virtual void* getWindow() const = 0;
		virtual float getDt() = 0;
		static IWindow createWindow();
	};
	
}