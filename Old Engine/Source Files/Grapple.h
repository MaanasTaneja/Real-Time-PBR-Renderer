#pragma once
Shader* lineShader;

class Grapple
{
public:
	glm::vec3 start;
	glm::vec3 end;

	float thickness;
	glm::vec3 color;

	btGeneric6DofSpring2Constraint* spring;
	btDiscreteDynamicsWorld* loadedWorld;

	Entity* body;
	btRigidBody* target;
	Player* player;

	Camera* playCamera;

	LineRenderer* lineRender;
	btVector3 hitPoint;
	btTransform cursorPos;

	void projectGrapple(glm::vec3 start_, glm::vec3 frontVector, glm::vec3 color_, float thiccness)
	{
		start = start_;
		end = frontVector * 100.0f;
		color = color_;
		thickness = thiccness;

		btVector3 strt = btVector3(start.x, start.y, start.z);
		btVector3 endp = btVector3(end.x, end.y, end.z);

		btCollisionWorld::ClosestRayResultCallback rayCallback(strt, endp);

		loadedWorld->rayTest(strt, endp, rayCallback);

		if (rayCallback.hasHit() == true)
		{
			body = (Entity*)rayCallback.m_collisionObject->getUserPointer();
			if (body->isLevel == true)
			{
				target = nullptr;
			}
			else {
				target = (btRigidBody*)rayCallback.m_collisionObject;
				hitPoint = rayCallback.m_hitPointWorld;
			}
		}

		lineRender->lineThickness = thickness;
		lineRender->lineColor = color;

		updateSpring();
	}

	void update()
	{
		btTransform a;
		player->playerRigidBody->getMotionState()->getWorldTransform(a);
		btTransform b;
		if (target != nullptr)
		{
			target->getMotionState()->getWorldTransform(b);

			glm::mat4 pointPos = glm::mat4(1.0f);
			pointPos = glm::translate(pointPos, glm::vec3(0.0f, 0.0f, -1.0f));

			glm::mat4 camModel = glm::inverse(playCamera->GetViewMatrix());

			pointPos = camModel * pointPos;

			glm::vec3 pos = pointPos[3];

			cursorPos.setOrigin(btVector3(pos.x, pos.y, pos.z));
			glm::vec3 vertex1 = glm::vec3(pos.x, pos.y, pos.z);

			

			glm::vec3 vertex2 = glm::vec3(b.getOrigin().getX(), b.getOrigin().getY(), b.getOrigin().getZ());

			lineRender->setPositions(vertex1, vertex2);
			updateSpring();
		}

	}

	void cleanRenderer()
	{
		lineRender->setPositions(glm::vec3(0.0f), glm::vec3(0.0f));
		if (spring != nullptr)
		{
			loadedWorld->removeConstraint(spring);

		}

	}


private:

	void updateSpring()
	{
		if (target != nullptr)
		{
			target->activate();
			btTransform rbTrans;
		    target->getMotionState()->getWorldTransform(rbTrans);
			btVector3 rbPos = rbTrans.getOrigin();

			btTransform camTrans;
	       player->playerRigidBody->getMotionState()->getWorldTransform(camTrans);
		   btVector3 camPos = cursorPos.getOrigin();

		   btVector3 forceDir = camPos - rbPos;

			target->applyCentralForce(forceDir * 40.0f);
		}
	}

	private:
		float springRestLen = 5.0f;
		float springRange = -5.0f;
};