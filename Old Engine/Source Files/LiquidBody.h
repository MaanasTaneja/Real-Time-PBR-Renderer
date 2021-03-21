#pragma once


class LiquidBody
{
public:
	glm::vec3 position;
	glm::vec3 flowDirection;
	glm::quat zRotation;
	int gridX_;
	int gridZ_;

	float x_pos;
	float z_pos;

	int INDEX_COUNT;

	bool defaultMesh = false;
	bool modelMesh = false;
	bool isFlowing = false;
	bool isStagnant = true;

	bool generic_shader;
	bool realistic_shader;
	bool fluid_sim_shader;

	Texture liquidTexture;
	unsigned int liquidMeshVAO;
	Model* liquidModel;

	float tileFactor;
	float waveSpeedFactor;
	float alphaFactor;
	bool additive;


	void generateTileLiquidMesh(int gridX, int gridZ, float SIZE, int VERTEX_COUNT)
	{
		gridX_ = gridX;
		gridZ_ = gridZ;
		generic_shader = true;
		defaultMesh = true;
		int count = VERTEX_COUNT * VERTEX_COUNT;
		std::vector<glm::vec3> vertices(count * 3), normals(count * 3);
		std::vector<glm::vec2> textureCoords(count * 3);
		std::vector<int> indices(6 * (VERTEX_COUNT - 1) * VERTEX_COUNT);


		int vertexPointer = 0;
		for (int i = 0; i < VERTEX_COUNT; i++)
		{
			for (int j = 0; j < VERTEX_COUNT; j++)
			{
				vertices[vertexPointer] = glm::vec3((float)j / ((float)VERTEX_COUNT - 1) * SIZE, 0.0f, (float)i / ((float)VERTEX_COUNT - 1) * SIZE);
				normals[vertexPointer] = glm::vec3(0.0f, 1.0f, 0.0f);
				textureCoords[vertexPointer] = glm::vec2((float)j / ((float)VERTEX_COUNT - 1), (float)i / ((float)VERTEX_COUNT - 1));
				vertexPointer++;
			}
		}

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

		INDEX_COUNT = indices.size();

		cout << "LiquidMeshGen Complete" << endl;

		unsigned int verticesVBO, normalsVBO, textureCoordsVBO, indicesEBO;

		glGenVertexArrays(1, &liquidMeshVAO);
		glBindVertexArray(liquidMeshVAO);

		glGenBuffers(1, &indicesEBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), indices.data(), GL_STATIC_DRAW);

		glGenBuffers(1, &verticesVBO);
		glBindBuffer(GL_ARRAY_BUFFER, verticesVBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float) , vertices.data() , GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		glGenBuffers(1, &textureCoordsVBO);
		glBindBuffer(GL_ARRAY_BUFFER, textureCoordsVBO);
		glBufferData(GL_ARRAY_BUFFER, textureCoords.size() * sizeof(float), textureCoords.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);

		glGenBuffers(1, &normalsVBO);
		glBindBuffer(GL_ARRAY_BUFFER, normalsVBO);

		glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), normals.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(2);

		glBindVertexArray(0);

		x_pos = gridX_ * SIZE; 
		z_pos = gridZ_ * SIZE;
	}

	void addLiquidTexture(string name)
	{
		liquidTexture.getTexID(name);
	}

	void makeLiquidFromMesh(Model* model)
	{
		generic_shader = true;
		modelMesh = true;
		liquidModel = model;
	}

	void flowLiquid(glm::vec3 direction)
	{
		isFlowing = true;
		isStagnant = false;
		flowDirection = direction;
	}

	void fluid_simulation()
	{
		fluid_sim_shader = true;
	}

	void setTileFactor(float fact)
	{
		tileFactor = fact;
	}

	void setAlpha(float fact)
	{
		alphaFactor = fact;
	}

	void setWaveSpeed(float fact)
	{
		waveSpeedFactor = fact;
	}

	void setAdditive(bool fact)
	{
		additive = fact;
	}

	int getX()
	{
		return x_pos;
	}

	int getZ()
	{
		return z_pos;
	}
};