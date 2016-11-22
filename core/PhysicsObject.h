#pragma once

#include "IPhysicsObject.h"
#include "Entity.h"

namespace ginkgo
{
	class PhysicsObject : public IPhysicsObject
	{
	private:
		ICollisionMesh* collisionMesh;
		Material material;
		float mass;
		bool canCollide;
		UINT32 numCollisions;
		UINT32 movementState;
		UINT32 collisionType;

		IEntity* const parent;

		MoveResult finalMove;

		std::forward_list<SurfaceData> normalList;

	public:
		PhysicsObject(IEntity* parent, ICollisionMesh* collision, UINT32 collisionType, float mass, Material mat, bool canCollide = true);

		bool checkCollision(float deltaTime, IPhysicsObject* other) override;

		void setMaterial(const Material& mat) override;
		void setMass(float mass) override;
		void setCanCollide(bool collides) override;
		void setCollisionMesh(ICollisionMesh* collision) override;
		void incrementCollision() override;
		void decrementCollision() override;
		void setMovementState(UINT32 state) override;

		const Material& getMaterial() const override;
		float getMass() const override;
		bool doesCollide() const override;
		ICollisionMesh* getCollisionMesh() const override;
		UINT32 getNumCollisions() const override;
		UINT32 getCollisionType() const override;

		bool isMoving() const override;

		void setFinalMove(MoveResult const& finalMove) override;

		void onTick(float elapsedTime) override;

		IEntity* const getParent() const override;

		const MoveResult& getMoveResult() const override;
		
		std::forward_list<SurfaceData> const& getCollisionNormalList() const override;
		void removeNormal(SurfaceData const& data) override;
	};
}