
class Entity
{
public:

	string name = "Entity"; 
	Model* modelEnt;
	bool isModel = false;
	bool isGlass = false;
	
	unsigned int VAO;
	Texture entTex;
	Texture diffuseTex;
	Texture specularTex;

	Texture *albedoTex;
	Texture *normalTex;
	Texture *metallicTex;
	Texture *roughnessTex;
	Texture *aoTex;

	glm::vec3 position;
	float rotation;
	glm::vec3 scale;

	bool textComp=false;
	bool pMaps = false;
	bool pbr = false;
	bool extraMaterials = false;

	float extraRough = 0.0f;
	float extraMetal = 0.0f;
	bool isLevel = false;
	bool isBullet = false;

	vector<glm::vec3> EntityCovexHull;


	btRigidBody* entityRigidBody;
	glm::vec3 frontVector;
	string tempCheck = "OOF";

	Entity(unsigned int VAO_, glm::vec3 position_, glm::vec3 scale_, float rotation_, float mass, bool lockRotation)
	{
		VAO = VAO_;
		position = position_;
		scale = scale_;
		rotation  = rotation_;

		btCollisionShape* collisionShape = new btBoxShape(btVector3(scale.x, scale.y, scale.z));

		btTransform trans;
		trans.setIdentity();
		trans.setOrigin(btVector3(position.x, position.y, position.z));

		btVector3 inertia = btVector3(0.0f, 0.0f, 0.0f);

		if (lockRotation != true)
		{
			collisionShape->calculateLocalInertia(mass, inertia);
		}
		collisionShape->setMargin(0.0f);

		btDefaultMotionState* motionState = new btDefaultMotionState(trans);

		btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(mass, motionState, collisionShape, inertia);

		entityRigidBody = new btRigidBody(rigidBodyCI);
		entityRigidBody->setCollisionFlags(entityRigidBody->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
	}

	Entity(unsigned int VAO_)
	{
		VAO = VAO_;
	}

	Entity(string modelN, glm::vec3 position_, glm::vec3 scale_, float rotation_)
	{
		isModel = true;
		modelEnt = new Model(modelN);
		position = position_;
		scale = scale_;
		rotation = rotation_;

	}


	Entity(Model* model, glm::vec3 position_, glm::vec3 scale_)
	{
		isModel = true;
		modelEnt = model;
		position = position_;
		scale = scale_;

	}

	Entity(Model* model, glm::vec3 position_, glm::vec3 scale_, float rotation_)
	{
		isModel = true;
		modelEnt = model;
		position = position_;
		scale = scale_;
		rotation = rotation_;
	
		btCollisionShape* collisionShape = new btBoxShape(btVector3(0.001f, 0.001f, 0.001f));

		btTransform trans;
		trans.setIdentity();
		trans.setOrigin(btVector3(position.x, position.y, position.z));

		btVector3 inertia = btVector3(0.0f, 0.0f, 0.0f);

		collisionShape->calculateLocalInertia(0.0f, inertia);

		btDefaultMotionState* motionState = new btDefaultMotionState(trans);

		btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(0.0f, motionState, collisionShape, inertia);

		entityRigidBody = new btRigidBody(rigidBodyCI);

		entityRigidBody->setCollisionFlags(entityRigidBody->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);

	}
	

	Entity(Model* model, glm::vec3 position_, glm::vec3 scale_, float rotation_, float mass)
	{
		isModel = true;
		modelEnt = model;
		position = position_;
		scale = scale_;
		rotation = rotation_;

		btConvexHullShape* convexHull = new btConvexHullShape();

		for (int i = 0; i < model->meshes.size() ; i++)
		{
			for (auto& pos : model->meshes.at(i).vertices)
			{

				//	cout << k << endl;

				btVector3 vertex0 = btVector3(pos.Position.x,
					pos.Position.y,
					pos.Position.z);


				btScalar scale1 = scale.x;
				vertex0 *= scale1;

				EntityCovexHull.push_back(pos.Position);
				convexHull->addPoint(vertex0);
			}
		}

		

		btCollisionShape* collisionShape = convexHull;

		btTransform trans;
		trans.setIdentity();
		trans.setOrigin(btVector3(position.x, position.y, position.z));

		btVector3 inertia = btVector3(0.0f, 0.0f, 0.0f);

		collisionShape->calculateLocalInertia(mass, inertia);
		collisionShape->setMargin(0.0001f);

		btDefaultMotionState* motionState = new btDefaultMotionState(trans);

		btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(mass, motionState, collisionShape, inertia);
		entityRigidBody = new btRigidBody(rigidBodyCI);
		entityRigidBody->setCollisionFlags(entityRigidBody->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);

	}

	Entity(Model* model, glm::vec3 position_, glm::vec3 scale_, float rotation_, bool isLevel)
	{
		isModel = true;
		modelEnt = model;
		position = position_;
		scale = scale_;
		rotation = rotation_;

		gContactAddedCallback = MyContactAddedCallback;

		btTriangleMesh* modelMesh = new btTriangleMesh();

		for (int i = 0; i < model->meshes.size(); i++)
		{
			for (int k = 0; k < model->meshes.at(i).indices.size() ; k+=3)
			{
				
			//	cout << k << endl;

				btVector3 vertex0 = btVector3(model->meshes.at(i).vertices.at(model->meshes.at(i).indices.at(k)).Position.x, 
					model->meshes.at(i).vertices.at(model->meshes.at(i).indices.at(k)).Position.y,
					model->meshes.at(i).vertices.at(model->meshes.at(i).indices.at(k)).Position.z);

				btVector3 vertex1 = btVector3(model->meshes.at(i).vertices.at(model->meshes.at(i).indices.at(k+1)).Position.x,
					model->meshes.at(i).vertices.at(model->meshes.at(i).indices.at(k+1)).Position.y,
					model->meshes.at(i).vertices.at(model->meshes.at(i).indices.at(k+1)).Position.z);

				btVector3 vertex2 = btVector3(model->meshes.at(i).vertices.at(model->meshes.at(i).indices.at(k+2)).Position.x,
					model->meshes.at(i).vertices.at(model->meshes.at(i).indices.at(k+2)).Position.y,
					model->meshes.at(i).vertices.at(model->meshes.at(i).indices.at(k+2)).Position.z);

				btScalar scale1 = scale.x;
				vertex0 *= scale1;
				vertex1 *= scale1;
				vertex2 *= scale1;


				modelMesh->addTriangle(vertex0, vertex1, vertex2);
			}
	    }

		btBvhTriangleMeshShape* trimesh = new btBvhTriangleMeshShape(modelMesh, true);


		btCollisionShape* collisionShape = trimesh;
		btTriangleInfoMap* triangleInfoMap = new btTriangleInfoMap();
		btGenerateInternalEdgeInfo(trimesh, triangleInfoMap);
		
		btTransform trans;
		trans.setIdentity();
		trans.setOrigin(btVector3(position.x, position.y, position.z));

		btVector3 inertia = btVector3(0.0f, 0.0f, 0.0f);

		collisionShape->calculateLocalInertia(0.0f, inertia);

		btDefaultMotionState* motionState = new btDefaultMotionState(trans);

		btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(0.0f, motionState, collisionShape, inertia);
		rigidBodyCI.m_restitution = 1.0f;

		entityRigidBody = new btRigidBody(rigidBodyCI);
		entityRigidBody->setFriction(1.5f);
		entityRigidBody->setCollisionFlags(entityRigidBody->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
		isLevel = true;
	}


	

	Entity()
	{

	}

	void addTextureComponent(string texPath)
	{
		entTex.getTexID(texPath);
		textComp = true;
	}

	void addPhongMapsComponent(string diffuse, string spec)
	{
		diffuseTex.getTexID(diffuse);
		specularTex.getTexID(spec);
		pMaps = true;
	}

	void addExtraMaterialComponents(float roughness, float metallic)
	{
		extraMaterials = true;
		extraRough = roughness;
		extraMetal = metallic;
	}

	void addPBRcomponent(string albedo,string normal,string metallic,string roughness,string ao)
	{
		albedoTex = new Texture();
		normalTex = new Texture();
		metallicTex = new Texture();
		roughnessTex = new Texture();
		aoTex = new Texture();

		albedoTex->getTexID(albedo);
		normalTex->getTexID(normal);
		metallicTex->getTexID(metallic);
		roughnessTex->getTexID(roughness);
		aoTex->getTexID(ao);

		pbr = true;
	}


	unsigned int getVAO()
	{
		return VAO;
	}

	void setPosition(glm::vec3 pos)
	{
		position = pos;
	}

	void setScale(glm::vec3 scal)
	{
		scale = scal;
	}

	void setRotation(float rotation_)
	{
		rotation = rotation_;
	}

	glm::vec3 getPosition()
	{
		return position;
	}

	glm::vec3 getScale()
	{
		return scale;
	}

	float getRotation()
	{
		return rotation;
	}

	glm::mat4 generateModelMatrix()
	{
		btTransform physicsTransform;
		entityRigidBody->getMotionState()->getWorldTransform(physicsTransform);

		float matrix[16];
		physicsTransform.getOpenGLMatrix(matrix);

		glm::mat4 modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::mat4(glm::make_mat4(matrix));

		modelMatrix = glm::scale(modelMatrix, scale);
		return modelMatrix;
	}

	private:
	

		static bool MyContactAddedCallback(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1)
		{
			btAdjustInternalEdgeContacts(cp, colObj1Wrap, colObj0Wrap, partId1, index1);
			return true;
		}
	

};
