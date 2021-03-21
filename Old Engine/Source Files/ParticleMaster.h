#pragma once

class ParticleMaster
{
public:
	vector<ParticleSystem*> allSystems;
	Shader* particleShad;

	void init(Shader* lol)
	{
		particleShad = lol;
	}

	void addParticleSystem(ParticleSystem* system)
	{
		system->initSystem(particleShad);
		allSystems.push_back(system);
	}

	void renderAllParticleSystems(glm::mat4 projection , glm::mat4 view, Camera cam)
	{
		for (int i = 0; i < allSystems.size(); i++)
		{
			allSystems.at(i)->render(projection , view, cam);
		
		}

		removeUnusedSystems(allSystems);
	}

private:

	void removeUnusedSystems(vector<ParticleSystem*> systems)
	{
		for (int i = 0; i < systems.size(); i++)
		{
			if (systems.at(i)->particleRenderer.particles.size() == 0)
			{
				systems = findAndRemove(systems.at(i), systems);
				//cout << "removed system" << endl;
			}
		}
	}

	vector<ParticleSystem*> findAndRemove(ParticleSystem* entity, vector<ParticleSystem*> list)
	{
		for (int i = 0; i < list.size(); i++)
		{
			if (entity == list.at(i))
			{
				list.erase(remove(list.begin(), list.end(), entity), list.end());

			}
		}

		return list;
	}

};