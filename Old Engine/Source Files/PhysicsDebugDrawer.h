#pragma once

Shader* debugDraw;
class BulletDebugDrawer : public btIDebugDraw {
public:

	unsigned int VAO, VBO;

	void Init()
	{
		debugDraw = new Shader("debugPhysics.vs", "debugPhysics.fs");
	}


	void SetMatrices(glm::mat4 pViewMatrix, glm::mat4 pProjectionMatrix) {

		debugDraw->use();
		debugDraw->setMat4("projection", pProjectionMatrix);
		debugDraw->setMat4("view", pViewMatrix);

	}

	virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color){
		GLfloat points[12];

		points[0] = from.x();
		points[1] = from.y();
		points[2] = from.z();
		points[3] = color.x();
		points[4] = color.y();
		points[5] = color.z();


		points[6] = to.x();
		points[7] = to.y();
		points[8] = to.z();
		points[9] = color.x();
		points[10] = color.y();
		points[11] = color.z();

		glDeleteBuffers(1, &VBO);
		glDeleteVertexArrays(1, &VAO);

		glGenBuffers(1, &VBO);
		glGenVertexArrays(1, &VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(points), &points, GL_STATIC_DRAW);
		glBindVertexArray(VAO);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glBindVertexArray(0);

		glBindVertexArray(VAO);

		debugDraw->use();
		

		glDrawArrays(GL_LINES, 0, 2);
		glBindVertexArray(0);

	}


	virtual void drawContactPoint(const btVector3 &,const btVector3 &,btScalar,int,const btVector3 &){}
    virtual void reportErrorWarning(const char *){}
	virtual void draw3dText(const btVector3 &,const char *){}
	virtual void setDebugMode(int p) 
	{
		m = p;
	}
	
	int getDebugMode(void) const {return 3;}
	int m;

};