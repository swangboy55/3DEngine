#pragma once

#include "ICollisionMesh.h"

#define AXIS_THIS_1 1
#define AXIS_THIS_2 2
#define AXIS_THIS_3 3
#define AXIS_OTHER_1 4
#define AXIS_OTHER_2 5
#define AXIS_OTHER_3 6
#define AXIS_1X1 7
#define AXIS_1X2 8
#define AXIS_1X3 9
#define AXIS_2X1 10
#define AXIS_2X2 11
#define AXIS_2X3 12
#define AXIS_3X1 13
#define AXIS_3X2 14
#define AXIS_3X3 15
#define AXIS_CROSS(a,b) (AXIS_1X1 + ((int)(b)) + (3 * ((int)(a))))

/*
all collision meshes are rotated prisms
the physics object is located at the center of the collision mesh, therefore
bounds are at 

+-(l/2)
+-(w/2)
+-(h/2)

		  ________
		 /|      /|
		/_|_____/ |    h
		| |_____|_|
		| /     | /
		|/______|/    l
    
			w
*/


namespace ginkgo
{
	class CollisionMesh : public ICollisionMesh
	{
	private:
		//width (x), height(y), length(z)
		float extents[3];
		glm::vec3 axes[3];
		glm::vec3 cachedCenter;
		glm::vec3 cachedVel;

		IPhysicsObject* owner;

		MoveInfo lastMove;

		void getLastSeparatingAxis(ICollisionMesh const& other, float deltaTime, CollisionInfo& collisionOut);
		void this_lastSeparatingAxis(ICollisionMesh const& other, CollisionInfo& collisionOut);
		void other_lastSeparatingAxis(ICollisionMesh const& other, CollisionInfo& collisionOut);
		void A0xB0_lastSeparatingAxis(ICollisionMesh const& other, CollisionInfo& collisionOut);
		void A0xB1_lastSeparatingAxis(ICollisionMesh const& other, CollisionInfo& collisionOut);
		void A0xB2_lastSeparatingAxis(ICollisionMesh const& other, CollisionInfo& collisionOut);
		void A1xB0_lastSeparatingAxis(ICollisionMesh const& other, CollisionInfo& collisionOut);
		void A1xB1_lastSeparatingAxis(ICollisionMesh const& other, CollisionInfo& collisionOut);
		void A1xB2_lastSeparatingAxis(ICollisionMesh const& other, CollisionInfo& collisionOut);
		void A2xB0_lastSeparatingAxis(ICollisionMesh const& other, CollisionInfo& collisionOut);
		void A2xB1_lastSeparatingAxis(ICollisionMesh const& other, CollisionInfo& collisionOut);
		void A2xB2_lastSeparatingAxis(ICollisionMesh const& other, CollisionInfo& collisionOut);

	public:
		CollisionMesh(float w, float h, float l, glm::vec3 const& wAxis, glm::vec3 const& hAxis, glm::vec3 const& lAxis);
		virtual glm::vec3 const* getBoundingVertices() const override;
		virtual MoveInfo const& getLastMove() const override;
		virtual void generateVertexPath(float deltaTime) override;

		virtual void setOwner(IPhysicsObject* owner) override;

		virtual bool testCollision(ICollisionMesh const& other, float deltaTime, CollisionInfo& collisionOut) override;
		virtual float getCollisionTime(glm::vec3 const& axisNorm, ICollisionMesh const& other, float deltaTime) const override;
		virtual bool testCollisionStationary(ICollisionMesh const& other, CollisionStationary& collisionOut) override;

		virtual glm::vec3 const& getAxis(int axis) const override;

		virtual float getExtent(int extent) const override;

		virtual void generateCollisionInfo(ICollisionMesh const& other, CollisionInfo& collisionOut) override;
		//TRUE if not intersecting, FALSE if intersecting
		bool testAxis(glm::vec3 const& axisNorm, ICollisionMesh const& other, float deltaTime) const;
		bool testAxisStationary(glm::vec3 const& axisNorm, ICollisionMesh const& other) const;
		float getAxisOverlap(glm::vec3 const& axisNorm, ICollisionMesh const& other) const override;

		virtual IPhysicsObject* getOwner() const override;


		void setCachedCenter(glm::vec3 const& center) override;
		glm::vec3 const& getCachedCenter() const override;

		void setCachedVelocity(glm::vec3 const& vel) override;
		glm::vec3 const& getCachedVelocity() const override;
	};
}