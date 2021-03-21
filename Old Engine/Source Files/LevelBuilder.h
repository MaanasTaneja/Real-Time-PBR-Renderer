#pragma once

class LevelBuilder
{
public:
	fstream file;
	string levelFile;
	int numOfStaticEnts;
	int numOfdynamicEnts;
	int numOfLevels;

	int numOfLights;
	int numOfInteractables;
	int numOfNPCS;

	vector<string> fileData;
	vector<glm::vec3> objectPositions;
	vector<Model> objectModels;

	vector<glm::vec3> lightPositions;
	Camera* LevelCamera;
	MasterRenderer* buildRenderer;

	MasterRenderer* buildLevelFromFile(string fileN)
	{
		levelFile = fileN;
		loadFile(fileN);

		buildRenderer = new MasterRenderer(glm::vec3(-0.2f, -1.0f, -0.3f), glm::vec3(10.0f, 10.0f, 10.0f), PE_CYCLES);
		buildRenderer->initRenderers();

		vector<string> linedata;

		for (int i = 0; i < fileData.size(); i++)
		{
			std::string s = fileData.at(i);
			std::string delimiter = ";";

			size_t pos = 0;
			std::string token;
			while ((pos = s.find(delimiter)) != std::string::npos) {
				token = s.substr(0, pos);

				linedata.push_back(token);
				s.erase(0, pos + delimiter.length());
			}

		}

		for (int j = 0; j < linedata.size(); j++)
		{
			string word = linedata.at(j);

			if (word.compare("Level") == 0)
			{
				numOfLevels++;
				Model* modelEnt;
				modelEnt = new Model(linedata.at(j + 1));

				float x = stof(linedata.at(j + 2));
				float y = stof(linedata.at(j + 3));
				float z = stof(linedata.at(j + 4));
				float mass = stof(linedata.at(j + 5));
				float scale = stof(linedata.at(j + 6));
				float rot = stof(linedata.at(j + 7));

				Entity* entity = new Entity(modelEnt, glm::vec3(x, y, z), glm::vec3(scale), rot, true);
				entity->addExtraMaterialComponents(0.4f, 0.3f);
				entity->isLevel = true;

				buildRenderer->addEntities(entity);
			}

			if (word.compare("Player") == 0)
			{
				float x = stof(linedata.at(j + 2));
				float y = stof(linedata.at(j + 3));
				float z = stof(linedata.at(j + 4));

				Player* PLAYER;
				PLAYER = new Player(glm::vec3(x, y, z), LevelCamera, buildRenderer->PhysicsWorld.world);
				buildRenderer->addPlayer(PLAYER);
				
			}

			if (word.compare("Skybox") == 0)
			{
				string hdrPath = linedata.at(j + 1);
				Skybox* sky;
				sky = new Skybox();
				sky->attachHDRMap(hdrPath);


				buildRenderer->addSkybox(sky);
			}

			if (word.compare("Entity") == 0)
			{
				numOfdynamicEnts++;
				Model* modelEnt;

				if (previousModel == nullptr)
				{
					modelEnt = new Model(linedata.at(j + 1));
				}

				else if (previousModel->name.compare(linedata.at(j + 1)) == 0)
				{
					modelEnt = previousModel;
                  }
			
				else {
					modelEnt = new Model(linedata.at(j + 1));
				}

					float x = stof(linedata.at(j + 2));
					float y = stof(linedata.at(j + 3));
					float z = stof(linedata.at(j + 4));
					float mass = stof(linedata.at(j + 5));
					float scale = stof(linedata.at(j + 6));
					float rot = stof(linedata.at(j + 7));
				

				Entity* entity = new Entity(modelEnt, glm::vec3(x, y, z), glm::vec3(scale), rot, mass);
				previousModel = modelEnt;
				buildRenderer->addEntities(entity);
			}

			if (word.compare("Light") == 0)
			{
				numOfLights++;
				float x = stof(linedata.at(j + 1));
				float y = stof(linedata.at(j + 2));
				float z = stof(linedata.at(j + 3));
				float colX = stof(linedata.at(j + 4));
				float colY = stof(linedata.at(j + 5));
				float colZ = stof(linedata.at(j + 6));

				buildRenderer->addLight(glm::vec3(x, y, z) , glm::vec3(colX, colY, colZ));
			}

		}

		return buildRenderer;
	}

	void writeToLevelFile(Entity *ent)
	{
		fstream file;
		file.open(levelFile, ios::out | ios::app);
		
		float x = ent->position.x;
		float y = ent->position.y;
		float z = ent->position.z;

		float scale = ent->scale.x;
		float mass = ent->entityRigidBody->getMass();

		string modelN = ent->modelEnt->name + ";";


		string x_ = to_string(x) + ";";
		string y_ = to_string(y) + ";";
		string z_ = to_string(z) + ";";

		string scale_ = to_string(scale) +";";
		string mass_ = to_string(mass)+ ";";

		string finalLine = string("Entity;") + modelN + x_+ y_ + z_ + mass_ + scale_ + "0.0;" ;


		file <<endl<< finalLine;
	
	}

	void writeToLevelFile(Light ent)
	{
		fstream file;
		file.open(levelFile, ios::out | ios::app);

		float x = ent.pos.x;
		float y = ent.pos.y;
		float z = ent.pos.z;

		float colx = ent.col.x;
		float coly = ent.col.y;
		float colz = ent.col.z;

		

		string x_ = to_string(x) + ";";
		string y_ = to_string(y) + ";";
		string z_ = to_string(z) + ";";
        
		string colx_ = to_string(colx) + ";";
		string coly_ = to_string(coly) + ";";
		string colz_ = to_string(colz) + ";";

		string finalLine = string("Light;")+ x_ + y_ + z_ + colx_ + coly_ + colz_ + ";";


		file << endl << finalLine;
		
	}

private:

	void loadFile(string filName)
	{
		file.open(levelFile, ios::in | ios::out | ios::app);

		string str;
		while (getline(file, str))
		{
			if (str.size() > 0)
			{
				fileData.push_back(str);
			}
			str = "";
		}
	}
	Model* previousModel;
	
};