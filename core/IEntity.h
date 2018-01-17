#pragma once

#include "CoreReource.h"


namespace ginkgo
{
	class IRenderComponent;
	class IComponent;

	class IEntity
	{
	public:
		virtual void beginTick(float elapsedTime) = 0;
		virtual void endTick(float elapsedTime) = 0;

		virtual const vec3& getPosition() const = 0;
		virtual const vec3& getVelocity() const = 0;
		virtual const vec3& getAcceleration() const = 0;
		virtual quat getRotationQ() const = 0;
		virtual const vec3& getRotation() const = 0;
		virtual long getEntityID() const = 0;
		virtual bool isGravityEnabled() const = 0;

		virtual void setPosition(const vec3& pos) = 0;
		virtual void setVelocity(const vec3& vel) = 0;
		virtual void setAcceleration(const vec3& acc) = 0;
		virtual void addAcceleration(const vec3& acc) = 0;
		virtual void setRotationQ(const quat& ang) = 0;
		virtual void setRotation(const vec3& ang) = 0;
		virtual void setEntityID(long ID) = 0;
		virtual void setGravityEnabled(bool enabled) = 0;

		virtual EntityType getEntityType() const = 0;

		virtual void addComponent(IComponent* component) = 0;

		// INTERNAL USE ONLY!!
		virtual IPhysicsComponent* getPhysics() = 0;
		
		virtual ~IEntity() = 0;
	};
	DECLSPEC_CORE IEntity* entityFactory(const vec3& pos, const vec3& rot = vec3(), const vec3& vel = vec3(), const vec3& accel = vec3());
};
