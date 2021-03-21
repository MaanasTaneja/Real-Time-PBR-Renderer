#pragma once



class Terrain
{
public:

	Loader loader;
	HeightsGenerator generate;
	 float SIZE = 800.0f;
	 int VERTEX_COUNT = 128;

	 float heights[128][128];

	 unsigned int terrainVAO;
	 unsigned int terrainVBO;
	 unsigned int vertexVBO;
	 unsigned int normalVBO;
	 unsigned int texVBO;
	 unsigned int terrainEBO;
	 unsigned int indicesLen;

	 float x;     //x pos of vertices lol
	 float z;     //y pos of vericessss lol [taking grid squarefs in jdbckdfjb vrk]

	 unsigned int terrainTexture;
	 unsigned int normalMap;
	 unsigned int metallicMap;
	 unsigned int roughnessMap;
	 unsigned int aoMap;

	 unsigned int blend;
	 unsigned int gBlend;
	 unsigned int rBlend;
	 unsigned int bBlend;

	 unsigned int defaultN;


	Terrain(int gridX, int gridZ, unsigned int terrainTex)
	{
		Texture norm;
		Texture mett;
		Texture rough;
		Texture ao;

		Texture blendmap;
		Texture rblend;
		Texture gblend;
		Texture bblend;

		Texture default_n;

		 generateTerrain();
		

		terrainTexture = terrainTex;

		normalMap = norm.getTexID("resources/textures/pbr/grass/normal.png");
		metallicMap = mett.getTexID("resources/textures/pbr/grass/metallic.png");
		roughnessMap = rough.getTexID("resources/textures/pbr/grass/roughness.jpg");
		aoMap = ao.getTexID("resources/textures/pbr/grass/ao.jpg");

		blend = blendmap.getTexID("resources/textures/blendMap.png");
		rBlend = rblend.getTexID("resources/textures/mud.png");
		gBlend = gblend.getTexID("resources/textures/grassFlowers.png");
		bBlend = bblend.getTexID("resources/textures/path.png");

		defaultN = default_n.getTexID("resources/textures/normal.jpg");

		x = gridX * SIZE;
		z = gridZ * SIZE;
	}

	float getX() {
		return x;
	}

	float getZ() {
		return z;
	}

	   unsigned int generateTerrain() 
	   {

		   int count = VERTEX_COUNT * VERTEX_COUNT;
		   std::vector<glm::vec3> vertices(count * 3), normals(count * 3);
		   std::vector<glm::vec2> textureCoords(count* 3);
		   std::vector<int> indices(6 * (VERTEX_COUNT - 1) * VERTEX_COUNT);


		int vertexPointer = 0;
		for (int i = 0; i < VERTEX_COUNT; i++)
		{
			for (int j = 0; j < VERTEX_COUNT; j++)
			{
				float height = getHeight(j, i, generate);
		
				heights[j][i] = height;
				vertices[vertexPointer] = glm::vec3((float)j / ((float)VERTEX_COUNT - 1) * SIZE, height ,(float)i / ((float)VERTEX_COUNT - 1) * SIZE);
				normals[vertexPointer] = calculateNormals(j , i , generate) ;
				textureCoords[vertexPointer] = glm::vec2((float)j / ((float)VERTEX_COUNT - 1) , (float)i / ((float)VERTEX_COUNT - 1));
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
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int) , indices.data() , GL_STATIC_DRAW);

		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float) , vertices.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0 , 0);
		glEnableVertexAttribArray(0);

		glGenBuffers(1, &texVBO);
		glBindBuffer(GL_ARRAY_BUFFER, texVBO);

		glBufferData(GL_ARRAY_BUFFER, textureCoords.size() * sizeof(float), textureCoords.data(), GL_STATIC_DRAW);
		
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);


		glGenBuffers(1, &normalVBO);
		glBindBuffer(GL_ARRAY_BUFFER, normalVBO);

		glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), normals.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE,0, 0);
		glEnableVertexAttribArray(2);

		
		indicesLen = indices.size();
		cout << "INDEX LENGTH" << indicesLen << endl;

		 return terrainVAO;
	}


	    float getHeightOfTerrain(float worldX, float worldZ) {
		   float terrainX = worldX - x;
		   float terrainZ = worldZ - z;
		   float gridSquareSize = SIZE / ((float)sizeof(heights) / sizeof(heights[0]) - 1);
		   int gridX = (int)floor(terrainX / gridSquareSize);
		   int gridZ = (int)floor(terrainZ / gridSquareSize);
		   if (gridX >= sizeof(heights) / sizeof(heights[0]) - 1 || gridZ >= sizeof(heights) / sizeof(heights[0]) - 1 || gridX < 0 || gridZ < 0) {
			   return 0;
		   }
		   float xCoord = fmod(terrainX, gridSquareSize) / gridSquareSize;
			   //(terrainX % gridSquareSize) / gridSquareSize;
		   float zCoord = fmod(terrainX, gridSquareSize) / gridSquareSize;
			   //(terrainZ % gridSquareSize) / gridSquareSize;
		   float answer;
		   if (xCoord <= (1 - zCoord)) {
			   answer = barryCentric(glm::vec3(0, heights[gridX][gridZ], 0), glm::vec3(1, heights[gridX + 1][gridZ], 0), glm::vec3(0, heights[gridX][gridZ + 1], 1), glm::vec2(xCoord, zCoord));
		   }
		   else {
			   answer = barryCentric(glm::vec3(1, heights[gridX + 1][gridZ], 0), glm::vec3(1, heights[gridX + 1][gridZ + 1], 1), glm::vec3(0, heights[gridX][gridZ + 1], 1), glm::vec2(xCoord, zCoord));
		   }
		   
		   return answer;
	   }


	   private:

		   float getHeight(int x, int z, HeightsGenerator gen)
		   {
			   
			   return gen.generateHeight(x, z);
		   }

		   glm::vec3 calculateNormals(int x, int z, HeightsGenerator geny)
		   {
			   float heightL = getHeight(x-1, z, geny);
				   float heightR = getHeight(x + 1, z, geny);
				   float heightD = getHeight(x, z-1, geny);
				   float heightU = getHeight(x , z+1, geny);

				   glm::vec3 normal = glm::vec3(heightL - heightR, 2.0f, heightD - heightU);
				   glm::normalize(normal);
				   return normal;

		   }

		    float barryCentric(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec2 pos) {
			   float det = (p2.z - p3.z) * (p1.x - p3.x) + (p3.x - p2.x) * (p1.z - p3.z);
			   float l1 = ((p2.z - p3.z) * (pos.x - p3.x) + (p3.x - p2.x) * (pos.y - p3.z)) / det;
			   float l2 = ((p3.z - p1.z) * (pos.x - p3.x) + (p1.x - p3.x) * (pos.y - p3.z)) / det;
			   float l3 = 1.0f - l1 - l2;
			   return l1 * p1.y + l2 * p2.y + l3 * p3.y;
		   }

		


		   float cubeVertices[320] = {
			   // positions           
				10.0f, -0.5f,  10.0f, 
			   -10.0f, -0.5f,  10.0f, 
			   -10.0f, -0.5f, -10.0f, 

				10.0f, -0.5f,  10.0f, 
			   -10.0f, -0.5f, -10.0f, 
				10.0f, -0.5f, -10.0f, 
		   };



};
