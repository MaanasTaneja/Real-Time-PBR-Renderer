#pragma once

class Player:public Entity
{
public:
	glm::vec3 PlayerPosition;
	string type = "player";

	unsigned int currentWeaponVAO;
	
	Model* playerModel;

	

	bool hasModel = false;
	int indexHotbar;

	Camera* PlayerCamera;

	btRigidBody* playerRigidBody;
	btDefaultMotionState* motionState;
	btVector3 playerWallRunVector;
	btSoftRigidDynamicsWorld* loadedWorld;

	bool onGround = true;
	bool isJump = false;
	bool isCrouched = false;
	bool isWallRunning = false;

	bool isJumping = false;
	bool hasPressed = false;

	bool rbMovement = true;
	bool isGunEquiped = false;
	float previousY ;


	Player(glm::vec3 initialPos, Camera* cam, Model* player)
	{
		PlayerPosition = initialPos;
		PlayerCamera = cam;
		playerModel = player;
		hasModel = true;
	}

	Player(glm::vec3 initialPos, Camera* cam, btSoftRigidDynamicsWorld* world)
	{
		PlayerPosition = initialPos;
		PlayerCamera = cam;

		btCollisionShape* collisionShape = new btCapsuleShape(btScalar(0.15f), btScalar(0.2f));
		collisionShape->setMargin(0.0f);


		btTransform trans;
		trans.setIdentity();
		trans.setOrigin(btVector3(initialPos.x, initialPos.y, initialPos.z));


		motionState = new btDefaultMotionState(trans);

		btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(10.0f, motionState, collisionShape, btVector3(0.0f, 0.0f, 0.0f));

		playerRigidBody = new btRigidBody(rigidBodyCI);
		playerRigidBody->setDamping(0.1f, 0.0f);
		
		previousY = initialPos.y;
		//	playerRigidBody->setFriction(0.3f);

			//playerRigidBody->setCcdSweptSphereRadius(4.0f);
			//playerRigidBody->setCcdMotionThreshold(0.0001f);

		loadedWorld = world;

		
	}

	glm::vec3 getPosition()
	{
		return PlayerPosition;
	}


	void update()
	{
		btTransform playerTrans;
		 playerRigidBody->getMotionState()->getWorldTransform(playerTrans);
		 float x = playerTrans.getOrigin().getX();
		float y = playerTrans.getOrigin().getY();
		float z = playerTrans.getOrigin().getZ();

		if(rbMovement==true)
		PlayerCamera->Position = glm::vec3(x, y, z);


	}

	void checkGround(btDiscreteDynamicsWorld* pWorld) //change from manifold method to raytest method then movemnt shoulf be done
	{

		float x = playerRigidBody->getWorldTransform().getOrigin().getX();
		float y = playerRigidBody->getWorldTransform().getOrigin().getY();
		float z = playerRigidBody->getWorldTransform().getOrigin().getZ();


		btVector3 start = btVector3(x, y, z);
		btVector3 end = btVector3(x, -10000, z);

		btCollisionWorld::ClosestRayResultCallback	rayCallback(start, end);

		pWorld->rayTest(start, end, rayCallback);

		if (rayCallback.hasHit() == true)
		{
			glm::vec3 hitPoint = glm::vec3(rayCallback.m_hitPointWorld.getX(), rayCallback.m_hitPointWorld.getY(), rayCallback.m_hitPointWorld.getZ());
			glm::vec3 startPoint = glm::vec3(start.getX(), start.getY(), start.getZ());

			float distance = glm::distance(startPoint, hitPoint);
			if (distance < 0.3f)
			{
				onGround = true;

			
			}

			else
			{
				
				onGround = false;
			}
		}
	}

	void checkJumpPad(btSoftRigidDynamicsWorld* world)
	{
		float x = playerRigidBody->getWorldTransform().getOrigin().getX();
		float y = playerRigidBody->getWorldTransform().getOrigin().getY();
		float z = playerRigidBody->getWorldTransform().getOrigin().getZ();

		btVector3 start = btVector3(x, y, z);
		btVector3 end = btVector3(x, -10000.0f , z);

		btCollisionWorld::ClosestRayResultCallback rayCall(start, end);

		world->rayTest(start, end, rayCall);

		if (rayCall.hasHit() == true)
		{
			//Entity* jumpPad = rayCall.getUserPOINTERTHING();

		}

	}

	void startCrouch(btSoftRigidDynamicsWorld* world)
	{
		world->removeRigidBody(playerRigidBody);
		
		
		btCollisionShape* crouchedShape = new btCapsuleShape(btScalar(0.15f), btScalar(0.05f));
		playerRigidBody->setCollisionShape(crouchedShape);
		
		world->addRigidBody(playerRigidBody);
		isCrouched = true;
		
	}

	void stopCrouch(btSoftRigidDynamicsWorld* world)
	{
	     world->removeRigidBody(playerRigidBody);


		btCollisionShape* crouchedShape = new btCapsuleShape(btScalar(0.15f), btScalar(0.2f));
		playerRigidBody->setCollisionShape(crouchedShape);

		world->addRigidBody(playerRigidBody);
		isCrouched = false;
		
	}

	void checkWalls(btDiscreteDynamicsWorld* pWorld)
	{
		
		int numOfManifolds = pWorld->getDispatcher()->getNumManifolds();
		for (int i = 0; i < numOfManifolds; i++)
		{
			btPersistentManifold* contactManifold = pWorld->getDispatcher()->getManifoldByIndexInternal(i);
			btCollisionObject* obA = (btCollisionObject*)(contactManifold->getBody0());
			btCollisionObject* obB = (btCollisionObject*)(contactManifold->getBody1());

			int numContacts = contactManifold->getNumContacts();

			for (int j = 0; j < numContacts; j++)
			{
				btManifoldPoint& point = contactManifold->getContactPoint(j);
				if (point.getDistance() < 0.f)
				{
					const btVector3& ptA = point.getPositionWorldOnA();
					const btVector3& ptB = point.getPositionWorldOnB();
					const btVector3& normalOnB = point.m_normalWorldOnB;

					Entity* lolum = (Entity*) obB->getUserPointer();
					Player* lolum2 = (Player*)obA->getUserPointer();
					
					if (lolum != nullptr && lolum2 != nullptr)
					{
						if (abs(normalOnB.getY()) - 0.3f < 0.1 && lolum->isLevel == true && lolum2->type.compare("player") == 0)
						{
							playerWallRunVector = normalOnB;
							isWallRunning = true;
						}
						else {
							
						}
						
					}
					
				}

			}
		}
	}


	float playerForce = 130.0f;
	void startWallrun()
	{
		btVector3 wallRunDir = -playerWallRunVector * 180.0f;
		glm::vec3 temp = glm::vec3(wallRunDir.getX(), wallRunDir.getY(), wallRunDir.getZ());
		glm::vec3 temp2 = PlayerCamera->Up;
		glm::vec3 moveVec = glm::cross(temp, temp2);
		btVector3 moveVector = btVector3(moveVec.x, moveVec.y, moveVec.z);
		btVector3 frontRunDir =moveVector * playerForce*0.00025;
		
		playerRigidBody->applyCentralForce(wallRunDir);
		playerRigidBody->applyCentralForce(frontRunDir);
		cout << "wallrunin lol" << endl;
	}

	void endWallrun()
	{
		
		isWallRunning = false;
	}



	
	void rigidBodyMovement(GLFWwindow* window, btSoftRigidDynamicsWorld *world , Camera *camera , float deltaTime)
	{

		rbMovement = true;
		float playerForce = 130.0f *100;

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

		playerRigidBody->setDamping(0.1f, 0.0f);
		checkGround(world);
		//checkWalls(world);

		btVector3  dir;

		if (isWallRunning == true)
		{
			//ALBIAN->player.player->startWallrun();
		}


		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		{
			if (playerRigidBody->getLinearVelocity().length() > 3)
			{

			}
			else {


				dir = btVector3(camera->Front.x, 0.0f, camera->Front.z);
				dir = dir * playerForce * deltaTime;

				int chkX = abs(dir.getX());
				int chkZ = abs(dir.getZ());

				if (chkX <= 4 && chkZ <= 4)
				{
					glm::vec3 cross = glm::cross(camera->Front, -camera->Right);
					btVector3 btCross = btVector3(cross.x, cross.y, cross.z);
					btCross = btCross * playerForce * deltaTime;
					playerRigidBody->applyCentralForce(btCross);
				}

				else {

					playerRigidBody->activate();
					playerRigidBody->applyCentralForce(dir);
					//ALBIAN.player.player->playerRigidBody->applyCentralForce(-dir * 0.2);
				}

			}

			hasPressed = true;
		}

		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		{
			if (playerRigidBody->getLinearVelocity().length() > 3)
			{

			}

			else {
				dir = btVector3(camera->Front.x, 0.0f, camera->Front.z);
				dir = -dir * playerForce * deltaTime;

				int chkX = abs(dir.getX());
				int chkZ = abs(dir.getZ());

				if (chkX <= 4 && chkZ <= 4)
				{
					glm::vec3 cross = glm::cross(camera->Front, camera->Right);
					btVector3 btCross = btVector3(cross.x, cross.y, cross.z);
					btCross = btCross * playerForce * deltaTime;
					playerRigidBody->applyCentralForce(btCross);
				}
				else {
					playerRigidBody->activate();
					playerRigidBody->applyCentralForce(dir);
				}
			}

			hasPressed = true;
		}

		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && onGround == true)
		{
			if (isJumping == true)
			{

			}

			//if (ALBIAN->player.player->isWallRunning == true)
		//	{
			//ALBIAN->player.player->endWallrun();
			//}

			else
			{
				isJumping = true;
				 isJump = true;
				dir = btVector3(0.0f, 1.0f, 0.0f);
				dir = dir * 140 * 13 * deltaTime * 70.0f;
				 playerRigidBody->activate();
				 playerRigidBody->applyCentralForce(dir);
			}
			hasPressed = true;
		}

		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
		{
			isJumping = false;
			 isJump = false;

		}

		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		{
			if ( playerRigidBody->getLinearVelocity().length() > 3)
			{

			}
			else {
				dir = btVector3(camera->Right.x, 0.0f, camera->Right.z);
				dir = -dir * playerForce * deltaTime;
				 playerRigidBody->activate();
				 playerRigidBody->applyCentralForce(dir);
			}
			hasPressed = true;
		}

		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		{
			if ( playerRigidBody->getLinearVelocity().length() > 3)
			{

			}
			else {
				dir = btVector3(camera->Right.x, 0.0f, camera->Right.z);
				dir = dir * playerForce * deltaTime;
				 playerRigidBody->activate();
				 playerRigidBody->applyCentralForce(dir);
			}
			hasPressed = true;
		}

		if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		{
			 startCrouch(world);

			hasPressed = true;
		}



		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_S) == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_A) == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_D) == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_RELEASE)
		{
			if (hasPressed == true &&  onGround == true &&  isCrouched == true)
			{
				 stopCrouch(world);
			}

			else if (hasPressed == true &&  onGround == true)
			{
				hasPressed = false;
				btVector3 counter = playerRigidBody->getLinearVelocity() * 4.7;
			    playerRigidBody->applyCentralImpulse(-counter);


				stopCrouch(world);

			}

			else {


			}
		}

		

	}

	void kinematicMovement(GLFWwindow* window, Camera* camera, float deltaTime)
	{

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			camera->ProcessKeyboard(FORWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			camera->ProcessKeyboard(BACKWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			camera->ProcessKeyboard(LEFT, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			camera->ProcessKeyboard(RIGHT, deltaTime);

		rbMovement = false;
			
	}




};

