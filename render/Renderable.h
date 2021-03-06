#pragma once


#include "IRenderable.h"
#include "Transform.h"
#include <gl/glew.h>

namespace ginkgo {

	class Mesh;
	struct Material;

	class Renderable : public IRenderable
	{
	private:
		const Mesh* mesh;
		Material* material;
		Transform model;
		static int index;
		int r_index;
	public:
		Renderable(const Mesh* mesh, Material* material);

		void setMesh(const Mesh* mesh) override { this->mesh = mesh; }
		void setMaterial(Material* material) override { this->material = material; }
		Material& getMaterial() override { return *material; }
		
		const mat4& getModel() const override;
		Transform& getTransform() override { return const_cast<Transform&>(static_cast<const Renderable*>(this)->getTransform()); }
		Transform const& getTransform() const override { return model; }

		const Mesh& getMesh() const override { return *mesh; }
		const Material& getMaterial() const override { return *material; }

		const int getIndex() const override { return r_index; }

		void draw() const override;

		virtual ~Renderable();
	};

}

/*
Renderables all share a 
Mesh (pointer)
but each have a unique Material
but (possible) share the Material's const Texture

change to one Mesh changes all Rendrables
change to one Material changes one Rendrables
cannot change Material's const Texture

cannot change Material of a mesh?
*/