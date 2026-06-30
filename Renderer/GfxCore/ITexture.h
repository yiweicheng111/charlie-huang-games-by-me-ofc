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
		bool gpuUploaded = false;
		ITexture(std::string path) { this->path = path; }
		std::string getPath() const
		{
			return path;
		}
		ITexture() = default;
		virtual void setPath(std::string newPath) = 0;
		template <class Archive>
		void save(Archive& ar) const
		{
			ar(path);
		}
		template <class Archive>
		void load(Archive& ar)
		{
			ar(path);
		}
	};
}