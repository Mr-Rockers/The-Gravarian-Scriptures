#ifndef ENTITY_H
#define ENTITY_H

#include "Includes.h"

class Entity {
protected:
	int currentMapID; //The map ID of the current entity.
	glm::dvec3 position; //The position of the entity.
	double yawAngles; //The angle of the entity in radians.
	std::string name; //The identifying name of the entity.
public:
	Entity();
	Entity(int mapID, glm::dvec3 position, double yawAngles, std::string name);

	virtual int getCurrentMapID();
	virtual glm::dvec3 getEntityPosition();
	virtual double getYawAngles();
	virtual std::string getEntityName();

	virtual void setCurrentMapID(int id);
	virtual void setEntityPosition(glm::dvec3 position, bool safe);
	virtual void setYawAngles(double angle);
	virtual void setEntityName(std::string name);
	virtual void setEntityName(const char* name);
};

class EntityUpdate : public Entity{
protected:
	double moveSpeed;
	double jumpProgress; //For internal use only.
	glm::dvec3 currentSpeed; //The current currentSpeed of the entity.
public:
	EntityUpdate();
	EntityUpdate(int mapID, glm::dvec3 position, double yawAngles, std::string name, double moveSpeed);

	virtual glm::dvec3 getEntityAcceleration();
	virtual double getMoveSpeed();

	virtual void setEntitySpeed(glm::dvec3 vector);
	virtual void setMoveSpeed(double moveSpeed);

	virtual void addSpeed(double yawAngles, double speed);
	virtual void addSpeed(glm::dvec3 vector);

	virtual void update(double deltaTime);
	virtual glm::dvec3 getNextPosition(double deltaTime);
};

class EntityLiving : public EntityUpdate {
protected:
	int maxHealth, currentHealth;
	std::string displayName; //The display name of the entity. (Not the same as the identifying name.)
public:
	EntityLiving();
	EntityLiving(int mapID, glm::dvec3 position, double yawAngles, std::string name, double moveSpeed, int startHealth, int maxHealth, std::string displayName);

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
	EntityPlayer(int mapID, glm::dvec3 position, double yawAngles, std::string name, int startHealth, int maxHealth, std::string displayName);

	virtual void update(double deltaTime);
	virtual bool checkCollision(glm::dvec2 checkPos);
};

extern EntityPlayer LOCALPLAYER;
#endif