#pragma once

class SoftBodyRenderer
{
public:

	vector<btSoftBody*> softBodies;
	Model* softModel;
	
	vector<btVector3> faceTriangles;
	vector<float> softBodyVertices;
	vector<float> softBodyNormals;
	vector<float> softBodyLines;

	unsigned int softBodyVAO;
	unsigned int vertexVBO;
	unsigned int normalVBO;
	unsigned int texVBO;

	void addSoftBody(btSoftBody* body)
	{
		softBodies.push_back(body);
	}
	
	void updateSoftBody(btSoftBody* sftBdy)
	{
		softBodies.push_back(sftBdy);

		for (int i = 0; i < sftBdy->m_faces.size(); i++)
		{
			for (int j = 0; j < 3; j++)
			{
				softBodyVertices.push_back(sftBdy->m_faces.at(i).m_n[j]->m_x.x());
				softBodyVertices.push_back(sftBdy->m_faces.at(i).m_n[j]->m_x.y());
				softBodyVertices.push_back(sftBdy->m_faces.at(i).m_n[j]->m_x.z());
			}
		}

		for (int i = 0; i < sftBdy->m_faces.size(); i++)
		{
				softBodyNormals.push_back(sftBdy->m_faces.at(i).m_normal.getX());
				softBodyNormals.push_back(sftBdy->m_faces.at(i).m_normal.getY());
				softBodyNormals.push_back(sftBdy->m_faces.at(i).m_normal.getZ());
		}


		glGenVertexArrays(1, &softBodyVAO);
		glBindVertexArray(softBodyVAO);

		glGenBuffers(1, &vertexVBO);
		glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);

		glBufferData(GL_ARRAY_BUFFER, softBodyVertices.size() * sizeof(float) , softBodyVertices.data() , GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		glGenBuffers(1, &normalVBO);
		glBindBuffer(GL_ARRAY_BUFFER, normalVBO);

		glBufferData(GL_ARRAY_BUFFER, softBodyNormals.size() * sizeof(float), softBodyNormals.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);

		if (softModel != nullptr)
		{
			vector<glm::vec2> uv = extractUVS();
			glGenBuffers(1, &texVBO);
			glBindBuffer(GL_ARRAY_BUFFER, texVBO);


			glBufferData(GL_ARRAY_BUFFER, uv.size() * sizeof(glm::vec2), uv.data(), GL_STATIC_DRAW);
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(2);

		}

	}

	void renderSoftBody()
	{

		glBindVertexArray(softBodyVAO);
		glDrawArrays(GL_TRIANGLES, 0, softBodyVertices.size());
	}

	void debugDraw(btSoftRigidDynamicsWorld* world)
	{
		for (int i = 0; i < softBodies.size(); i++)
		{
			btSoftBodyHelpers::Draw(softBodies.at(i) , world->getDebugDrawer(), 0x0004);
		}
	}


private:
	vector<glm::vec2> extractUVS()
	{
		vector<glm::vec2> uvs;

		for (int i = 0; i < softModel->meshes.size(); i++)
		{
			for (int j = 0; j < softModel->meshes.at(i).vertices.size(); j += 2)
			{
				uvs.push_back(softModel->meshes.at(i).vertices.at(j).TexCoords);
			}
		}

		return uvs;
	}
};