#pragma once

#define RADIOSITY_TEXTURE_SIZE 32

void preprocessModel(Model& objectModel, Shader& preprocessShader, glm::mat4 objectModelMatrix)
{
	unsigned int preprocessFramebuffer;
	unsigned int preprocessRenderbuffer;

	unsigned int worldspacePosData;
	unsigned int worldspaceNormalData;

	unsigned int idData;
	unsigned int uvData;

	glGenTextures(1, &worldspacePosData);
	glBindTexture(GL_TEXTURE_2D, worldspacePosData);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, RADIOSITY_TEXTURE_SIZE, RADIOSITY_TEXTURE_SIZE, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);  //NEAREST so as to remove any texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	

	glGenTextures(1, &worldspaceNormalData);
	glBindTexture(GL_TEXTURE_2D, worldspaceNormalData);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, RADIOSITY_TEXTURE_SIZE, RADIOSITY_TEXTURE_SIZE, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	glGenTextures(1, &idData);
	glBindTexture(GL_TEXTURE_2D, idData);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, RADIOSITY_TEXTURE_SIZE, RADIOSITY_TEXTURE_SIZE, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	glGenTextures(1, &uvData);
	glBindTexture(GL_TEXTURE_2D, uvData);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, RADIOSITY_TEXTURE_SIZE, RADIOSITY_TEXTURE_SIZE, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	glGenFramebuffers(1, &preprocessFramebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, preprocessFramebuffer);

	//Attaching all Color Buffers

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, worldspacePosData, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, worldspaceNormalData, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, idData, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, uvData, 0);

	glGenRenderbuffers(1, &preprocessRenderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, preprocessRenderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, RADIOSITY_TEXTURE_SIZE, RADIOSITY_TEXTURE_SIZE);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, preprocessRenderbuffer);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Radiosty Preprocess Framebuffer isn't complete" << std::endl;
	}

	unsigned int attachments[] = { worldspacePosData, worldspaceNormalData, idData, uvData };
	glDrawBuffers(4, attachments);

	glViewport(0, 0, RADIOSITY_TEXTURE_SIZE, RADIOSITY_TEXTURE_SIZE);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	for (int i = 0; i < objectModel.meshes.size(); i++)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		preprocessShader.use();

		std::vector<GLfloat> worldspacePositionDataBuffer(RADIOSITY_TEXTURE_SIZE * RADIOSITY_TEXTURE_SIZE * 3);
		std::vector<GLfloat> normalVectorDataBuffer(RADIOSITY_TEXTURE_SIZE * RADIOSITY_TEXTURE_SIZE * 3);

		std::vector<GLfloat> idDataBuffer(RADIOSITY_TEXTURE_SIZE * RADIOSITY_TEXTURE_SIZE * 3);
		std::vector<GLfloat> uvDataBuffer(RADIOSITY_TEXTURE_SIZE * RADIOSITY_TEXTURE_SIZE * 3);

		preprocessShader.setMat4("model", objectModelMatrix);

		objectModel.meshes.at(i).Draw(&preprocessShader, false);

		glReadBuffer(GL_COLOR_ATTACHMENT0);
		glReadPixels(0, 0, RADIOSITY_TEXTURE_SIZE, RADIOSITY_TEXTURE_SIZE, GL_RGB, GL_FLOAT, &worldspacePositionDataBuffer[0]);   //Stroe data of color attachment 0 in buffer

		glReadBuffer(GL_COLOR_ATTACHMENT1);
		glReadPixels(0, 0, RADIOSITY_TEXTURE_SIZE, RADIOSITY_TEXTURE_SIZE, GL_RGB, GL_FLOAT, &idDataBuffer[0]);

		glReadBuffer(GL_COLOR_ATTACHMENT2);
		glReadPixels(0, 0, RADIOSITY_TEXTURE_SIZE, RADIOSITY_TEXTURE_SIZE, GL_RGB, GL_FLOAT, &normalVectorDataBuffer[0]);

		glReadBuffer(GL_COLOR_ATTACHMENT3);
		glReadPixels(0, 0, RADIOSITY_TEXTURE_SIZE, RADIOSITY_TEXTURE_SIZE, GL_RGB, GL_FLOAT, &uvDataBuffer[0]);

		objectModel.meshes.at(i).worldspacePosData = worldspacePositionDataBuffer;
		objectModel.meshes.at(i).worldspaceNormalData = normalVectorDataBuffer;
		objectModel.meshes.at(i).idData = idDataBuffer;
		objectModel.meshes.at(i).uvData = uvDataBuffer;
		


		for (int j = 0; j < objectModel.meshes.size(); j += 3)
		{
			float redIDValue = objectModel.meshes[i].idData[j];
			float greenIDValue = objectModel.meshes[i].idData[j + 1];
			float blueIDValue = objectModel.meshes[i].idData[j + 2];

			float idSum = redIDValue + greenIDValue + blueIDValue;

			if (idSum > 0) {
				objectModel.meshes[i].texturespaceShooterIndices.push_back(j);
			}

			objectModel.meshes.at(i).texelArea = 1.0f;

		}


	}


	glUseProgram(0);

	glDeleteTextures(1, &worldspacePosData);
	glDeleteTextures(1, &worldspaceNormalData);
	glDeleteTextures(1, &idData);
	glDeleteTextures(1, &uvData);

	glReadBuffer(GL_COLOR_ATTACHMENT0);
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}