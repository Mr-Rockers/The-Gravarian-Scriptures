#include "Entity.h"
#include "MathHelper.h"
#include "Input.h"
#include "World.h"

Entity::Entity(int mapID, glm::dvec3 position, double yawAngles, std::string name) :
currentMapID(mapID), position(position), yawAngles(yawAngles), name(name){}

Entity::Entity() : Entity(-1, glm::dvec3(0.0, 0.0, 0.0), 0.0, std::string("entity.unspecified")) {}


int Entity::getCurrentMapID() {
	return this->currentMapID;
}

glm::dvec3 Entity::getEntityPosition() {
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

void Entity::setEntityPosition(glm::dvec3 position, bool safe) {
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

EntityUpdate::EntityUpdate(int mapID, glm::dvec3 position, double yawAngles, std::string name, double moveSpeed):
	Entity(mapID, position, yawAngles, name), currentSpeed(glm::dvec3(0.0, 0.0, 0.0)), moveSpeed(moveSpeed){}
EntityUpdate::EntityUpdate() : EntityUpdate(-1, glm::dvec3(0.0, 0.0, 0.0), 0.0, std::string("entityupdate.unspecified"), 0.0) {}

glm::dvec3 EntityUpdate::getEntityAcceleration() {
	return this->currentSpeed;
}

double EntityUpdate::getMoveSpeed(){
	return this->moveSpeed;
}

void EntityUpdate::setEntitySpeed(glm::dvec3 vector){
	this->currentSpeed = vector;
}

void EntityUpdate::setMoveSpeed(double moveSpeed) {
	this->moveSpeed = moveSpeed;
}

void EntityUpdate::addSpeed(double yawAngles, double speed) {
	this->currentSpeed.x += sin(yawAngles) * speed;
	this->currentSpeed.y += cos(yawAngles) * speed;
}

void EntityUpdate::addSpeed(glm::dvec3 vector) {
	this->currentSpeed += vector;
}

void EntityUpdate::update(double deltaTime) {
	this->position += this->currentSpeed * deltaTime;
}

glm::dvec3 EntityUpdate::getNextPosition(double deltaTime) {
	return this->position + this->currentSpeed * deltaTime;
}

EntityLiving::EntityLiving(int mapID, glm::dvec3 position, double yawAngles, std::string name, double moveSpeed, int startHealth, int maxHealth, std::string displayName) :
EntityUpdate(mapID, position, yawAngles, name, moveSpeed), currentHealth(startHealth), maxHealth(maxHealth), displayName(displayName){}
EntityLiving::EntityLiving() : EntityLiving(-1, glm::dvec3(0.0,0.0,0.0), 0.0, std::string("entityliving.unspecified"), 0.0, 0, 0, std::string("Unspecified EntityLiving")) {}

int EntityLiving::getMaxHealth() {
	return this->maxHealth;
}

int EntityLiving::getHealth() {
	return this->currentHealth;
}

std::string EntityLiving::getDisplayName() {
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

EntityPlayer::EntityPlayer(int mapID, glm::dvec3 position, double yawAngles, std::string name, int startHealth, int maxHealth, std::string displayName) :
EntityLiving(mapID, position, yawAngles, name, 2.0, startHealth, maxHealth, displayName){}
EntityPlayer::EntityPlayer() :
EntityPlayer(-1, glm::dvec3(0.0, 0.0, 0.0), 0.0, std::string("entityplayer.unspecified"), 0, 0, std::string("Unspecified EntityPlayer")){}

void EntityPlayer::update(double deltaTime) {
	LOCALPLAYER.setEntitySpeed(glm::dvec3(0.0, 0.0, 0.0));

	bool startJump = false;
	if (INPUT_JUMP) {
		if (this->getEntityPosition().z <= WORLD_HEIGHT && this->jumpProgress == 0.0) {
			startJump = true;
		}
	}
	if (INPUT_FORWARD) {
		LOCALPLAYER.addSpeed(LOCALPLAYER.getYawAngles(), this->getMoveSpeed());
	}
	if (INPUT_BACKWARD) {
		LOCALPLAYER.addSpeed(LOCALPLAYER.getYawAngles(), -this->getMoveSpeed());
	}
	if (INPUT_LEFT) {
		LOCALPLAYER.addSpeed(LOCALPLAYER.getYawAngles() - M_PI / 2, this->getMoveSpeed());
	}
	if (INPUT_RIGHT) {
		LOCALPLAYER.addSpeed(LOCALPLAYER.getYawAngles() + M_PI / 2, this->getMoveSpeed());
	}

	//basic collision detection
	glm::dvec2 nextPosition = glm::dvec2(LOCALPLAYER.getNextPosition(deltaTime));
	glm::dvec2 oldPosition = glm::dvec2(LOCALPLAYER.getEntityPosition());

	//if player will move within bounds of WORLD_MAP
	if ((int)nextPosition.x < 0 && (int)nextPosition.x > WORLD_MAPSIZE &&
		(int)nextPosition.y < 0 && (int)nextPosition.y > WORLD_MAPSIZE) {
		LOCALPLAYER.setEntitySpeed(glm::dvec3(0.0, 0.0, 0.0));
	}
	
	bool horizontalAllowed = true, verticalAllowed = true;
	if (LOCALPLAYER.checkCollision(glm::dvec2(nextPosition))) {

		//Check if we can move horizonally
		glm::dvec2 checkPosition = glm::dvec2(nextPosition.x, oldPosition.y);
		horizontalAllowed = (!LOCALPLAYER.checkCollision(checkPosition));
		//Check if we can move vertically
		checkPosition = glm::dvec2(oldPosition.x, nextPosition.y);
		verticalAllowed = (!LOCALPLAYER.checkCollision(checkPosition));

	}

	//do jump calculation
	if (startJump || jumpProgress > 0.0) {
		jumpProgress += deltaTime * 5.0;
		startJump = false;
	}
	if (jumpProgress > M_PI) {
		jumpProgress = 0.0;
	}

	LOCALPLAYER.setEntityPosition(glm::dvec3(oldPosition.x, oldPosition.y, WORLD_HEIGHT + sin(jumpProgress) * 0.5), false);

	EntityUpdate::update(deltaTime);
	
	//After position has been updated, correct if the player can move hor/ver or not.
	glm::dvec3 updatedPosition = LOCALPLAYER.getEntityPosition();
	LOCALPLAYER.setEntityPosition(glm::dvec3(
		horizontalAllowed ? updatedPosition.x : oldPosition.x,
		verticalAllowed ? updatedPosition.y : oldPosition.y,
		updatedPosition.z), false);
}

bool EntityPlayer::checkCollision(glm::dvec2 checkPos) {

	double cushionFactor = 0.2;
	for (int x = 0; x < WORLD_MAPSIZE; x++) {
		for (int y = 0; y < WORLD_MAPSIZE; y++) {
			if (WORLD_MAP[x][y] != nullptr) {
				if (WORLD_MAP[x][y]->isSolid()) {
					if (checkPos.x >= (double)x - cushionFactor && checkPos.x <= (double)x + 1.0 + cushionFactor &&
						checkPos.y >= (double)y - cushionFactor && checkPos.y <= (double)y + 1.0 + cushionFactor) {
						return true;
					}
				}
			}
		}
	}

	return false;
}
