#include "Entity.h"
#include "MathHelper.h"
#include "Input.h"
#include "World.h"

Entity::Entity(int mapID, glm::dvec2 position, double yawAngles, std::string name) :
currentMapID(mapID), position(position), yawAngles(yawAngles), name(name){}

Entity::Entity() : Entity(-1, glm::dvec2(0.0, 0.0), 0.0, std::string("entity.unspecified")) {}


int Entity::getCurrentMapID() {
	return this->currentMapID;
}

glm::dvec2 Entity::getEntityPosition() {
	return this->position;
}

double Entity::getYawAngles() {
	return this->yawAngles;
}

std::string Entity::getEntityName() {
	return this->name;
}

void Entity::setCurrentMapID(int id) {
	this->currentMapID = id;
}

void Entity::setEntityPosition(glm::dvec2 position, bool safe) {
	this->position = position;
}

void Entity::setYawAngles(double angle) {
	this->yawAngles = angle;
	WrapAngles(&this->yawAngles);
}

void Entity::setEntityName(std::string name) {
	this->name = name;
}

void Entity::setEntityName(const char* name) {
	this->name = std::string(name);
}

EntityUpdate::EntityUpdate(int mapID, glm::dvec2 position, double yawAngles, std::string name):
	Entity(mapID, position, yawAngles, name), acceleration(glm::dvec2(0.0, 0.0)){}
EntityUpdate::EntityUpdate() : EntityUpdate(-1, glm::dvec2(0.0, 0.0), 0.0, std::string("entityupdate.unspecified")) {}

glm::dvec2 EntityUpdate::getEntityAcceleration() {
	return this->acceleration;
}

void EntityUpdate::setEntityAcceleration(glm::dvec2 vector){
	this->acceleration = vector;
}

void EntityUpdate::accelerate(double yawAngles, double speed) {
	this->acceleration.x += sin(yawAngles) * speed;
	this->acceleration.y += cos(yawAngles) * speed;
}

void EntityUpdate::accelerate(glm::dvec2 vector) {
	this->acceleration += vector;
}

void EntityUpdate::update(double deltaTime) {
	this->position += this->acceleration * deltaTime;
}

glm::dvec2 EntityUpdate::getNextPosition(double deltaTime) {
	return this->position + this->acceleration * deltaTime;
}

EntityLiving::EntityLiving(int mapID, glm::dvec2 position, double yawAngles, std::string name, int startHealth, int maxHealth, std::string displayName) :
EntityUpdate(mapID, position, yawAngles, name), currentHealth(startHealth), maxHealth(maxHealth), displayName(displayName){}
EntityLiving::EntityLiving() : EntityLiving(-1, glm::dvec2(0.0,0.0), 0.0, std::string("entityliving.unspecified"), 0, 0, std::string("Unspecified EntityLiving")) {}

int EntityLiving::getMaxHealth()
{
	return this->maxHealth;
}

int EntityLiving::getHealth()
{
	return this->currentHealth;
}

std::string EntityLiving::getDisplayName()
{
	return this->displayName;
}

void EntityLiving::setMaxHealth(int health) {
	this->maxHealth = health;
}

void EntityLiving::setHealth(int health) {
	this->currentHealth = health;
}

void EntityLiving::damage(int amount) {
	this->currentHealth -= amount;
}

void EntityLiving::setDisplayName(std::string name) {
	this->displayName = name;
}

void EntityLiving::setDisplayName(const char* name) {
	this->displayName = std::string(name);
}

bool EntityLiving::isDead(){
	return currentHealth <= 0;
}

EntityPlayer::EntityPlayer(int mapID, glm::dvec2 position, double yawAngles, std::string name, int startHealth, int maxHealth, std::string displayName) :
EntityLiving(mapID, position, yawAngles, name, startHealth, maxHealth, displayName){}
EntityPlayer::EntityPlayer() :
EntityPlayer(-1, glm::dvec2(0.0, 0.0), 0.0, std::string("entityplayer.unspecified"), 0, 0, std::string("Unspecified EntityPlayer")){}

void EntityPlayer::update(double deltaTime) {
	LOCALPLAYER.setEntityAcceleration(glm::dvec2(0.0, 0.0));
	double playerSpeed = 2.0;

	if (INPUT_FORWARD) {
		LOCALPLAYER.accelerate(LOCALPLAYER.getYawAngles(), playerSpeed);
	}
	if (INPUT_BACKWARD) {
		LOCALPLAYER.accelerate(LOCALPLAYER.getYawAngles(), -playerSpeed);
	}
	if (INPUT_LEFT) {
		LOCALPLAYER.accelerate(LOCALPLAYER.getYawAngles() - M_PI / 2, playerSpeed);
	}
	if (INPUT_RIGHT) {
		LOCALPLAYER.accelerate(LOCALPLAYER.getYawAngles() + M_PI / 2, playerSpeed);
	}

	//basic collision detection
	glm::dvec2 nextPosition = LOCALPLAYER.getNextPosition(deltaTime);
	for (int i = 0; i < WORLD_MAPARRAYSIZE; i++) {
		//if player will accelerate within bounds of WORLD_MAP
		if ((int)nextPosition.x >= 0 && (int)nextPosition.x <= WORLD_MAPSIZE &&
			(int)nextPosition.y >= 0 && (int)nextPosition.y <= WORLD_MAPSIZE) {
			//if player will accelerate into solid tile
			if (WORLD_MAP[(int)(nextPosition.x)][(int)(nextPosition.y)] > 0) {
				LOCALPLAYER.setEntityAcceleration(glm::dvec2(0.0, 0.0));
			}
		}
		else { //otherwise stop the player leaving the WORLD_MAP for now
			   ///Here, we should have the logic for moving the player into a new chunk, but that is a whole other beast to tackle.
			LOCALPLAYER.setEntityAcceleration(glm::dvec2(0.0, 0.0));
		}
	}

	EntityUpdate::update(deltaTime);
}