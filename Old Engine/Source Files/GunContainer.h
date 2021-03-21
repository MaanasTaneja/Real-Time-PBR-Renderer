#pragma once
#include <vector>

Shader* gunShader;


class GunContainer
{

public:

	vector<Gun*> guns;

	Gun* activeGun;

	bool isChildOfCamera = false;
	Camera* playerCamera;
	btSoftRigidDynamicsWorld* loadedWorld;
	bool isGunEquiped = false;
	glm::vec3 childPosition;
	glm::vec3 pickupPosition;
	glm::quat pickupRotation;

	

	EntityRenderer* entRenderer;
	ParticleMaster* particleHandler;


	void addGun(Gun* gun)
	{
		gun->particleHandler = particleHandler;
		gun->entityRenderer = entRenderer;
		gun->playerCamera = playerCamera;
		guns.push_back(gun);
	}

	void setupShaderUniforms(glm::mat4 projection_, glm::mat4 view_, Camera camera3) //temp to be shifted to masterrednerer
	{

		gunShader->use();
		gunShader->setMat4("projection", glm::mat4(projection_));
		gunShader->setMat4("view", glm::mat4(view_));
		gunShader->setVec3("skyColor", glm::vec3(1.0f, 1.0f, 1.0f));

	}

	glm::mat4 setupModelMatrix(int Index)  //SETS UP ONLY MODEL BOI
	{
		
		    gunShader->use();
			btTransform physicsTransform;
			guns.at(Index)->gunBody->getMotionState()->getWorldTransform(physicsTransform);

			float matrix[16];
			physicsTransform.getOpenGLMatrix(matrix);

			glm::mat4 modelMatrix = glm::mat4(1.0f);
			modelMatrix = glm::mat4(glm::make_mat4(matrix));

		modelMatrix = glm::scale(modelMatrix, guns.at(Index)->scale);


			gunShader->setMat4("model", modelMatrix);
			return modelMatrix;
		

	}

	int i = 0;

	glm::mat4 setupChildModel(glm::mat4 viewMat)
	{
		glm::mat4 cameraModel = glm::inverse(viewMat);
		glm::mat4 localTransform = glm::mat4(1.0f);


		glm::mat3 cameraRot = glm::mat3(cameraModel);
		glm::quat cameraRotTrans = (glm::quat_cast(cameraRot));
		
		
		glm::quat targetRot = glm::angleAxis(glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::quat interpRot = glm::slerp(pickupRotation, targetRot, deltaTime * 13.0f);
		
		glm::vec4 targetPos = cameraModel * glm::vec4(0.1f, -0.07f,-0.25f, 1.0f);

		glm::vec3 interpPos = glm::mix(pickupPosition, glm::vec3(targetPos) , deltaTime * 13.0f);

		glm::quat interpRecoilRot = glm::slerp(activeGun->startRecoil , activeGun->recoilRotation , deltaTime*20.0f);


		localTransform = glm::translate(localTransform, glm::vec3(interpPos));
		localTransform = localTransform * glm::mat4_cast(cameraRotTrans);
		localTransform = localTransform * glm::mat4_cast(interpRot);
		localTransform = localTransform * glm::mat4_cast(interpRecoilRot);
		localTransform = glm::scale(localTransform, activeGun->scale);

		childPosition = localTransform[3];
		pickupPosition = childPosition;
		pickupRotation = interpRot;
		activeGun->startRecoil = interpRecoilRot;

		gunShader->use();
		gunShader->setMat4("model", localTransform);
		i++;
		activeGun->tempPos = childPosition;

		//cout << "PLAYERPOS" << playerCamera->Position.x << " " << playerCamera->Position.y << " " << playerCamera->Position.z << " " << "SHOULD BE CORRCT PLS " << targetPos.x << "  " << targetPos.y << "  " << targetPos.z << endl;
		return localTransform;
	}

	void setupNormalMatrix(glm::mat4 modelMatrix)
	{
		gunShader->use();
		glm::mat3 normalMatrix = glm::mat3(1.0f);
		normalMatrix = glm::mat3(glm::transpose(glm::inverse(modelMatrix)));

		gunShader->setMat3("normalMatrix", normalMatrix);
	}

	void init(Shader* entShader, EntityRenderer* entRender)
	{
		gunShader = entShader; //change this to init
		entRenderer = entRender;
	}

	Shader* getShader()
	{
		return gunShader;
	}

	int pickupGun()
	{
		if (activeGun == nullptr)
		{
			isGunEquiped = true;
			btVector3 start = btVector3(playerCamera->Position.x, playerCamera->Position.y, playerCamera->Position.z);
			btVector3 end = btVector3(playerCamera->Front.x, playerCamera->Front.y, playerCamera->Front.z) * 100.0f;

			btCollisionWorld::ClosestRayResultCallback rayCallback(start, end);

			loadedWorld->rayTest(start, end, rayCallback);

			if (rayCallback.hasHit() == true)
			{
				Gun* picked = (Gun*)rayCallback.m_collisionObject->getUserPointer();

				if (picked->tempCheck.compare("OOF") != true)
				{
					activeGun = picked;
					pickupPosition = glm::vec3(activeGun->gunBody->getWorldTransform().getOrigin().getX(), activeGun->gunBody->getWorldTransform().getOrigin().getY(), activeGun->gunBody->getWorldTransform().getOrigin().getZ());

					btTransform pickupTrans;
					activeGun->gunBody->getMotionState()->getWorldTransform(pickupTrans);
					float temp[16];
					pickupTrans.getOpenGLMatrix(temp);

					glm::mat4 tempTrans = glm::mat4(1.0f);
					tempTrans = glm::mat4(glm::make_mat4(temp));

					glm::vec3 scale;
					glm::vec3 trans;
					glm::vec3 skew;
					glm::vec4 pers;

					glm::decompose(tempTrans, scale, pickupRotation, trans, skew, pers);

					isChildOfCamera = true;
					guns = findAndRemove(picked, guns);
				}

			}
		}
		return 0;
	}

	void throwGun()
	{
		if (activeGun != nullptr)
		{
			btConvexHullShape* convexHull = new btConvexHullShape();

			for (int i = 0; i < activeGun->gunModel->meshes.size(); i++)
			{
				for (auto& pos : activeGun->gunModel->meshes.at(i).vertices)
				{
					btVector3 vertex0 = btVector3(pos.Position.x,
						pos.Position.y,
						pos.Position.z);


					btScalar scale1 = activeGun->scale.x;
					vertex0 *= scale1;


					convexHull->addPoint(vertex0);
				}
			}



			btCollisionShape* collisionShape = convexHull;

			btTransform trans;
			trans.setIdentity();
			trans.setOrigin(btVector3(childPosition.x, childPosition.y, childPosition.z));


			btDefaultMotionState* motion = new btDefaultMotionState(trans);

			btVector3 inertia = btVector3(0.0f, 0.0f, 0.0f);
			collisionShape->calculateLocalInertia(1.0f, inertia);

			btRigidBody::btRigidBodyConstructionInfo rbCI(1.0f, motion, collisionShape, inertia);
			btRigidBody* rb = new btRigidBody(rbCI);


			Gun* tempGun = new Gun();
			tempGun = activeGun;
			tempGun->gunBody = rb;
			tempGun->tempCheck = "OOF";
			tempGun->gunBody->setUserPointer(tempGun);
			loadedWorld->addRigidBody(tempGun->gunBody);
			tempGun->gunBody->applyCentralForce(btVector3(playerCamera->Front.x, playerCamera->Front.y, playerCamera->Front.z) *200);

			guns.push_back(tempGun);

			isChildOfCamera = false;
			isGunEquiped = false;
			activeGun = nullptr;
		}
		
	}


	void renderGuns(glm::mat4 proj, glm::mat4 viewM, Camera camera2)
	{

		setupShaderUniforms(proj, viewM, camera2);

		for (int i = 0; i < guns.size(); i++)
		{
			glm::mat4 pass = setupModelMatrix(i);
			setupNormalMatrix(pass);
			guns.at(i)->gunModel->Draw(gunShader);
		}

		if (activeGun != nullptr)
		{
			glClear(GL_DEPTH_BUFFER_BIT);
			glm::mat4 pass = setupChildModel(viewM);
			setupNormalMatrix(pass);
			activeGun->gunModel->Draw(gunShader);
			activeGun->update();
		}
		
	}

	private:

		btRigidBody* currentlyPicked;

		vector<Gun*> findAndRemove(Gun* entity, vector<Gun*> list)
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