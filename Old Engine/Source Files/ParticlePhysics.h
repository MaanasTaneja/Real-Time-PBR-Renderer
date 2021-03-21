#pragma once

class ParticlePhysics
{
	btSoftRigidDynamicsWorld* loadedWorld;

public:

private:
	void collisionUpdate(Particle* particle)
	{
		btVector3 startPos = btVector3(particle->position.x, particle->position.y, particle->position.z);
		btVector3 direction = btVector3(particle->velocity.x, particle->velocity.y, particle->velocity.z);

		btCollisionWorld::ClosestRayResultCallback rayCall(startPos, direction);
		loadedWorld->rayTest(startPos, direction, rayCall);
		if (rayCall.hasHit() == true)
		{
			glm::vec3 normalAtPoint = glm::vec3(rayCall.m_hitNormalWorld.getX(), rayCall.m_hitNormalWorld.getY(), rayCall.m_hitNormalWorld.getZ());
			particle->velocity.y = particle->velocity.y + normalAtPoint.y;
		}
	}
};