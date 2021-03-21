#pragma once

class TexturedModel
{
public:
	string path;
	Model* model;

	Texture albedo;
	Texture metallic;
	Texture roughness;
	Texture ao;
	Texture normal;

	TexturedModel(string ModelName)
	{
		model = new Model(ModelName);
		int lastIndex = ModelName.find_last_of('/');
		path = ModelName.substr(0, lastIndex);

		albedo.getTexID(path + "/" + "albedo.png");
		normal.getTexID(path + "/" + "normal.jpg");
		metallic.getTexID(path + "/" + "metallic.jpg");
		roughness.getTexID(path + "/" + "roughness.jpg");
		ao.getTexID(path + "/" + "ao.jpg");
		
	}

	void draw(Shader* SHADER)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, albedo.textId);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, normal.textId);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, metallic.textId);

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, roughness.textId);

		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, ao.textId);

		model->Draw(SHADER);

	}
};
