#ifndef ENTITY_H
#define ENTITY_H

#include "Includes.h"

class Entity {
protected:
	int currentMapID; //The map ID of the current entity.
	glm::dvec2 position; //The position of the entity.
	double yawAngles; //The angle of the entity in radians.
	std::string name; //The identifying name of the entity.
public:
	Entity();
	Entity(int mapID, glm::dvec2 position, double yawAngles, std::string name);

	virtual int getCurrentMapID();
	virtual glm::dvec2 getEntityPosition();
	virtual double getYawAngles();
	virtual std::string getEntityName();

	virtual void setCurrentMapID(int id);
	virtual void setEntityPosition(glm::dvec2 position, bool safe);
	virtual void setYawAngles(double angle);
	virtual void setEntityName(std::string name);
	virtual void setEntityName(const char* name);
};

class EntityUpdate : public Entity{
protected:
	glm::dvec2 acceleration; //The current acceleration of the entity.
public:
	EntityUpdate();
	EntityUpdate(int mapID, glm::dvec2 position, double yawAngles, std::string name);

	virtual glm::dvec2 getEntityAcceleration();

	virtual void setEntityAcceleration(glm::dvec2 vector);

	virtual void accelerate(double yawAngles, double speed);
	virtual void accelerate(glm::dvec2 vector);

	virtual void update(double deltaTime);
	virtual glm::dvec2 getNextPosition(double deltaTime);
};

class EntityLiving : public EntityUpdate {
protected:
	int maxHealth, currentHealth;
	std::string displayName; //The display name of the entity. (Not the same as the identifying name.)
public:
	EntityLiving();
	EntityLiving(int mapID, glm::dvec2 position, double yawAngles, std::string name, int startHealth, int maxHealth, std::string displayName);

	virtual int getMaxHealth();
	virtual int getHealth();

	virtual std::string getDisplayName();
	
	virtual void setMaxHealth(int health);
	virtual void setHealth(int health);
	virtual void damage(int amount);

	virtual void setDisplayName(std::string name);
	virtual void setDisplayName(const char* name);

	virtual bool isDead();

};

class EntityPlayer : public EntityLiving {
public:
	EntityPlayer();
	EntityPlayer(int mapID, glm::dvec2 position, double yawAngles, std::string name, int startHealth, int maxHealth, std::string displayName);

	virtual void update(double deltaTime);
};

extern EntityPlayer LOCALPLAYER;
#endif