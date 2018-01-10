#pragma once

#include "Globals.h"
#include <string>
#include <fstream>
#include <iostream>

struct Addon
{
	uint64_t mod_id;
	uint64_t assigned_id;
	std::string name;
};

class Addons
{
	Addons();
	Addon* loadedAddons;
	Addon* getAssignedID(uint64_t mod_ID);
	std::string getObjectFile(ObjectID global_id);
	ObjectID* getAddonObjects(Addon addon);
};

Addons::Addons()
{
}