#pragma once
#include <string>
namespace Cle::Gfx
{
	class ITexture
	{
	protected:
		std::string path = "";

	public:
		virtual int getID() const = 0;
		virtual void bind(unsigned int slot) const = 0;
		bool loaded = false;
		std::string getPath() const
		{
			return path;
		}
		virtual void setPath(std::string newPath) = 0;
	
	};
}