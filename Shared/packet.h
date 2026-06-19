#pragma once
#include <cereal.hpp>
#include <access.hpp>
#include <types/string.hpp>
#include <archives/binary.hpp>
#include "entt.hpp"
#include <iostream>
#include "Assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
namespace Cle
{
	struct ColorPacket
	{
		int networkID;
		double r, b, g, a;
		ColorPacket()
		{
		}
		template <class Archieve>
		void save(Archieve ar, const ColorPacket& packet)
		{
			ar(packet.networkID, packet.r, packet.b, packet.g, packet.a);
		}
		template <class Archieve>

		void load(Archieve ar, ColorPacket& packet)
		{
			ar(packet.networkID, packet.r, packet.b, packet.g, packet.a);
		}
	};
	struct MeshPacket
	{
		int networkID;
		int meshIndex;
		std::string path;
		MeshPacket(const std::string& path, int meshIndex) :path(path), meshIndex(meshIndex) {}

		static std::vector<MeshPacket> loadModel(const std::string& path)
		{
			std::vector<MeshPacket> total;
			Assimp::Importer importer;
			const aiScene* scene = importer.ReadFile(path,0);
			for (int i = 0; i < scene->mNumMeshes; i++)
			{
				total.emplace_back(path, i);
			}
		}
		template <class Archieve>
		void save(Archieve ar, const MeshPacket& packet)
		{
			ar(packet.networkID, packet.path);
		}
		template <class Archieve>

		void load(Archieve ar, MeshPacket& packet)
		{
			ar(packet.networkID, packet.path);
		}
	};

	struct TransformPacket
	{
		int networkID;
		double px, py, pz;
		double rx, ry, rz;
		double sx, sy, sz;
		template <class Archieve>
		void save(Archieve ar, const TransformPacket& packet)
		{
			ar(packet.networkID, packet.px, packet.py, packet.pz, packet.rx, packet.ry, packet.rz, packet.sx, packet.sy, packet.sz);
		}
		template <class Archieve>

		void load(Archieve ar, TransformPacket& packet)
		{
			ar(packet.networkID, packet.px, packet.py, packet.pz, packet.rx, packet.ry, packet.rz, packet.sx, packet.sy, packet.sz);
		}
	};
}