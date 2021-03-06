#include "Collision.h"
#include "ICollisionMesh.h"
#include "IPhysicsObject.h"
#include "Core.h"
#include "IEntity.h"

namespace ginkgo
{
	Collision::Collision(float deltaTime, CollisionInfo const& manifold)
		: deltaTime(deltaTime), manifold(manifold.thisMesh, manifold.otherMesh)
	{
		valid = true;
		markedForDestruction = false;
		overlapSkipFixRan = false;
		getUpdatedParams();
		this->manifold.normal = manifold.collisionNormal;
		this->manifold.overlapDist = manifold.thisMesh->getAxisOverlap(manifold.collisionNormal, *manifold.otherMesh);
	}

	void Collision::applyFriction()
	{
		float COF = glm::min(manifold.thisMesh->getOwner()->getMaterial().friction, manifold.otherMesh->getOwner()->getMaterial().friction);
		
		vec3 normal = glm::normalize(manifold.normal);
		vec3 invNormal = -normal;

		float angle = glm::min(glm::acos(glm::dot(-glm::normalize(getWorld()->getGravity()), normal)), glm::acos(glm::dot(glm::normalize(getWorld()->getGravity()), normal)));

		float normalScalar = glm::abs(glm::length(getWorld()->getGravity()) * glm::cos(angle));

		vec3 refRVel = referenceResult.finalVel - otherResult.finalVel;
		vec3 invRVel = -refRVel;

		//PERPENDICULAR FORMULA: (N dot N / Vel dot N) * vel - N
		//resist motion along velocity direction
		float vDotNormal = glm::dot(refRVel, normal);
		float vDotInvNormal = glm::dot(invRVel, invNormal);

		vec3 frictVector;
		vec3 invFrictVector;

		if (vDotNormal == 0.f)
		{
			frictVector = -glm::normalize(refRVel) * normalScalar * deltaTime * COF;
			if (glm::abs(normalScalar) > glm::length(refRVel))
			{
				frictVector = -refRVel;
			}
		}
		else
		{
			vec3 pVec = (1.f / vDotNormal) * refRVel - normal;

			float pLen = glm::length(pVec);

			if (pLen > 0.00001f)
			{
				pVec = (glm::dot(refRVel, pVec) / (pLen * pLen)) * pVec;
				frictVector = -glm::normalize(pVec) * normalScalar * deltaTime * COF;
				if (glm::length(pVec) < glm::length(frictVector))
				{
					frictVector = -pVec;
				}
			}
		}

		if (manifold.otherMesh->getOwner()->getCollisionType() == CTYPE_WORLDDYNAMIC)
		{
			if (vDotInvNormal == 0.f)
			{
				invFrictVector = -glm::normalize(invRVel) * normalScalar * deltaTime * COF;
				if (glm::abs(normalScalar) > glm::length(invRVel))
				{
					invFrictVector = -invRVel;
				}
			}
			else
			{
				vec3 pVec = (1.f / vDotInvNormal) * invRVel - invNormal;

				float pLen = glm::length(pVec);

				if (pLen > 0.00001f)
				{
					pVec = (glm::dot(invRVel, pVec) / (pLen * pLen)) * pVec;
					invFrictVector = -glm::normalize(pVec) * normalScalar * deltaTime * COF;
					if (glm::length(pVec) < glm::length(invFrictVector))
					{
						invFrictVector = -pVec;
					}
				}
			}
		}

		referenceResult.finalVel += frictVector;
		otherResult.finalVel += invFrictVector;

		manifold.thisMesh->setCachedVelocity(referenceResult.finalVel);
		manifold.otherMesh->setCachedVelocity(otherResult.finalVel);

		/* 
*******************!!ALTERNATE IMPLEMENTATION!!********************
		float COF = glm::min(manifold.thisMesh->getOwner()->getMaterial().friction, manifold.otherMesh->getOwner()->getMaterial().friction);

		float xe = manifold.otherMesh->getExtent(0);


		//PERPENDICULAR FORMULA: (N dot N / Vel dot N) * vel - N
		vec3 normal = glm::normalize(manifold.normal);
		float vDotNormal = glm::dot(referenceResult.finalVel, normal);

		vec3 pVecRef;

		if (vDotNormal == 0.f)
		{
		pVecRef = referenceResult.finalVel;
		}
		else
		{
		pVecRef = (1.f / vDotNormal) * referenceResult.finalVel - normal;

		float pLen = glm::length(pVecRef);

		if (pLen != 0.f)
		{
		pVecRef = (glm::dot(referenceResult.finalVel, pVecRef) / (pLen * pLen)) * pVecRef;
		}
		}
		vec3 nVecRef = vDotNormal * normal;

		vec3 invNormal = -normal;
		float vDotInvNormal = glm::dot(otherResult.finalVel, invNormal);
		vec3 pVecOther;
		if (vDotInvNormal == 0.f)
		{
		pVecOther = otherResult.finalVel;
		}
		else
		{
		pVecOther = (1.f / vDotInvNormal) * otherResult.finalVel - invNormal;

		float pLen = glm::length(pVecOther);

		if (pLen != 0.f)
		{
		pVecOther = (glm::dot(otherResult.finalVel, pVecOther) / (pLen * pLen)) * pVecOther;
		}
		}

		vec3 nVecOther = vDotInvNormal * invNormal;

		pVecRef *= glm::pow(COF, deltaTime);
		pVecOther *= glm::pow(COF, deltaTime);

		if (glm::length(pVecRef) < 0.0001f)
		{
		pVecRef = vec3();
		}

		if (glm::length(pVecOther) < 0.0001f)
		{
		pVecOther = vec3();
		}

		referenceResult.finalVel = pVecRef + nVecRef;
		otherResult.finalVel = pVecOther + nVecOther;

		manifold.thisMesh->setCachedVelocity(referenceResult.finalVel);
		manifold.otherMesh->setCachedVelocity(otherResult.finalVel);
		*/

	}

	void Collision::impulseCorrection()
	{
		IPhysicsObject* refObj = manifold.thisMesh->getOwner(), *otherObj = manifold.otherMesh->getOwner();
		float mass = refObj->getMass(), otherMass = otherObj->getMass();

		vec3 const& vel = referenceResult.finalVel;
		vec3 const& otherVel = otherResult.finalVel;

		vec3 const& gravity = getWorld()->getGravity();
		
		//Contact velocity
		float contactVel = glm::dot(otherVel - vel, manifold.normal);
		//printf("%f\n", contactVel);
		if (contactVel < 0)
		{
			return;
		}
		//restitutionA * restitutionB = total restitution
		float restitution = /*glm::max(otherObj->getMaterial().reboundFraction , */(refObj->getMaterial().reboundFraction);

		//impulse scalar
		//if type is worldstatic, mass is infinite (1/mass = 0)
		float IS = -(1.f + restitution) * contactVel;
		float invMassThis, invMassOther;

		invMassThis = 1.f / mass;

		if (otherObj->getCollisionType() == CTYPE_WORLDSTATIC)
		{
			invMassOther = 0;
		}
		else
		{
			invMassOther = 1 / otherMass;
		}

		IS /= (invMassThis + invMassOther);

		//if we reset position in 4 ticks, stick
		vec3 impulse = manifold.normal * IS;
		vec3 alongNormal = glm::dot(manifold.normal, vel - impulse) * manifold.normal;
		vec3 gravDirection = (refObj->getParent()->isGravityEnabled() ? glm::normalize(gravity) : vec3());
		float amountInGrav = glm::dot(alongNormal, gravDirection);
		float gravScale = glm::length(gravity);
		float gravFloor = -(gravScale * deltaTime * 8);

		if (amountInGrav < 0 && amountInGrav > gravFloor)
		{
			IS = (-contactVel) / (invMassThis + invMassOther);
			impulse = manifold.normal * IS;
		}

		referenceResult.finalVel -= (invMassThis * impulse);
		otherResult.finalVel += (invMassOther * impulse);

		manifold.thisMesh->setCachedVelocity(referenceResult.finalVel);
		manifold.otherMesh->setCachedVelocity(otherResult.finalVel);
	}

	void Collision::positionalCorrectionInternal(float frameSegment)
	{
		float invMassRef = manifold.thisMesh->getOwner()->getMass();
		float invMassOther = manifold.otherMesh->getOwner()->getMass();
		if (manifold.thisMesh->getOwner()->getCollisionType() == CTYPE_WORLDSTATIC)
		{
			invMassRef = 0;
		}
		else
		{
			invMassRef = 1.f / invMassRef;
		}

		if (manifold.otherMesh->getOwner()->getCollisionType() == CTYPE_WORLDSTATIC)
		{
			invMassOther = 0;
		}
		else
		{
			invMassOther = 1.f / invMassOther;
		}

		float correctionScalar = 0;
		if (manifold.overlapDist >= MIN_CORRECTDIST)
		{
			correctionScalar = manifold.overlapDist;
		}

		vec3 correction = (correctionScalar / (invMassRef + invMassOther)) * frameSegment * manifold.normal;
		referenceResult.finalPos += invMassRef * correction;
		otherResult.finalPos -= invMassOther * correction;

		referenceResult.finalPos = glm::round(referenceResult.finalPos * 10000.f) / 10000.f;
		otherResult.finalPos = glm::round(otherResult.finalPos * 10000.f) / 10000.f;

		manifold.thisMesh->setCachedCenter(referenceResult.finalPos);
		manifold.otherMesh->setCachedCenter(otherResult.finalPos);
	}

	void Collision::positionalCorrection(float correctionPercent)
	{
		if (!valid)
		{
			return;
		}
		impulseCorrection();
		positionalCorrectionInternal(correctionPercent);
		updateValidity();
	}

	void Collision::getUpdatedParams()
	{
		referenceResult.finalPos = manifold.thisMesh->getCachedCenter();
		referenceResult.finalVel = manifold.thisMesh->getCachedVelocity();
		otherResult.finalPos = manifold.otherMesh->getCachedCenter();
		otherResult.finalVel = manifold.otherMesh->getCachedVelocity();
	}

	void Collision::postCorrection()
	{
		manifold.thisMesh->getOwner()->setFinalMove(referenceResult);
		manifold.otherMesh->getOwner()->setFinalMove(otherResult);
	}

	void Collision::updateValidity()
	{
		valid = !manifold.thisMesh->testCollisionStationary(*manifold.otherMesh, manifold);
	}

	void Collision::preCollisionUpdate()
	{
		markedForDestruction = !manifold.thisMesh->testCollisionStationary(*manifold.otherMesh, manifold);
	}

	bool Collision::eq(IPhysicsObject* a, IPhysicsObject* b) const
	{
		long IDt = manifold.thisMesh->getOwner()->getParent()->getEntityID();
		long IDo = manifold.otherMesh->getOwner()->getParent()->getEntityID();
		long IDa = a->getParent()->getEntityID();
		long IDb = b->getParent()->getEntityID();

		return (IDt == IDa && IDo == IDb) || (IDt == IDb && IDo == IDa);
	}
}