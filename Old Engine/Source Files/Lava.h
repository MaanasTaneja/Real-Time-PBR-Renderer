#pragma once

class Lava
{
public:
	bool flowing = true;
	float SIZE = 800.0f;
	int VERTEX_COUNT = 128;
	float yPos = 0.0f;

	unsigned int terrainVAO;
	unsigned int terrainVBO;
	unsigned int vertexVBO;
	unsigned int normalVBO;
	unsigned int texVBO;
	unsigned int terrainEBO;
	unsigned int indicesLen;

	Texture lavaTexture;

	int gridX;
	int gridZ;

	Lava(int gridX_, int gridZ_, float yLocation , string lavTexture)
	{
		yPos = yLocation;
		lavaTexture.getTexID(lavTexture);

		gridX = gridX_;
		gridZ = gridZ_;
	}

	unsigned int generateTerrain()
	{
		int count = VERTEX_COUNT * VERTEX_COUNT;
		std::vector<glm::vec3> vertices(count * 3), normals(count * 3);
		std::vector<glm::vec2> textureCoords(count * 3);
		std::vector<int> indices(6 * (VERTEX_COUNT - 1) * VERTEX_COUNT);


		int vertexPointer = 0;
		for (int i = 0; i < VERTEX_COUNT; i++)
		{
			for (int j = 0; j < VERTEX_COUNT; j++)
			{
				vertices[vertexPointer] = glm::vec3((float)j / ((float)VERTEX_COUNT - 1) * SIZE, yPos, (float)i / ((float)VERTEX_COUNT - 1) * SIZE);
				normals[vertexPointer] = glm::vec3(0.0f, 1.0f, 0.0f);
				textureCoords[vertexPointer] = glm::vec2((float)j / ((float)VERTEX_COUNT - 1), (float)i / ((float)VERTEX_COUNT - 1));
				vertexPointer++;
			}
		}

		cout << "VERTEXPOINTER:" << vertices.size() << endl;
		int pointer = 0;
		for (int gz = 0; gz < VERTEX_COUNT - 1; gz++)
		{
			for (int gx = 0; gx < VERTEX_COUNT - 1; gx++)
			{
				int topLeft = (gz * VERTEX_COUNT) + gx;
				int topRight = topLeft + 1;
				int bottomLeft = ((gz + 1) * VERTEX_COUNT) + gx;
				int bottomRight = bottomLeft + 1;
				indices[pointer++] = topLeft;
				indices[pointer++] = bottomLeft;
				indices[pointer++] = topRight;
				indices[pointer++] = topRight;
				indices[pointer++] = bottomLeft;
				indices[pointer++] = bottomRight;
			}
		}


		glGenVertexArrays(1, &terrainVAO);
		glBindVertexArray(terrainVAO);

		glGenBuffers(1, &terrainVBO);
		glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);


		glGenBuffers(1, &terrainEBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrainEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), indices.data(), GL_STATIC_DRAW);

		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		glGenBuffers(1, &texVBO);
		glBindBuffer(GL_ARRAY_BUFFER, texVBO);

		glBufferData(GL_ARRAY_BUFFER, textureCoords.size() * sizeof(float), textureCoords.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);


		glGenBuffers(1, &normalVBO);
		glBindBuffer(GL_ARRAY_BUFFER, normalVBO);

		glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), normals.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(2);


		indicesLen = indices.size();
		cout << "INDEX LENGTH" << indicesLen << endl;

		return terrainVAO;
	}

	


};