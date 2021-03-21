#pragma once


const unsigned int SCR_WIDTH1 = 1280;
const unsigned int SCR_HEIGHT1 = 720;


class MasterRenderer
{
public:

	int scrollCount=0;
	vector<Light> lights;
	Light lights_[100];
	int nLights = 0;
	glm::vec3 sunDir;
	glm::vec3 sunColor;


	Shader* entityShader;
	Shader* terrainShader;
	Shader* skyboxShader;
	Shader* waterShader;
	Shader* lightShader;
	Shader* debugShader;
	Shader* particleShader;
	Shader* liquidShader1;

	EntityRenderer Ent_Render;
	SoftBodyRenderer softEnt_Render;
	TerrainRenderer Terr_Render;
	PostProcesser postprocess;
	SkyboxRenderer skybox;
	//AreaLightsRenderer areaLight_Render;
	PlayerRenderer player;
	GunContainer guncontainer;
	LightRenderer lightCubeDraw;
	Physics PhysicsWorld;
	BulletDebugDrawer DebugDraw;
	LineRenderer LineRender;
	ParticleMaster particleMaster;
	LiquidBodyRenderer liquidRenderer;
	
	glm::vec3 rayPoint;

	bool doReflectionPass = false;
	bool doRefractionPass = false;
	bool doShadowmapPass = false;


	
	MasterRenderer(glm::vec3 sunDirection,glm::vec3 sunCol,  int rEngine)
	{
		
		sunDir = sunDirection;
		sunColor = sunCol;
		if (rEngine == PE_CYCLES)
		{
			entityShader = new Shader("entityVertex.vs", "entityFragment.fs");
			terrainShader = new Shader("terrainVertex.vs", "terrainFragment.fs");
			skyboxShader = new Shader("skyboxVertex.vs", "skyboxFragment.fs");
			waterShader = new Shader("waterVertex.vs", "waterFragment.fs");
			lightShader = new Shader("lightVertex.vs", "lightFragment.fs");
			debugShader = new Shader("universalDebugShader.vs", "universalDebugShader.fs");
			particleShader = new Shader("particleShader.vs", "particleShader.fs");
			liquidShader1 = new Shader("liquidShader_dusk.vs", "liquidShader_dusk.fs");
			
		}

		if (rEngine == PE_EEVE)
		{
			entityShader = new Shader("entityVertex_EVEE.vs", "entityFragment_EVEE.fs");
			terrainShader = new Shader("terrainVertex.vs", "terrainFragment.fs");
			skyboxShader = new Shader("skyboxVertex.vs", "skyboxFragment.fs");
			waterShader = new Shader("waterVertex.vs", "waterFragment.fs");
			lightShader = new Shader("lightVertex.vs", "lightFragment.fs");
			debugShader = new Shader("universalDebugShader.vs", "universalDebugShader.fs");
			particleShader = new Shader("particleShader.vs", "particleShader.fs");
			liquidShader1 = new Shader("liquidShader_dusk.vs", "liquidShader_dusk.fs");
		}
	}

	void addEntities(Entity* ENT)
	{
		Ent_Render.addEntity(ENT);
		if (ENT->entityRigidBody != nullptr)
		{
			ENT->entityRigidBody->setUserPointer(ENT);
			PhysicsWorld.world->addRigidBody(ENT->entityRigidBody);
		}
		else {

		}

	}

	void addSoftEntity(btSoftBody* bdy)
	{
		softEnt_Render.addSoftBody(bdy);
		PhysicsWorld.world->addSoftBody(bdy);
	}

	void addTerrain(Terrain terrain)
	{
		Terr_Render.addTerrain(terrain);
	}

	void addSkybox(Skybox* sky)
	{
		skybox.addSkybox(sky);

	}

	void addLiquid(LiquidBody* liquid)
	{
		liquidRenderer.addLiquidBody(liquid);
	}


	void addPlayer(Player* playerr)
	{
		player.addPlayer(playerr);
		playerr->playerRigidBody->setUserPointer(playerr);
		PhysicsWorld.world->addRigidBody(playerr->playerRigidBody);
	}

	void addGun(Gun* gun)
	{
		guncontainer.addGun(gun);
		gun->gunBody->setUserPointer(gun);
		PhysicsWorld.world->addRigidBody(gun->gunBody);
	}

	void addLight(glm::vec3 positions , glm::vec3 colors)
	{
		lights_[nLights].pos = positions;
		lights_[nLights].col = colors;
		//lights_[nLights].addDebugLamp();

		lights.push_back(lights_[nLights]);
		nLights = nLights + 1;
	}

	void addParticleSystem(ParticleSystem* system)
	{
		particleMaster.addParticleSystem(system);
	}

	void addDebugEntity(Model* ent, int counter)
	{
		currentDebug = ent;
		scrollCount = counter;
	}

	void initRenderers()
	{
		Ent_Render.init(entityShader);
		Terr_Render.init(terrainShader);
		skybox.init(skyboxShader);
		player.init(entityShader);

		guncontainer.init(entityShader, &Ent_Render);
		guncontainer.particleHandler = &particleMaster;

		lightCubeDraw.init(lightShader);
		lightCubeDraw.cube();
		PhysicsWorld.initializeRigidWorld();
		PhysicsWorld.assignDebugDrawer(&DebugDraw);
		LineRender.init();
		postprocess.initPostProcesser();
		postprocess.attachPostshader("post.vs", "post.fs");
		particleMaster.init(particleShader);
		liquidRenderer.init(liquidShader1);

		initializeReflectionFBO();
	}

	void loadClipPlane(glm::vec4 planePos)
	{
		entityShader->use();
		entityShader->setVec4("clipPlane" , planePos);
		glUseProgram(0);
	}

	void renderLights(Camera cam , glm::mat4 proj)
	{
		Ent_Render.getShader()->use();
		Ent_Render.getShader()->setInt("nLights", nLights);
		/* FOR PHONG
		Ent_Render.getShader()->setVec3("lightAmbient", glm::vec3(0.4f, 0.4f, 0.4f));
		Ent_Render.getShader()->setVec3("lightDiffuse", glm::vec3(0.5f, 0.5f, 0.5f));
		Ent_Render.getShader()->setVec3("lightSpecular", glm::vec3(1.0f, 1.0f, 1.0f));
		*/

		Ent_Render.getShader()->setVec3("viewPos", cam.Position);
		Ent_Render.getShader()->setVec3("sunDir", sunDir);
		Ent_Render.getShader()->setVec3("sunColor", sunColor);


		Ent_Render.getShader()->setInt("albedoMap", 0); 
		Ent_Render.getShader()->setInt("normalMap",1);
		Ent_Render.getShader()->setInt("metallicMap", 2);
		Ent_Render.getShader()->setInt("roughnessMap", 3);
		Ent_Render.getShader()->setInt("aoMap", 4);
		Ent_Render.getShader()->setInt("irradianceMap", 5);
		Ent_Render.getShader()->setInt("prefilterMap", 6);
		Ent_Render.getShader()->setInt("brdfLUT", 7);

		Ent_Render.getShader()->setInt("envMap", 9);

		
		skybox.getShader()->use();
		skybox.getShader()->setVec3("fogColor", glm::vec3(1.0f, 1.0f, 1.0f));


		for (int i = 0; i < lights.size(); i++)
		{
			Ent_Render.getShader()->use();
			Ent_Render.getShader()->setVec3("lightPos["+ std::to_string(i) + "]", lights.at(i).pos);
			Ent_Render.getShader()->setVec3("lightColors[" + std::to_string(i) + "]", lights.at(i).col);

		}

		for (int i = 0; i < lights.size(); i++)
		{
			guncontainer.getShader()->use();
			guncontainer.getShader()->setVec3("lightPos[" + std::to_string(i) + "]", lights.at(i).pos);
			guncontainer.getShader()->setVec3("lightColors[" + std::to_string(i) + "]", lights.at(i).col);

		}
		
	}	

	void bindPrecomputedData()
	{
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.skybox->irradianceMap);
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.skybox->prefilterMap);
		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_2D, skybox.skybox->brdfLUTTexture);
	}

	void hotreloadShaders(int rEngine)
	{
		cout << "HOTRELOAD::SHADERS::DELETED" << endl;
		delete entityShader;
		delete terrainShader;
		delete skyboxShader;
		delete waterShader;
		delete lightShader;

		cout << "HOTRELOAD::SHADERS::INITIALIZED" << endl;
		if(rEngine == PE_CYCLES)
		{
			entityShader = new Shader("entityVertex.vs", "entityFragment.fs");
		}
		else
		{
			entityShader = new Shader("entityVertex_EVEE.vs", "entityFragment_EVEE.fs");
		}

		terrainShader = new Shader("terrainVertex.vs", "terrainFragment.fs");
		skyboxShader = new Shader("skyboxVertex.vs", "skyboxFragment.fs");
		waterShader = new Shader("waterVertex.vs", "waterFragment.fs");
		lightShader = new Shader("lightVertex.vs", "lightFragment.fs");
		liquidShader1 = new Shader("liquidShader.vs", "liquidShader.fs");
		initRenderers();
	}

	void updatePhysics_internal(float timestep)
	{
		PhysicsWorld.world->stepSimulation(deltaTime, 10, timestep);
	}

	void render_internal(Camera camera1, bool tickForward)
	{
	
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glEnable(GL_CULL_FACE);
		
		glm::mat4 projection = glm::perspective(glm::radians(camera1.Zoom), (float)SCR_WIDTH1 / (float)SCR_HEIGHT1, 0.1f, 100.0f);
		glm::mat4 view = camera1.GetViewMatrix();
		LineRender.updateProjections(projection, view);

	  
		glEnable(GL_DEPTH_TEST);
		
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		LineRender.renderLines();
		//DebugDraw.SetMatrices(view, projection);
		//bindPrecomputedData();
		Ent_Render.irradiance = skybox.skybox->irradianceMap;
	    //debugEntityUpdate(rayPoint, projection, view);
		Ent_Render.renderEntities(projection, view , camera1);
		particleMaster.renderAllParticleSystems(projection, view, camera1);
		renderLights(camera1 , projection);

		skybox.renderSkybox(projection, view, camera1);
		
		player.renderPlayer(projection, view, camera1);
		liquidRenderer.renderLiquids(projection, view, camera1);
		guncontainer.renderGuns(projection, view, camera1);
	   // PhysicsWorld.world->debugDrawWorld();

		if (tickForward == true)
		{
			updatePhysics_internal(1. / 120.);
		}
	}

	void render_hull(Camera camera1)
	{
	
		glEnable(GL_CULL_FACE);

		glm::mat4 projection = glm::perspective(glm::radians(camera1.Zoom), (float)SCR_WIDTH1 / (float)SCR_HEIGHT1, 0.1f, 100.0f);
		glm::mat4 view = camera1.GetViewMatrix();
		

		glEnable(GL_DEPTH_TEST);

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		bindPrecomputedData();
		Ent_Render.irradiance = skybox.skybox->irradianceMap;
		Ent_Render.renderEntities(projection, view, camera1);
		renderLights(camera1, projection);
		skybox.renderSkybox(projection, view, camera1);

		
	}
	

	void render(Camera camera1)
	{
		glEnable(GL_CLIP_DISTANCE0);

		if (doReflectionPass == true)
		{
			//Reflection Pass
		}

		if (doRefractionPass == true)
		{
			//Refraction Pass
		}

		if (doShadowmapPass == true)
		{
			//Light-Space ShadowMap Pass
		}

		
		postprocess.begin();
		render_internal(camera1, true);
		postprocess.end();
		postprocess.drawPostQuad();

		
	}


	void setupStaticUniforms(glm::mat4 projection , glm::mat4 view)
	{
		

	}

	private:

		unsigned int reflectionFBO;
		unsigned int reflectionRBO;
		unsigned int refractionFBO;
		unsigned int refractionRBO;
		unsigned int shadowMapFBO;

		unsigned int reflectionTexture;
		unsigned int refractionTexture;
		unsigned int shadowMapTexture;


		void initializeReflectionFBO()
		{
		
			glGenTextures(1, &reflectionTexture);
			glBindTexture(GL_TEXTURE_2D, reflectionTexture);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1280, 720, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glGenFramebuffers(1, &reflectionFBO);
			glBindFramebuffer(GL_FRAMEBUFFER, reflectionFBO);
			glBindTexture(GL_TEXTURE_2D, reflectionTexture);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, reflectionTexture, 0);

			glGenRenderbuffers(1, &reflectionRBO);
			glBindRenderbuffer(GL_RENDERBUFFER, reflectionRBO);
				glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1280, 720);
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, reflectionRBO); \

				glDrawBuffer(GL_COLOR_ATTACHMENT0);

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				cout << "ERROR::FRAMEBUFFER:: Reflection Framebuffer is not complete!" << endl;

		}


		void reflectionPass(glm::mat4 view)
		{

		}
		

		void initializeRefractionFBO()
		{
			float anisotropy = 0.0f;
			glGetFloatv(GL_TEXTURE_MAX_ANISOTROPY_EXT, &anisotropy);

			glGenTextures(1, &refractionTexture);
			glBindTexture(GL_TEXTURE_2D, refractionTexture);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1280, 720, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_ARB_texture_filter_anisotropic);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_ARB_texture_filter_anisotropic);
			glTexParameterf(GL_TEXTURE_2D, GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, anisotropy);

			glGenFramebuffers(1, &refractionFBO);
			glBindFramebuffer(GL_FRAMEBUFFER, refractionFBO);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, refractionTexture, 0);

			glGenRenderbuffers(1, &refractionRBO);
			glBindRenderbuffer(GL_RENDERBUFFER, refractionRBO);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1280, 720);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, refractionRBO);

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				cout << "ERROR::FRAMEBUFFER:: Refraction Framebuffer is not complete!" << endl;

		}













		float lastFrame = 0.0f;
		float deltaTime = 0.0f;

		float test = 0.0f;

		Model* currentDebug;
		
		void debugEntityUpdate(glm::vec3 rayHit, glm::mat4 projection , glm::mat4 view)
		{

	        glm::mat4 debugModelMatrix = glm::mat4(1.0f);

			debugShader->use();
			debugShader->setMat4("projection", projection);
			debugShader->setMat4("view", view);

			debugModelMatrix = glm::translate(debugModelMatrix, glm::vec3(rayHit));
			if (scrollCount == 0)
			{
				debugModelMatrix = glm::scale(debugModelMatrix, glm::vec3(0.2f));
			}
			else if (scrollCount == 1)
			{
				debugModelMatrix = glm::scale(debugModelMatrix, glm::vec3(0.004f));
			}
			else if(scrollCount ==2) {
				debugModelMatrix = glm::scale(debugModelMatrix, glm::vec3(0.15f));
			}
		
			
			debugShader->setMat4("model", debugModelMatrix);

			currentDebug->Draw(debugShader);
			
			
		}

		void debugEntitySetup()
			{
		
				debugShader = new Shader("universalDebugShader.vs", "universalDebugShader.fs");
			}
	


	
};
