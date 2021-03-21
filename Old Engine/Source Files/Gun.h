#pragma once
class Gun:public Entity
{
public:
	Model* bulletModel;
	Model* gunModel;
	float cooldown = 0.0f;
	float recoil = 0.0f;
	float bulletSpeed = 0.0f;
	float bulletSpread = 0.0f;
	int nBullets = 0;
	bool isDynamic = false;

	float killstrength = 0.1;
	float attractstrength = 0.0f;

	btSoftRigidDynamicsWorld* loadedWorld;
	btRigidBody* gunBody;
	btVector3 gunTip;
	Camera* playerCamera;
	vector<Entity*> bullets;


	EntityRenderer* entityRenderer;
	Shape primitive;
	ParticleMaster* particleHandler;

	string name;

	glm::vec3 position;
	glm::vec3 scale = glm::vec3(0.1f);
	float rot;

	glm::quat recoilRotation = glm::angleAxis(glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	glm::quat startRecoil = glm::angleAxis(glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	float recoilTimer = 0.0f;

	bool active = false;
	glm::vec3 tempPos = glm::vec3(0.0f);

	Gun(bool dynamic, float recoil, float bulletspeed, float spread, int numBullets, glm::vec3 guntip, btSoftRigidDynamicsWorld* world, Camera* player)
	{
		isDynamic = dynamic;
		bulletSpeed = bulletspeed;
		bulletSpread = spread;
		nBullets = numBullets;
		name = "normal_gun";

		loadedWorld = world;
		playerCamera = player;

		gunTip = btVector3(guntip.x, guntip.y, guntip.z);
	}

	Gun(bool dynamic, float recoil, float bulletspeed, float spread, int numBullets, glm::vec3 guntip)
	{
		isDynamic = dynamic;
		bulletSpeed = bulletspeed;
		bulletSpread = spread;
		nBullets = numBullets;
		name = "normal_gun";

		gunTip = btVector3(guntip.x, guntip.y, guntip.z);
	}

	Gun(float killStrength)
	{
		name = "gravity_killer";
		killstrength = killStrength;
	}

	Gun(float killStrength , bool isAttracter)
	{
		name = "gravity_attracter";
		attractstrength = killStrength;
	}

	Gun()
	{

	}

	void shoot()
	{
		if (name.compare("normal_gun") == 0)
		{
			gun_shoot();
		}

		else if (name.compare("gravity_killer") == 0)
		{
		gravKiller_shoot();
		}

		else if (name.compare("gravity_attracter") == 0)
		{
		attracter_shoot();
		}
	}

	

	void gun_shoot()
	{
		
		recoilRotation = glm::angleAxis(glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		recoilTimer = deltaTime;

		srand(glfwGetTime());
		glm::mat4 pointPos = glm::mat4(1.0f);

		pointPos = glm::translate(pointPos, glm::vec3(0.0f, 0.0f, -0.3f));

		glm::mat4 camModel = glm::inverse(playerCamera->GetViewMatrix());

		pointPos = camModel * pointPos;

		glm::vec3 pos = pointPos[3];

	

		ParticleSystem* muzzleFlash = new ParticleSystem(STATIONARY_SPHERE_SYSTEM, glm::vec3(tempPos.x, tempPos.y+0.04f, tempPos.z), glm::vec3(0.0f), glm::vec3(0.1f), 2, 1.0f, 0.08f, 1.0f, false);
		muzzleFlash->sphereRadius = glm::vec3(0.001f);
		//muzzleFlash->randomizeColors();
		muzzleFlash->addTextureAtlas("resources/gameObjects/particleTextures/Particle_Explosion_5.png", 4);
		muzzleFlash->randomizeParticleRotations();
		muzzleFlash->randomizeParticleScale();
		muzzleFlash->animateRotationTo(360.0f);
		muzzleFlash->animateScaleTo(1.0f);

		particleHandler->addParticleSystem(muzzleFlash);

		for (int i = 0; i < nBullets; i++)
		{
			Entity* bullet;
			
			bullet = new Entity(primitive.cube(), pos, glm::vec3(0.007f), 0.0f, 1.25f, true);
			bullet->isBullet = true;
			bullet->entityRigidBody->setUserPointer(bullet);
			bullet->frontVector = playerCamera->Front;
			entityRenderer->addEntity(bullet);
			bullet->entityRigidBody->setCcdSweptSphereRadius(0.1);
			loadedWorld->addRigidBody(bullet->entityRigidBody);
			bullet->entityRigidBody->setGravity(btVector3(0.0f, 0.0f, 0.0f));
			
		
			float randRes = (double)rand() / RAND_MAX * 0.25f;
			glm::mat4 rotationMat = glm::mat4(1.0f);

			randRes = (randRes > bulletSpread) ? bulletSpread : randRes;
		
			rotationMat = glm::translate(rotationMat, glm::vec3(gunTip.x(), gunTip.y(), gunTip.z()));

			glm::vec3 finalDir = playerCamera->Front;
			finalDir = glm::vec3(rotationMat * glm::vec4(finalDir, 1.0f));

			if (i == 0)
			{
				bullet->entityRigidBody->setLinearVelocity((btVector3(finalDir.x, finalDir.y, finalDir.z) ) * bulletSpeed / 8.0f);
			}
			else {

				rotationMat = glm::rotate(rotationMat, randRes, glm::vec3(0.0f, 1.0f, 0.0f));
				rotationMat = glm::rotate(rotationMat, randRes, glm::vec3(1.0f, 0.0f, 0.0f));
				rotationMat = glm::translate(rotationMat, glm::vec3(gunTip.x(), gunTip.y(), gunTip.z()));

				glm::vec3 finalDir = playerCamera->Front;
				finalDir = glm::vec3(rotationMat * glm::vec4(finalDir, 1.0f));

				bullet->entityRigidBody->setLinearVelocity((btVector3(finalDir.x, finalDir.y, finalDir.z) ) * bulletSpeed / 8.0f);
			}
		
			bullets.push_back(bullet);
		}

		//playerCamera->shake_camera();
	}

	void gravKiller_shoot()
	{
		glm::vec3 haha = playerCamera->Front * 100.0f;
		btVector3 start = btVector3(playerCamera->Position.x, playerCamera->Position.y, playerCamera->Position.z);
		btVector3 end = btVector3(haha.x, haha.y, haha.z);
		

		btCollisionWorld::ClosestRayResultCallback rayCall(start, end);
		Entity* hitEnt;
		loadedWorld->rayTest(start, end, rayCall);
	
		if (rayCall.hasHit() == true)
		{
		    hitEnt = (Entity*) rayCall.m_collisionObject->getUserPointer();
			hitEnt->entityRigidBody->activate();
			
			hitEnt->entityRigidBody->setGravity(btVector3(0.0f, killstrength, 0.0f));
		}
	}

	void attracter_shoot()
	{
		glm::mat4 pointPos = glm::mat4(1.0f);
		pointPos = glm::translate(pointPos, glm::vec3(0.0f, 0.0f, -1.0f));

		glm::mat4 camModel = glm::inverse(playerCamera->GetViewMatrix());

		pointPos = camModel * pointPos;

		glm::vec3 pos = pointPos[3];


		glm::vec3 haha = playerCamera->Front * attractstrength;
		btVector3 start = btVector3(playerCamera->Position.x, playerCamera->Position.y, playerCamera->Position.z);
		btVector3 end = btVector3(haha.x, haha.y, haha.z);


		btCollisionWorld::ClosestRayResultCallback rayCall(start, end);
		Entity* hitEnt;
		loadedWorld->rayTest(start, end, rayCall);

		if (rayCall.hasHit() == true)
		{
			hitEnt = (Entity*)rayCall.m_collisionObject->getUserPointer();
			hitEnt->entityRigidBody->activate();

			hitEnt->entityRigidBody->applyCentralForce(-btVector3(pos.x, pos.y, pos.z) * 100.0f * hitEnt->entityRigidBody->getMass());
			
		}
	}
	

	
	void update()
	{
		for (int i = 0; i < bullets.size(); i++)
		{
			onContact(i);
		}

		if (recoilTimer != 0.0f)
		{
			if (recoilTimer > 0.1f)
			{
				recoilTimer = 0.0f;
				recoilRotation = glm::angleAxis(glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			
			}
			else {
				recoilTimer = recoilTimer + deltaTime;
			}
		
		}
	
	}

	void genGunRigid()
	{
		btConvexHullShape* convexHull = new btConvexHullShape();

		for (int i = 0; i < gunModel->meshes.size(); i++)
		{
			for (auto& pos : gunModel->meshes.at(i).vertices)
			{
				btVector3 vertex0 = btVector3(pos.Position.x,
					pos.Position.y,
					pos.Position.z);


				btScalar scale1 = scale.x;
				vertex0 *= scale1;


				convexHull->addPoint(vertex0);
			}
		}



		btCollisionShape* collisionShape = convexHull;

		btTransform trans;
		trans.setIdentity();
		trans.setOrigin(btVector3(position.x, position.y, position.z));

		btVector3 inertia = btVector3(0.0f, 0.0f, 0.0f);

		collisionShape->calculateLocalInertia(1.0f, inertia);
		collisionShape->setMargin(0.0001f);

		btDefaultMotionState* motionState = new btDefaultMotionState(trans);

		btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(1.0F, motionState, collisionShape, inertia);

		gunBody = new btRigidBody(rigidBodyCI);

	}

private:

	btConvexHullShape* hullOfModel(Model* model, glm::vec3 scale)
	{
		btConvexHullShape* convexHull = new btConvexHullShape();

		for (int i = 0; i < model->meshes.size(); i++)
		{
			for (auto& pos : model->meshes.at(i).vertices)
			{

				//	cout << k << endl;

				btVector3 vertex0 = btVector3(pos.Position.x,
					pos.Position.y,
					pos.Position.z);


				btScalar scale1 = scale.x;
				vertex0 *= scale1;


				convexHull->addPoint(vertex0);
			}
		}

		return convexHull;


	}


	void onContact(int i)
	{
		if (bullets.data() != NULL)
		{
			btVector3 currentStart = btVector3(bullets.at(i)->entityRigidBody->getWorldTransform().getOrigin().getX(), bullets.at(i)->entityRigidBody->getWorldTransform().getOrigin().getY(),
				bullets.at(i)->entityRigidBody->getWorldTransform().getOrigin().getZ());

			glm::vec4 end = bullets.at(i)->generateModelMatrix() * glm::vec4(bullets.at(i)->frontVector * 20.0f, 1.0f);
			btVector3 currentEnd = btVector3(end.x, end.y, end.z);
			btCollisionWorld::ClosestRayResultCallback rayCall(currentStart, currentEnd);

			loadedWorld->rayTest(currentStart, currentEnd, rayCall);

			if (rayCall.hasHit() == true)
			{
				entityRenderer->ents = findAndRemove(bullets.at(i), entityRenderer->ents);
				bullets = findAndRemove(bullets.at(i), bullets);
				//loadedWorld->removeRigidBody(bullets.at(i)->entityRigidBody);
				glm::vec3 hitPoint = glm::vec3(rayCall.m_hitPointWorld.getX(), rayCall.m_hitPointWorld.getY(), rayCall.m_hitPointWorld.getZ());
				glm::vec3 normalAtPoint = glm::vec3(rayCall.m_hitNormalWorld.getX(), rayCall.m_hitNormalWorld.getY(), rayCall.m_hitNormalWorld.getZ());


				ParticleSystem* temp = new ParticleSystem(BURST_SYSTEM, hitPoint, normalAtPoint, glm::vec3(0.008f), 30, -2.0f, 4.0f, 1.0f, true);
				temp->sphereRadius = glm::vec3(0.01f);
				temp->customInitialVelocity = glm::vec3(0.4f);
				//temp->randomizeColors();
				//temp->addTextureAtlas("resources/gameObjects/particleTextures/Particle_Triangle_4.png", 4);
				temp->randomizeParticleRotations();
				temp->randomizeParticleScale();
				temp->animateRotationTo(360.0f);

				particleHandler->addParticleSystem(temp);


			}
			
		}
		
	}

	vector<Entity*> findAndRemove(Entity* entity, vector<Entity*> list)
	{
		for (int i = 0; i < list.size(); i++)
		{
			if (entity == list.at(i))
			{
				list.erase(remove(list.begin(), list.end(), entity), list.end());
					
			}
		}

		return list;
	}

	
};