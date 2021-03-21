#pragma once

class Physics
{
public:

	btBroadphaseInterface* m_Broadphase;
	btSoftRigidDynamicsWorld* world;
	btSequentialImpulseConstraintSolver* m_Solver;
	btDefaultSoftBodySolver* m_softSolver;
	btSoftBodyRigidBodyCollisionConfiguration* m_collisionConfig;
	btCollisionDispatcher* m_Dispatcher;

	void initializeRigidWorld()
	{
		m_Broadphase = new btDbvtBroadphase();
		m_collisionConfig = new btSoftBodyRigidBodyCollisionConfiguration();
		m_Dispatcher = new btCollisionDispatcher(m_collisionConfig);

		m_Solver = new btSequentialImpulseConstraintSolver();
		m_softSolver = new btDefaultSoftBodySolver();

		world = new btSoftRigidDynamicsWorld(m_Dispatcher, m_Broadphase, m_Solver, m_collisionConfig, m_softSolver);
		
		world->setGravity(btVector3(0.0f, -9.81f, 0.0f));
	}

	void initializeSoftRigidWorld()
	{
		m_Broadphase = new btDbvtBroadphase();
		m_collisionConfig = new btSoftBodyRigidBodyCollisionConfiguration();
		m_Dispatcher = new btCollisionDispatcher(m_collisionConfig);

		m_Solver = new btSequentialImpulseConstraintSolver();
		m_softSolver = new btDefaultSoftBodySolver();

		world = new btSoftRigidDynamicsWorld(m_Dispatcher, m_Broadphase, m_Solver, m_collisionConfig, m_softSolver);
		world->setGravity(btVector3(0.0f, -9.81f, 0.0f));
	
	}

	void assignDebugDrawer(BulletDebugDrawer* drawer)
	{
		drawer->Init();
		world->setDebugDrawer(drawer);
	}
	
};
 

inline btScalar	calculateCombinedFriction(float friction0, float friction1)
{
	return 0.f;
	btScalar friction = friction0 * friction1;

	const btScalar MAX_FRICTION = 10.f;
	if (friction < -MAX_FRICTION)
		friction = -MAX_FRICTION;
	if (friction > MAX_FRICTION)
		friction = MAX_FRICTION;
	return friction;

}

inline btScalar	calculateCombinedRestitution(float restitution0, float restitution1)
{
	return restitution0 * restitution1;
}



